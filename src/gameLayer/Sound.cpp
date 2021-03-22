#include "Sound.h"
#include "tools.h"
#include "Settings.h"

#undef max
#undef min

#include <cmath>

#define MinSoundDist 32


#if 0

void SoundManager::setMusicPositions(MapData& mapData) {}

void SoundManager::loadMusic() {}

void SoundManager::setMusicAndEffectVolume(glm::vec2 pos) {}

void SoundManager::updateSoundTransation(float deltaTime) {}

void SoundManager::updateSoundVolume(){}

void SoundManager::stoppMusic() {}

void SoundManager::playSound(int sound) {}

#else

void SoundManager::setMusicPositions(MapData & mapData)
{
	constexpr unsigned short musicTypes[musicTapesCount] =
	{
		Block::musicRed, Block::musicTiki, Block::musicSnow, Block::musicCave, Block::musicIce,
		Block::musicBlue, Block::musicCrimson, Block::musicKhaki, Block::musicDarkGreen, Block::musicGreen, Block::musicLightGray
	};

	for (int i = 0; i < musicTapesCount; i++)
	{
		musicPositions[i].clear();
	}

	// sound effects
	constexpr unsigned short soundTypes[musicEffectsCount] =
	{
		Block::musicEffectGreen,
		Block::musicEffecSnow,
		Block::musicEffecWater,
		Block::musicEffecCave,
	};

	for (int i = 0; i < musicEffectsCount; i++)
	{
		effectsPositions[i].clear();
	}

	for (int y = 0; y < mapData.h; y++)
		for (int x = 0; x < mapData.w; x++)
		{
			auto &b = mapData.get(x, y).type;
			
			for (int i = 0; i < musicTapesCount; i++)
			{
				if(b == musicTypes[i])
				{
					b = Block::none;

					musicPositions[i].push_back({ x * BLOCK_SIZE + BLOCK_SIZE / 2.f, y * BLOCK_SIZE + BLOCK_SIZE / 2.f });
					break;
				}

			}

			for(int i=0; i<musicEffectsCount; i++)
			{
				if(b == soundTypes[i] ||
					(i == musicEffects::musicEffectWater && isWaterMusicSource(b)))
				{
					if(b == soundTypes[i])
					{
						b = Block::none;
					}

					effectsPositions[i].push_back({ x * BLOCK_SIZE + BLOCK_SIZE / 2.f, y * BLOCK_SIZE + BLOCK_SIZE / 2.f });
				}
			}

		}

	for (int i = 0; i < musicTapesCount; i++)
	{
		if(musicPositions[i].size() != 0)
		{
			musicVect[i].setVolume(0);
			musicVect[i].play();
			musicVect[i].currentVolume = 0;
			musicVect[i].desiredVolume = 0;

		}else
		{
			musicVect[i].setVolume(0);
			musicVect[i].stop();
			musicVect[i].currentVolume = 0;
			musicVect[i].desiredVolume = 0;
		}
	}

	for (int i=0; i<musicEffectsCount; i++)
	{
		if(effectsPositions[i].size()!=0)
		{
			effectsVect[i].setVolume(0);
			effectsVect[i].play();
			effectsVect[i].currentVolume = 0;
			effectsVect[i].desiredVolume = 0;
		}else
		{
			effectsVect[i].setVolume(0);
			effectsVect[i].stop();
			effectsVect[i].currentVolume = 0;
			effectsVect[i].desiredVolume = 0;
		}
	
	}
	

}

void SoundManager::loadMusic()
{

	for(int i=0; i<soundEffects::soundEffectCount; i++)
	{
		soundBuffers[i] = LoadSound(soundNames[i]);
	}


	for(int i=0; i<musicTapesCount; i++)
	{
		if(tapesNames[i] != nullptr)
		{
			musicVect[i].m = LoadMusicStream(tapesNames[i]);
			
			if(musicVect[i].m.sampleCount)
			{
				musicVect[i].loaded = true;
			}
		
		}
	}

	for(int i=0; i<musicEffectsCount; i++)
	{
		if(effectsNames[i]!=nullptr)
		{
			effectsVect[i].m = LoadMusicStream(effectsNames[i]);

			if(effectsVect[i].m.sampleCount)
			{
				effectsVect[i].loaded = true;
			}
		}

	}

	updateSoundVolume();
}

float distFunc2(float dist)
{

	dist /= BLOCK_SIZE;

	dist /= 1.5;

	dist -= 6;

	dist = std::max(1.f, dist);
	//shortestDist /= BLOCK_SIZE;


	//min distance:
	if(dist > 15)
	{
		return 0;
	}

	float perc = (1.f * BLOCK_SIZE) / (pow(dist, 2) * 0.04 + 3 + pow(dist, 3) * 0.008);
	perc = std::min(perc, 1.f);
	perc = std::max(perc, 0.f);

	if (perc < 0.02)
	{
		perc = 0;
	}

	return perc * 0.08;
}

