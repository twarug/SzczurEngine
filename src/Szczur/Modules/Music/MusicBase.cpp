#include "MusicBase.hpp"

#include <fstream>

#include <Szczur/Json.hpp>

namespace rat
{ 

	MusicBase::MusicBase(const std::string& fileName, sf::Music& source)
		: _base(source), _name(fileName)
	{
		_timeLeft = getDuration();
		getJsonData();
	}

	void MusicBase::update(float deltaTime) 
	{
		if (getStatus() == sf::SoundSource::Status::Playing && !_finishing) {
			_timeLeft -= deltaTime;
			if (_timeLeft <= _fadeTime)
				_isEnding = true;
		}
	}

	bool MusicBase::isEnding() 
	{
		return _isEnding;
	}

	bool MusicBase::finish(float deltaTime) 
	{

		if (_finishInit) { 	
			_finishInit = false;
			_timeLeft = _fadeTime;
			_finishing = true;
			_isEnding = false;
		}

		_timeLeft -= deltaTime;

		if (_timeLeft >= 0) {
			_base.setVolume((_timeLeft / _fadeTime) * _baseVolume);
			return false;
		}
		
		reset();
		stop();
		return true;
	}

	void MusicBase::start(float deltaTime, float introTime)
	{
		if (_startInit) {
			_timeLeft = getDuration();
			_base.setVolume(0);
			_startInit = false;
		}
		update(deltaTime);
		_base.setVolume((_baseVolume * (getDuration() - _timeLeft)) / introTime);
	}

	void MusicBase::getJsonData() 
	{
		Json json;
		std::ifstream file("res/Music/Music.json");
		
		if(file.is_open()){
			file >> json;
			file.close();
		}

		_bpm = json[_name]["BPM"];
		float numberOfBars = json[_name]["FadeTime"];

		if(numberOfBars > 0) {
			float barTime = 240 / _bpm;
			_fadeTime = barTime * numberOfBars;
		}
	}

	void MusicBase::play() 
	{
		_base.play();
	}

	void MusicBase::pause() 
	{
		_base.pause();
	}

	void MusicBase::stop() 
	{
		_base.stop();
	}

	void MusicBase::setLoop(bool loop)
	{
		_base.setLoop(loop);
	}

	bool MusicBase::getLoop() const
	{
		return _base.getLoop();
	}

	sf::SoundSource::Status MusicBase::getStatus() const 
	{
		return _base.getStatus();
	}

	float MusicBase::getFadeTime() const 
	{
		return _fadeTime;
	}

	float MusicBase::getDuration() const 
	{
		return _base.getDuration().asSeconds();
	}

	float MusicBase::getTimeLeft() const
	{
		return _timeLeft;
	}

	void MusicBase::setTimeLeft(float timeLeft)
	{
		_timeLeft = timeLeft;
	}

	float MusicBase::getVolume() const 
	{
		return _baseVolume;
	}

	void MusicBase::setVolume(float volume) 
	{
		if (volume > 100) volume = 100;
		else if (volume < 0) volume = 0;

		_baseVolume = volume;
		_base.setVolume(volume);
	}
	
	void MusicBase::reset() 
	{
		_timeLeft = getDuration();
		_finishing = false;
		_finishInit = true;
		_startInit = true;
	}

	const std::string& MusicBase::getName() const
	{
		return _name;
	}

}