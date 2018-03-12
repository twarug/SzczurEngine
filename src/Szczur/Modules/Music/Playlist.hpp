#pragma once

#include <vector>
#include <string>
#include <memory>

#include "MusicBase.hpp"

namespace rat 
{
	class Playlist
	{ 
		using BasePointer_t = std::shared_ptr<MusicBase>;
		using Container_t = std::vector<BasePointer_t>;

	public:

		enum class PlayingMode {
			Random,
			Orderly,
			Single
		};

	private:

		enum class Status {
			Playing, 
			Paused,
			Stopped
		};

		Container_t _playlist;

		unsigned int _currentID;

		BasePointer_t _endingFile = nullptr;
		bool _isFileEnding = false;

		bool hasBeenEverPlayed = false;

		PlayingMode _playingMode = PlayingMode::Random;
		Status _status = Status::Stopped;

		inline static float _globalVolume = 100;

	public:
	
		void clear();

		void update(float deltaTime);
		void stopUpdates();

		bool includes(const std::string& fileName) const;		

		void add(MusicBase&& base);
		void remove(const std::string& fileName);

		BasePointer_t getCurrentPlaying() const;

		void play(unsigned int id, float timeLeft);
		void play(const std::string& fileName = "");
		void play(BasePointer_t prevMusicFile, const std::string& fileName = "");

		void pause();
		void stop();

		void setPlayingMode(PlayingMode mode);

		void setVolume(float volume, const std::string& fileName = "");
		float getVolume(const std::string& fileName) const;

		unsigned int getID(const std::string& fileName) const;

		const Container_t& getContainerRef() const;
	
	private:

		void play(unsigned int id);

		void playNext();

		void unPause();

		unsigned int getRandomId() const;
		
		void setPlaylistToPlaying(int id);
			
	};
}


















