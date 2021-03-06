#pragma once
#include "mapData.h"
#include "raudio.h"

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
		
		RESOURCES_PATH "music/jungle.ogg",
		RESOURCES_PATH "music/tikiForest.ogg",
		RESOURCES_PATH "music/snow.ogg",
		RESOURCES_PATH "music/cave.ogg",
		0,//ice
		0,//snow cave ?
		RESOURCES_PATH "music/tension.ogg",//tension
		0,//wood
		0,//dark green
		RESOURCES_PATH "music/forest.ogg",
		RESOURCES_PATH "music/castle.ogg",
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
		RESOURCES_PATH "music/rainForest.wav",
		0,
		RESOURCES_PATH "music/water.wav",
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
		RESOURCES_PATH "music/leaves.wav",
		RESOURCES_PATH "music/pick_up.wav",
	};

	//this is the thing that playes music
	struct MusicStruct
	{
		Music m = {};
		bool loaded = 0;
		float currentVolume = 0;
		float desiredVolume = 0;

		void play() { if (loaded) { PlayMusicStream(m); m.looping = true; } }
		void stop() { if (loaded) StopMusicStream(m); }
		void setVolume(float f) { if (loaded)SetMusicVolume(m, f); }

	};


	Sound  soundBuffers[soundEffects::soundEffectCount] = {};

	//this vector holds music players
	MusicStruct musicVect[musicTapesCount] = {};
	MusicStruct effectsVect[musicEffectsCount] = {};


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

