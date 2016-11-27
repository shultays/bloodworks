#include "cGame.h"
#include "cTools.h"
#include "cGlobals.h"
#include "cResources.h"
#include "cShader.h"
#include "cRenderable.h"

extern SDL_Window *mainWindow;
extern SDL_GLContext mainContext;

class cRootRenderable : public cRenderable
{
public:
	cRootRenderable() : cRenderable(nullptr)
	{
	}
	virtual void render(bool isIdentity, const Mat3& mat) override {}

};

void cGame::initInternal()
{
	cameraPos.setZero();
	cameraZoom = 1.0f;
	cameraAngle = 0.0f;


	GLfloat vertexData[] =
	{
		-1.0f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f,  -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	};


	//IBO data
	GLuint indexData[] = { 0, 1, 2, 3 };

	//Create VBO
	glGenBuffers(1, &quad);
	glBindBuffer(GL_ARRAY_BUFFER, quad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	first = new cRootRenderable();

	init();
}

void cGame::tickInternal()
{
	tick(timer.getDt());
}

void cGame::renderInternal()
{
	Vec2 windowSize = getScreenDimensions().toVec();
	float halfWidth = windowSize.w * 0.5f;
	float halfHeight = windowSize.h * 0.5f;
	worldViewMatrix = 
		Mat3::translationMatrix(-halfWidth - cameraPos.x, -halfHeight - cameraPos.y)
		.scaleBy(1.0f / windowSize.w, 1.0f / windowSize.h)
		.translateBy(0.5f)
		.scaleBy(2.0f);

	cRenderable *cur = first->next;

	while(cur)
	{
		if (cur->visible)
		{
			cur->render();
		}
		cur = cur->next;
	}
	debugRenderer.render();

	render();
}

cGame::~cGame()
{
	lastShader = nullptr;
	if (first->next)
	{
		assert("there are some non-removed renderables");
	}
	SAFE_DELETE(first);
}

void cGame::setCameraPos(const Vec2& newPos)
{
	cameraPos = newPos;
}

void cGame::setCameraAngle(float angle)
{
	cameraAngle = angle;
}

void cGame::setCameraZoom(float zoom)
{
	cameraZoom = zoom;
}

IntVec2 cGame::getScreenDimensions() const
{
	IntVec2 ret;
	SDL_GetWindowSize(mainWindow, &ret.w, &ret.h);
	return ret;
}

void cGame::addRenderable(cRenderable* renderable, int level)
{
	cRenderable *cur = first;
	renderable->level = level;
	while (cur->next != nullptr && cur->next->level < level)
	{
		cur = cur->next;
	}

	renderable->next = cur->next;
	renderable->prev = cur;
	if (renderable->next)
	{
		renderable->next->prev = renderable;
	}
	cur->next = renderable;
}

void cGame::removeRenderable(cRenderable* renderable)
{
	if (renderable->prev == nullptr)
	{
		return;
	}
	renderable->prev->next = renderable->next;
	if (renderable->next)
	{
		renderable->next->prev = renderable->prev;
	}
}
