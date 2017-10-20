// This class keep texture loaded from files
// It's simple wrapper for sf::Texture with some utilities

#pragma once

#include <iostream>
#include <fstream>

#include <SFML/Graphics.hpp>

namespace rat {
	class Texture {
	private:
		sf::Texture _texture;
		sf::Vector2i _frames; // Max number of horizontal/vertical frames (for sprites)
		
		sf::Vector2i _frameSize;
		
	public:
	// Init
		Texture() :
			_frames(1, 1) {
		}
		Texture(const std::string& filename) {
			loadFromFile(filename, 1, 1);
		}
		Texture(const std::string& filename, int horiz, int vert) {
			loadFromFile(filename, horiz, vert);
		}
		Texture(const std::string& filename, const sf::Vector2i& frames) {
			loadFromFile(filename, frames);
		}
		
	// Setters
		void setFrames(int horiz, int vert) {
			_frames.x = horiz;
			_frames.y = vert;
			_frameSize.x = _texture.getSize().x / horiz;
			_frameSize.y = _texture.getSize().y / vert;
		}
		void setFrames(const sf::Vector2i& frames) {
			_frames = frames;
			_frameSize.x = _texture.getSize().x / _frames.x;
			_frameSize.y = _texture.getSize().y / _frames.y;
		}
		
	// Loading
		bool loadFromFile(const std::string& filename, int horiz, int vert) {
			bool ret = _texture.loadFromFile(filename);
			setFrames(horiz, vert);
			return ret;
		}		
		bool loadFromFile(const std::string& filename, const sf::Vector2i& frames) {
			bool ret = _texture.loadFromFile(filename);
			setFrames(frames);
			return ret;
		}
		
	// Getters
		sf::Vector2i getFrames() const {
			return _frames;
		}
		const sf::Texture& getTexture() const {
			return _texture;
		}
		const sf::Vector2i& getFrameSize() const {
			return _frameSize;
		}
	};
}