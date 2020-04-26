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

					musicPositions[i].push_back({ x,y });
					break;
				}
			}
		
		}

	for (int i = 0; i < musicTapesCount; i++)
	{
		if(musicPositions[i].size() != 0)
		{
			glog("yes");
			musicVect[i].m.setVolume(0);
			musicVect[i].m.play();
			musicVect[i].currentVolume = 0;
			musicVect[i].desiredVolume = 0;

		}else
		{
			musicVect[i].m.setVolume(0);
			musicVect[i].m.stop();
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
			}
		}

	}


}
