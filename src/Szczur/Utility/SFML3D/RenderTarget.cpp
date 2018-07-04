#include "RenderTarget.hpp"

/** @file RenderTarget.cpp
 ** @author Tomasz (Knayder) Jatkowski 
 ** @author Patryk (PsychoX) Ludwikowski <psychoxivi+basementstudios@gmail.com>
 **/

#include <cstdio> // snprintf
#include <iostream> // cout

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad.h>

#include <SFML/Graphics/Color.hpp>

#include "RenderStates.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "VertexArray.hpp"
#include "Drawable.hpp"
#include "LightPoint.hpp"
#include "Linear.hpp"
#include "ShaderProgram.hpp"

namespace sf3d
{

/* Properties */
// DefaultRenderStates
RenderStates RenderTarget::getDefaultRenderStates() const 
{
	return this->defaultStates;
}
void RenderTarget::setDefaultRenderStates(const RenderStates& states)
{
	this->defaultStates = states;
}

void RenderTarget::setDefaultShaderProgram(ShaderProgram* program)
{
	this->defaultStates.shader = program;
}

// Camera
Camera* RenderTarget::getCamera()
{
	return this->camera;
}
const Camera* RenderTarget::getCamera() const
{
	return this->camera;
}
void RenderTarget::setCamera(Camera* camera)
{
	if (camera) {
		this->camera = camera;
	}
	else {
		this->camera = this->defaultCamera;
	}
}
void RenderTarget::setCamera(Camera& camera)
{
	this->setCamera(&camera);
}

// FOV // @todo ? move to Camera?
float RenderTarget::getFOV() const
{
	return this->FOVy;
}
void RenderTarget::setFOV(float fov)
{
	this->FOVy = fov;
	this->FOVx = glm::degrees(
		2 * glm::atan(
			glm::tan(glm::radians(this->FOVy / 2.f)) * 
			(static_cast<float>(this->size.x) / static_cast<float>(this->size.y))
		)
	);
	this->halfFOVxTan = glm::tan(glm::radians(this->FOVx / 2.f));
	this->halfFOVyTan = glm::tan(glm::radians(this->FOVy / 2.f));
	this->updatePerspective();
}

// RenderDistance
void RenderTarget::setRenderDistance(float maxRenderDistance, float minRenderDistance)
{
	this->renderDistance = maxRenderDistance;
	// @todo , minRenderDistance
	this->updatePerspective();
}
float RenderTarget::getRenderDistance() const
{
	return this->renderDistance;
}



/* Operators */
RenderTarget::RenderTarget()
{}

RenderTarget::RenderTarget(const glm::uvec2& size, float FOV, ShaderProgram* program)
{
	this->create(size, FOV, program);
}

RenderTarget::~RenderTarget()
{
	if (this->defaultCamera) {
		delete defaultCamera;
	}
}



/* Methods */
void RenderTarget::create(const glm::uvec2& size, float fov, ShaderProgram* program)
{
	this->size = size;

	if (program) {
		this->setDefaultShaderProgram(program);
	}

	if (!this->defaultCamera) {
		this->camera = this->defaultCamera = new Camera();
	}

	this->setFOV(fov); // It will also update perspective
	
	this->positionFactor = 2.f / static_cast<float>(this->size.y);
}

bool RenderTarget::_setActive([[maybe_unused]] bool state)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
	return true; 
} 

void RenderTarget::updatePerspective()
{
	this->projectionMatrix = glm::perspective(
		glm::radians(this->FOVy), 
		(static_cast<float>(this->size.x) / static_cast<float>(this->size.y)), 
		0.1f, this->renderDistance
	);
}

