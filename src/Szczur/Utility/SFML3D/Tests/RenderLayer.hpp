#pragma once

#include "Szczur/Utility/SFML3D/ShaderProgram.hpp"
#include "Szczur/Utility/SFML3D/Shader.hpp"
#include "Szczur/Utility/SFML3D/RenderLayer.hpp"
#include "Szczur/Utility/SFML3D/RectangleShape.hpp"
#include "./Fixtures/RenderTargetTest.hpp"
#include "Szczur/Utility/Tests.hpp"

struct SimpleRenderLayerTest : public RenderTargetTest
{
	sf3d::RenderLayer renderLayer;
	
	sf3d::ShaderProgram targetShaderProgram;
	sf3d::ShaderProgram layerShaderProgram;
	
	virtual void SetUp() 
	{
		RenderTargetTest::SetUp();

		// Target
		{
			// Shader
			this->targetShaderProgram.linkShaders(
				sf3d::Shader {sf3d::Shader::Vertex, 	"Assets/Shaders/assemble.vert"},
				sf3d::Shader {sf3d::Shader::Fragment, 	"Assets/Shaders/assemble.frag"}
			);
			this->renderTarget->setDefaultShaderProgram(targetShaderProgram);
		}
		
		// Layer
		{
			// Create
			glEnable(GL_DEPTH_TEST);
			this->renderLayer.create(this->windowModule->getSize());
			this->renderLayer.clear({0u, 0u, 0u, 0u}, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Shader
			this->layerShaderProgram.linkShaders(
				sf3d::Shader {sf3d::Shader::Vertex, 	"Assets/Shaders/model.vert"},
				sf3d::Shader {sf3d::Shader::Fragment, 	"Assets/Shaders/texture.frag"}
			);
			this->renderLayer.setDefaultShaderProgram(layerShaderProgram);

			// Reset position factor
			this->renderLayer.positionFactor = 1.f;
		}
	}

	virtual void TearDown()
	{
		// Draw layer
		glDisable(GL_DEPTH_TEST);
		this->renderTarget->draw(this->renderLayer);

		RenderTargetTest::TearDown();
	}
};

VISUAL_TEST_F(SimpleRenderLayerTest, Clear)
{
	renderLayer.clear({0u, 0u, 200u, 255u}, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

VISUAL_TEST_F(SimpleRenderLayerTest, DrawBasic)
{
	sf3d::RectangleShape object({0.3f, 0.2f});
	renderLayer.draw(object);
}

VISUAL_TEST_F(SimpleRenderLayerTest, DrawMove)
{
	sf3d::RectangleShape object({0.3f, 0.2f});
	object.move({-0.5f, 0.1f, 0.f});
	renderLayer.draw(object);
}

VISUAL_TEST_F(SimpleRenderLayerTest, DrawRotate)
{
	sf3d::RectangleShape object({0.3f, 0.2f});
	object.rotate({30.f, 40.f, 10.f});
	renderLayer.draw(object);
}

// @todo , more tests
