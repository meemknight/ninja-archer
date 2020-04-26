#include "Sound.h"
#include "tools.h"

void SoundManager::setMusicPositions(MapData & mapData)
{
	unsigned short musicTypes[musicTapesCount] =
	{
		Block::musicRed, Block::musicTiki, Block::musicSnow, Block::musicCave, Block::musicIce,
		Block::musicBlue, Block::musicCrimson, Block::musicKhaki, Block::musicDarkGreen, Block::musicGreen, Block::musicLightGray
	};

	for (int i = 0; i < musicTapesCount; i++)
	{
		musicPositions[i].clear();
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
		
		}

	for (int i = 0; i < musicTapesCount; i++)
	{
		if(musicPositions[i].size() != 0)
		{
			glog("yes");
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
	float shortestDistVect[musicTapesCount];
	for(int i=0; i< musicTapesCount; i++)
	{
		shortestDistVect[i] = -1;
	}

	for(int m=0; m< musicTapesCount; m++)
	{
		if (musicPositions[m].size() == 0) { continue; }

		for (auto& i : musicPositions[m])
		{
			float dist = sqrt(
				(pos.x - i.x) * (pos.x - i.x) +
				(pos.y - i.y) * (pos.y - i.y)
			);

			if (shortestDistVect[m] < 0)
			{
				shortestDistVect[m] = dist;
			}
			else
			{
				shortestDistVect[m] = std::min(shortestDistVect[m], dist);
			}

		}

	}

	for (int i = 0; i < musicTapesCount; i++)
	{
		if (shortestDistVect[i] > 0) 
		{
			float v = distFunc2(shortestDistVect[i]);
			musicVect[i].setVolume(v * settings.musicVolume);
		};
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
