#include "ModWindow.h"
#include "cRenderableContainer.h"
#include "cTexturedQuadRenderable.h"
#include "Bloodworks.h"
#include "cTexture.h"

ModWindow::ModWindow(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;
	if (userDetails.hasStoredUser())
	{
		userDetails.tryLogin();
	}

	mainWindow = new cRenderableContainer(bloodworks);
	mainWindow->setAlignment(RenderableAlignment::center);

	cTexturedQuadRenderable *t = new cTexturedQuadRenderable(bloodworks, "resources/level_up_bg.png", "resources/default");
	t->setWorldMatrix(Mat3::scaleMatrix(t->getTexture()->getDimensions().toVec() * 0.55f));
	mainWindow->addRenderable(t);
	mainWindow->setVisible(false);
	bloodworks->addRenderable(mainWindow, GUI + 151);
}

ModWindow::~ModWindow()
{
	SAFE_DELETE(mainWindow);
}

void ModWindow::tick()
{
}
