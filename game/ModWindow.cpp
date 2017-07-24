#include "ModWindow.h"
#include "cRenderableContainer.h"
#include "cTexturedQuadRenderable.h"
#include "Bloodworks.h"
#include "cTexture.h"
#include "cGlobals.h"
#include "cTextRenderable.h"

ModWindow::ModWindow(Bloodworks *bloodworks) : loginWork(this)
{
	this->bloodworks = bloodworks;

	mainWindow = new cRenderableContainer(bloodworks);
	mainWindow->setAlignment(RenderableAlignment::center);
	mainWindow->setVisible(false);

	cTexturedQuadRenderable *t = new cTexturedQuadRenderable(bloodworks, "resources/level_up_bg.png", "resources/default");
	t->setWorldMatrix(Mat3::scaleMatrix(t->getTexture()->getDimensions().toVec() * 0.55f));
	mainWindow->addRenderable(t);

	cTextRenderable *text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "12\n2\n3\n34 1234 1234567890123456798 123 123 1 1 123457689", 18.0f);
	text->setMaxLength(140.0f);
	mainWindow->addRenderable(text);
	
	cSlaveController* slaveController = coral.getSlaveController();

	if (userDetails.hasStoredUser())
	{
		bloodworks->addSlaveWork(&loginWork);
	}

	bloodworks->addRenderable(mainWindow, GUI + 151);
}

ModWindow::~ModWindow()
{
	SAFE_DELETE(mainWindow);
}

void ModWindow::tick()
{
	loginWork.isDone();
}