void SoundManager::setMusicAndEffectVolume(glm::vec2 pos)
{
	{
		float shortestDist[musicTapesCount];
		for (int i = 0; i < musicTapesCount; i++)
		{
			shortestDist[i] = -1;
		}

		for (int m = 0; m < musicTapesCount; m++)
		{
			if (musicPositions[m].size() == 0) { continue; }

			for (auto& i : musicPositions[m])
			{
				float dist = sqrt(
					(pos.x - i.x) * (pos.x - i.x) +
					(pos.y - i.y) * (pos.y - i.y)
				);

				if (shortestDist[m] < 0)
				{
					shortestDist[m] = dist;
				}
				else
				{
					shortestDist[m] = std::min(shortestDist[m], dist);
				}

			}

			if(shortestDist[m] >  MinSoundDist * BLOCK_SIZE )
			{
				shortestDist[m] = -1;
			}

		}


		float minSoundDist = -1;
		int id = -1;

		for(int i=0; i<musicTapesCount; i++)
		{
			if(shortestDist[i] > 0)
			{
				if(minSoundDist<0)
				{
					minSoundDist = shortestDist[i];
					id = i;
				}else if(minSoundDist > shortestDist[i])
				{
					minSoundDist = shortestDist[i];
					id = i;
				}

			}
		}

		for (int i = 0; i < musicTapesCount; i++)
		{
			if(i == id)
			{
				musicVect[i].desiredVolume = 1;
				//musicVect[i].setVolume(1 * settings.musicVolume);
			}else
			{
				musicVect[i].desiredVolume = 0;
				//musicVect[i].setVolume(0);
			}

			//if (shortestDist[i] > 0)
			//{
			//	float v = distFunc2(shortestDist[i]);
			//	musicVect[i].setVolume(v * settings.musicVolume);
			//};
		}

	}

	// set effect volume
	{
		float shortestDist[musicEffectsCount];
		for(int i=0; i< musicEffectsCount; i++)
		{
			shortestDist[i] = -1;
		}
		
		for(int m=0; m<musicEffectsCount; m++)
		{
			//effectsPositions
			if (effectsPositions[m].size() == 0) { continue; }

			for(auto &i : effectsPositions[m])
			{
				float dist = sqrt(
					(pos.x - i.x) * (pos.x - i.x) +
					(pos.y - i.y) * (pos.y - i.y)
				);

				if (shortestDist[m] < 0)
				{
					shortestDist[m] = dist;
				}
				else
				{
					shortestDist[m] = std::min(shortestDist[m], dist);
				}
			}

		}

		for (int i = 0; i < musicEffectsCount; i++)
		{
			if(shortestDist[i]>0)
			{
				float v = distFunc2(shortestDist[i]);
				effectsVect[i].desiredVolume = v;
			}

		}

	}

}

constexpr float transationSpeed = 0.15;

void SoundManager::updateSoundTransation(float deltaTime)
{
	for(int i=0; i<musicTapesCount; i++)
	{
		if(musicVect[i].loaded && musicPositions[i].size())
		{
			if(std::abs(musicVect[i].desiredVolume - musicVect[i].currentVolume) > transationSpeed * deltaTime)
			{
				if(musicVect[i].currentVolume > musicVect[i].desiredVolume)
				{
					musicVect[i].currentVolume -= transationSpeed * 2 * deltaTime;
					if(musicVect[i].currentVolume < 0)
					{
						musicVect[i].currentVolume = 0;
					}
				}else
				{
					musicVect[i].currentVolume += transationSpeed * deltaTime;
				}
			}else
			{
				musicVect[i].currentVolume = musicVect[i].desiredVolume;
			}

			musicVect[i].setVolume(musicVect[i].currentVolume * settings::getMusicSound());

		}
	
	}

	for (int i = 0; i < musicEffectsCount; i++)
	{
		effectsVect[i].currentVolume = effectsVect[i].desiredVolume;
		effectsVect[i].setVolume(effectsVect[i].currentVolume * settings::getAmbientSound());
	}
}

void SoundManager::updateSoundVolume()
{
	for (int i = 0; i < musicTapesCount; i++)
	{
		musicVect[i].setVolume(musicVect[i].currentVolume * settings::getMusicSound());
		UpdateMusicStream(musicVect[i].m);
	}

	for (int i = 0; i < musicEffectsCount; i++)
	{
		effectsVect[i].setVolume(effectsVect[i].currentVolume * settings::getAmbientSound() );
		UpdateMusicStream(effectsVect[i].m);
	}

	for (int i=0; i< soundEffects::soundEffectCount; i++)
	{
		SetSoundVolume(soundBuffers[i], settings::getAmbientSound() / 7.f);
	}

}

void SoundManager::stoppMusic()
{
	for(int i=0; i<tapes::musicTapesCount; i++)
	{	
		musicVect[i].stop();
	}

	for (int i = 0; i < musicEffectsCount; i++)
	{
		effectsVect[i].stop();
	}

}

#undef PlaySound

void SoundManager::playSound(int sound)
{

	PlaySound(soundBuffers[sound]);

}

#endif // !_debug
