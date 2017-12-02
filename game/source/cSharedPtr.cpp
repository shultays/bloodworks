#include "cSharedPtr.h"

#include "cShader.h"
#include "cFont.h"
#include "cTexture.h"
#include "cSound.h"


void cSharedPtr<cShader>::deleteObject(cShader* object)
{
	delete object;
}

void cSharedPtr<cFont>::deleteObject(cFont* object)
{
	delete object;
}

void cSharedPtr<cTexture>::deleteObject(cTexture* object)
{
	delete object;
}

void cSharedPtr<cSoundSample>::deleteObject(cSoundSample* object)
{
	delete object;
}
