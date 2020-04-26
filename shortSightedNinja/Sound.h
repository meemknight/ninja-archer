#pragma once
#include <SFML/Audio.hpp>
#include "mapData.h"

struct SoundManager
{
	struct DataToSave
	{
		float musicVolume = 0.5;
		float ambientVolume = 0.5;
		float buttonsVolume = 0.5;

	}settings;

	enum tapes
	{
		musicRed = 0,
		musicDarkBrown,
		musicWhite,
		musicCave1,
		musicIce,
		musicBlue,
		musicCrimson,
		musicKhaki,
		musicDarkGreen,
		musicLime,
		musicGray, //cave 2 I guess
		musicTapesCount
	};

	static constexpr const char *tapesNames[tapes::musicTapesCount] = 
	{
		"resources//jungle.wav",
		"resources//tikiForest.wav",
		0,
		"resources//cave.wav",
		0,
		0,
		0,
		0,
		0,
		0,
		0,
	};

	enum musicEffects
	{
		musicEffectBirds = 0,
		musicEffectSnowStorm,
		musicEffectWater,
		musicEffectCavem,
		musicEffectsCount
	};

	static constexpr const char *effectsNames[musicEffects::musicEffectsCount] =
	{
		"resources//rainForest.wav",
		0,
		0,
		"resources//water.wav",
	};

	//this is the thing that playes music
	struct MusicStruct
	{
		sf::Music m;
		bool loaded = 0;
		float currentVolume = 0;
		float desiredVolume = 0;
	};

	//this vector holds music players
	MusicStruct musicVect[musicTapesCount] = {};
	MusicStruct effectsVect[musicEffectsCount] = {};

	//this holds sound emitors positions and it has no size, that sound doesn't exist
	std::vector<glm::ivec2> musicPositions[tapes::musicTapesCount] = {};
	std::vector<glm::ivec2> effectsPositions[musicEffects::musicEffectsCount] = {};

	void setMusicPositions(MapData &mapData);

	void loadMusic();

	void setMusicAndEffectVolume(glm::vec2 pos);

};

