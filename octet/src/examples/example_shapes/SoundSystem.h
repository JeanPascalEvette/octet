
#include "inc\fmod\fmod.hpp"
#include "inc\fmod\fmod_errors.h"
#include <stddef.h>

namespace octet
{

	// Using https://cuboidzone.wordpress.com/2013/07/26/tutorial-implementing-fmod/ and some StackOverflow posts to get the up to date syntax (tutorial is based on old version of FMOD)
	typedef FMOD::Sound* SoundClass;
	class SoundSystemClass
	{
		// Pointer to the FMOD instance
		FMOD::System *m_pSystem;
	public:
		SoundSystemClass()
		{
			if (FMOD::System_Create(&m_pSystem) != FMOD_OK)
			{
				// Report Error
				return;
			}

			int driverCount = 0;
			m_pSystem->getNumDrivers(&driverCount);

			if (driverCount == 0)
			{
				// Report Error
				return;
			}

			// Initialize our Instance with 36 Channels
			m_pSystem->init(36, FMOD_INIT_NORMAL, NULL);
		}
		void createSound(SoundClass *pSound, const char* pFile)
		{
			m_pSystem->createSound(pFile, FMOD_CREATESAMPLE, 0, pSound);
		}
		FMOD::Channel* playSound(SoundClass pSound, bool bLoop = false)
		{
			if (!bLoop)
				pSound->setMode(FMOD_LOOP_OFF);
			else
			{
				pSound->setMode(FMOD_LOOP_NORMAL);
				pSound->setLoopCount(-1);
			}

			FMOD::Channel *channel = NULL;
			FMOD_RESULT result = m_pSystem->playSound(pSound, NULL, false, &channel);
			return channel;
		}

		void releaseSound(SoundClass pSound)
		{
			pSound->release();
		}
	};
}