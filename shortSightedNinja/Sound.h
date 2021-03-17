#pragma once
#include <SFML/Audio.hpp>
#include "mapData.h"

struct SoundManager
{

	enum tapes
	{
		musicRed = 0,
		musicDarkBrown,
		musicWhite,
		musicCave1,
		musicIce,
		musicBlue,
		musicCrimson,	//tension
		musicKhaki,
		musicDarkGreen,
		musicLime, //jungle 1
		musicGray, //cave 2 I guess
		musicTapesCount
	};

	static constexpr const char *tapesNames[tapes::musicTapesCount] = 
	{
		"resources//jungle.wav",
		"resources//tikiForest.wav",
		0,
		"resources//cave.wav",
		0,//ice
		0,//snow cave ?
		"resources//tension.wav",//tension
		0,//wood
		0,//dark green
		"resources//jungle1.wav",
		0,
	};

	enum musicEffects
	{
		musicEffectBirds = 0,
		musicEffectSnowStorm,
		musicEffectWater,
		musicEffectCavem, // gray
		musicEffectsCount
	};

	static constexpr const char *effectsNames[musicEffects::musicEffectsCount] =
	{
		"resources//rainForest.wav",
		0,
		"resources//water.wav",
		0,
	};

	enum soundEffects
	{
		soundEffectGrass = 0,
		soundEffectPickUp,
		soundEffectCount,
	};

	static constexpr const char* soundNames[soundEffects::soundEffectCount] =
	{
		"resources//leaves.wav",
		"resources//pick_up.wav",
	};

	//this is the thing that playes music
	struct MusicStruct
	{
		sf::Music m;
		bool loaded = 0;
		float currentVolume = 0;
		float desiredVolume = 0;

		void play() { if (loaded) { m.play(); m.setLoop(1); } }
		void stop() { if (loaded)m.stop(); }
		void setVolume(float f) { if (loaded)m.setVolume(f * 100); }
	};

#ifndef _DEBUG

	sf::SoundBuffer soundBuffers[soundEffects::soundEffectCount];
	sf::Sound soundPlayer;

	//this vector holds music players
	MusicStruct musicVect[musicTapesCount] = {};
	MusicStruct effectsVect[musicEffectsCount] = {};

#endif // !_DEBUG


	//this holds sound emitors positions and it has no size, that sound doesn't exist
	std::vector<glm::vec2> musicPositions[tapes::musicTapesCount] = {};
	std::vector<glm::vec2> effectsPositions[musicEffects::musicEffectsCount] = {};

	void setMusicPositions(MapData &mapData);

	void loadMusic();

	void setMusicAndEffectVolume(glm::vec2 pos);

	void updateSoundTransation(float deltaTime);
		
	void updateSoundVolume();

	void stoppMusic();

	void playSound(int sound);
}
;

