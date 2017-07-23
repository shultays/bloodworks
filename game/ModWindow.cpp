#include "ModWindow.h"
#include "cRenderableContainer.h"
#include "cTexturedQuadRenderable.h"
#include "Bloodworks.h"
#include "cTexture.h"
#include "cGlobals.h"

ModWindow::ModWindow(Bloodworks *bloodworks) : loginWork(this)
{
	this->bloodworks = bloodworks;

	mainWindow = new cRenderableContainer(bloodworks);
	mainWindow->setAlignment(RenderableAlignment::center);

	cTexturedQuadRenderable *t = new cTexturedQuadRenderable(bloodworks, "resources/level_up_bg.png", "resources/default");
	t->setWorldMatrix(Mat3::scaleMatrix(t->getTexture()->getDimensions().toVec() * 0.55f));
	mainWindow->addRenderable(t);
	mainWindow->setVisible(false);
	bloodworks->addRenderable(mainWindow, GUI + 151);

	cSlaveController* slaveController = coral.getSlaveController();

	if (userDetails.hasStoredUser())
	{
		bloodworks->addSlaveWork(&loginWork);
	}
}

ModWindow::~ModWindow()
{
	SAFE_DELETE(mainWindow);
}

void ModWindow::tick()
{
	loginWork.isDone();
}
