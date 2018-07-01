#pragma once

#include "SFML/Audio/SoundBuffer.hpp"

#include "RatSound.hpp"

namespace rat
{
    class SoundBase
    {
        using Second_t = float;
    
    private:
            
        struct
        {
            Second_t beginTime {0};
            Second_t endTime   {0};
        } offset;

        Second_t _length      {0};
        Second_t _playingTime {0};

        std::string _name     {""};
        std::string _fileName {""};

        float _volume {100};
        float _pitch  {1};
            
        sf::SoundBuffer _buffer;
        RatSound _sound;

    public:

        bool init(const std::string& fileName, const std::string& name);

        void play();
        void stop();
        void pause();

        float getVolume() const;
        void setVolume(float volume);

        float getPitch() const;
        void setPitch(float pitch);

        bool isRelativeToListener() const;
        void setRelativeToListener(bool relative);

        float getAttenuation() const; 
        void setAttenuation(float attenuation);

        float getMinDistance() const; 
        void setMinDistance(float minDistance);

        void setPosition(float x, float y, float z);
        sf::Vector3f getPosition() const;

        bool getLoop() const;
        void setLoop(bool loop);

        void setOffset(Second_t beginT, Second_t endT);
        Second_t getLength() const;

        Second_t getBeginTime() const;
        Second_t getEndTime() const;  

        const std::string getName() const;
        std::string getFileName() const;

        void load(const std::string& fileName);

        template <typename T>
		T& getEffect() {
            return _sound.getEffect<T>();
        }

        template <typename T>
    	void cleanEffect() {
            _sound.cleanEffect<T>();
        }

    private:

        bool loadBuffer();
        std::string getPath() const;

    };
}
