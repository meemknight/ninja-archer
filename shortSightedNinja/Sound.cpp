#include "Sound.h"
#include "tools.h"

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
