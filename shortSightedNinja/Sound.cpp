#include "Sound.h"
#include "tools.h"

#define MinSoundDist 32

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

	for(int i=0; i<musicTapesCount; i++)
	{
		if(tapesNames[i] != nullptr)
		{
			if(musicVect[i].m.openFromFile(tapesNames[i]))
			{
				musicVect[i].loaded = true;
				musicVect[i].m.setLoop(1);

			}
		}
	}

	for(int i=0; i<musicEffectsCount; i++)
	{
		if(effectsNames[i]!=nullptr)
		{
			if(effectsVect[i].m.openFromFile(effectsNames[i]))
			{
				effectsVect[i].loaded = true;
				effectsVect[i].m.setLoop(1);
			}
		}

	}


}

float distFunc2(float dist)
{

	dist /= BLOCK_SIZE;

	dist /= 1.5;

	dist -= 6;

	dist = std::max(1.f, dist);
	//shortestDist /= BLOCK_SIZE;

	float perc = (1.f * BLOCK_SIZE) / (pow(dist, 2) * 0.04 + 3 + pow(dist, 3) * 0.008);
	perc = std::min(perc, 100.f);
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

constexpr float transationSpeed = 0.10;

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
					musicVect[i].currentVolume -= transationSpeed * deltaTime;
				}else
				{
					musicVect[i].currentVolume += transationSpeed * deltaTime;
				}
			}else
			{
				musicVect[i].currentVolume = musicVect[i].desiredVolume;
			}

			musicVect[i].setVolume(musicVect[i].currentVolume * settings.musicVolume);

		}
	
	}

	for (int i = 0; i < musicEffectsCount; i++)
	{
		effectsVect[i].currentVolume = effectsVect[i].desiredVolume;
		effectsVect[i].setVolume(effectsVect[i].currentVolume * settings.ambientVolume);
	}
}

void SoundManager::updateSoundVolume()
{
	for (int i = 0; i < musicTapesCount; i++)
	{
		musicVect[i].setVolume(musicVect[i].currentVolume * settings.musicVolume);
	}

	for (int i = 0; i < musicEffectsCount; i++)
	{
		effectsVect[i].setVolume(effectsVect[i].currentVolume * settings.ambientVolume);
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