// Clearing
void RenderTarget::clear(float r, float g, float b, float a, GLbitfield flags)
{
	if (this->_setActive()) {
		glClearColor(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
		glClear(flags);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
void RenderTarget::clear(const sf::Color& color, GLbitfield flags)
{
	if (this->_setActive()) {
		glClearColor(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
		glClear(flags);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

// Drawing drawables
void RenderTarget::draw(const Drawable& drawable, const RenderStates& states)
{
	drawable.draw(*this, states);
}
void RenderTarget::draw(const Drawable& drawable)
{
	this->draw(drawable, this->defaultStates);
}

glm::mat4 ps(glm::mat4 m, float f) // @todo ! move it in vaild place... xd
{
	m[3][0] *= f;
	m[3][1] *= f;
	m[3][2] *= f;
	return m;
}

// Drawing vertices
void RenderTarget::draw(const VertexArray& vertices, const RenderStates& states)
{
	if (vertices.getSize() > 0 && _setActive()) {
		vertices.update();
		
		// Shader selection
		ShaderProgram* shaderProgram = (states.shader ? states.shader : this->defaultStates.shader);
		if (!(shaderProgram && shaderProgram->isVaild())) {
			std::cout << "Error: No shader available!\n";
			return;
		}

		// Shader configuration
		{
			shaderProgram->use(); 
			
			// Model, view. projection matrixes
			shaderProgram->setUniform("model",			ps(states.transform.getMatrix(), this->positionFactor));
			shaderProgram->setUniform("view",			ps(camera->getViewMatrix(), this->positionFactor));
			shaderProgram->setUniform("projection", 	this->projectionMatrix);
			shaderProgram->setUniform("positionFactor", this->positionFactor);

			if (states.texture) { // @todo ? Może dodać `Lightable`, a nie oświetlać tylko oteksturowane...
				shaderProgram->setUniform("isObject", true);
				
				// Material
				{
					// Diffuse
					glActiveTexture(GL_TEXTURE0);
					states.texture->bind();
					//glBindTexture(GL_TEXTURE_2D, );
					shaderProgram->setUniform("material.diffuseTexture", 0);

					// Specular // @todo . specular
					//aderProgram->setUniform("material.specularTexture", ???.texture->getID());
					//aderProgram->setUniform("material.shininess", ???.shininess);				
				}
				
				// Lighting
				shaderProgram->setUniform("viewPosition", camera->getPosition());
				shaderProgram->setUniform("basicAmbient", glm::vec3{0.1f, 0.1f, 0.1f});
				applyLightPoints(shaderProgram);
			}
			else {
				shaderProgram->setUniform("isObject", false);
			}
		}

		// Pass the vertices
		vertices.bind();
		glDrawArrays(vertices.getPrimitiveType(), 0, vertices.getSize());
		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (states.texture)
			states.texture->unbind();
	}
}
void RenderTarget::draw(const VertexArray& vertices)
{
	this->draw(vertices, this->defaultStates);
}

// Interaction
Linear RenderTarget::getLinearByScreenPos(const glm::vec2& pos) const
{
	float x = glm::atan(
		( 2.f * (pos.x / static_cast<float>(this->size.x)) - 1.f) * this->halfFOVxTan
	);

	float y = glm::atan(
		(-2.f * (pos.y / static_cast<float>(this->size.y)) + 1.f) * this->halfFOVyTan
	);

	float siny = glm::sin(y);
	float cosy = glm::cos(y);
	float sinx = glm::sin(x);
	float cosx = glm::cos(x);

	glm::vec3 rotation{
		cosy * sinx,
		siny * cosx,
		-cosy * cosx
	};
	rotation = glm::rotateX(rotation, glm::radians(this->camera->getRotation().x));
	rotation = glm::rotateY(rotation, glm::radians(this->camera->getRotation().y));
	return Linear(this->camera->getPosition(), {
		rotation.x,
		rotation.y,
		rotation.z
	});
}

// Light points
void RenderTarget::resetLightPoints()
{
	this->lightPoints.clear();
}
void RenderTarget::registerLightPoint(LightPoint* lightPoint)
{
	this->lightPoints.push_back(lightPoint);
}
void RenderTarget::applyLightPoints(ShaderProgram* shaderProgram)
{
	unsigned int i = 0;
	for (const LightPoint* lightPoint : this->lightPoints) {
		std::snprintf(this->uniformNameBuffer, 64, "pointLights[%u].position", i); 
		shaderProgram->setUniform(this->uniformNameBuffer, lightPoint->getPosition() * this->positionFactor);
		std::snprintf(this->uniformNameBuffer, 64, "pointLights[%u].color", i);
		shaderProgram->setUniform(this->uniformNameBuffer, lightPoint->getColor());
		std::snprintf(this->uniformNameBuffer, 64, "pointLights[%u].attenuation.constant", i);
		shaderProgram->setUniform(this->uniformNameBuffer, lightPoint->attenuation.constant);
		std::snprintf(this->uniformNameBuffer, 64, "pointLights[%u].attenuation.linear", i); 
		shaderProgram->setUniform(this->uniformNameBuffer, lightPoint->attenuation.linear);
		std::snprintf(this->uniformNameBuffer, 64, "pointLights[%u].attenuation.quadratic", i); 
		shaderProgram->setUniform(this->uniformNameBuffer, lightPoint->attenuation.quadratic);
		std::snprintf(this->uniformNameBuffer, 64, "pointLights[%u].ambientFactor", i); 
		shaderProgram->setUniform(this->uniformNameBuffer, lightPoint->getAmbientFactor());
		std::snprintf(this->uniformNameBuffer, 64, "pointLights[%u].diffuseFactor", i); 
		shaderProgram->setUniform(this->uniformNameBuffer, lightPoint->getDiffuseFactor());
		//d::snprintf(this->uniformNameBuffer, 64, "pointLights[%u].specularFactor", i); // @todo . specular 
		//aderProgram->setUniform(this->uniformNameBuffer, lightPoint->getSpecularFactor());
		i++;
	}
	shaderProgram->setUniform("pointLightsLength", i);
}

}
