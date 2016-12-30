#pragma once

#include <string>

namespace SoLoud 
{
	class Soloud;
}

class cSoundManager
{
	friend class cSoundSample;
	friend class cSoundHandle;
	SoLoud::Soloud *soloud;

public:
	cSoundManager();
	~cSoundManager();
	SoLoud::Soloud* getSoloud() const
	{
		return soloud;
	}
};
