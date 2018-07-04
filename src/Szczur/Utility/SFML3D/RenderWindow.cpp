#include "RenderWindow.hpp"

#include <iostream> // cout
#include <string>

#include <SFML/Window.hpp> // @todo . remove (for Uint32 now)
#include <SFML/Graphics.hpp>
#include <glad.h>
#include <glm/vec2.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <SFML/Window/ContextSettings.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "RenderTarget.hpp"
#include "ShaderProgram.hpp"

namespace sf3d
{

RenderWindow::RenderWindow() 
{}

RenderWindow::RenderWindow(
	sf::VideoMode mode,
	const std::string& title,
	ShaderProgram* program,
	sf::Uint32 style,
	const sf::ContextSettings& settings
)
:	sf::RenderWindow(mode, title, style, settings),
	sf3d::RenderTarget(glm::vec2{mode.width, mode.height}, 45.f, program)
{
	if (!gladLoadGL()) {
		std::cout << "Failed to initialize GLAD!\n";
		return;
	}
	glViewport(0, 0, mode.width, mode.height);
}

void RenderWindow::create(
	sf::VideoMode mode,
	const std::string& title,
	ShaderProgram* program,
	sf::Uint32 style,
	const sf::ContextSettings& settings
)
{
	sf::RenderWindow::create(mode, title, style, settings);
	sf3d::RenderTarget::create(glm::vec2{mode.width, mode.height}, 45.f, program);
	
	if (!gladLoadGL()) {
		std::cout << "Failed to initialize GLAD!\n";
		return;
	}
	glViewport(0, 0, mode.width, mode.height);
}

}
