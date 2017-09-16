#include "cGame.h"
#include "cTools.h"
#include "cGlobals.h"
#include "cResources.h"
#include "cShader.h"
#include "cRenderable.h"
#include "cPostProcess.h"

extern SDL_Window *mainWindow;
extern SDL_GLContext mainContext;

class cRootRenderable : public cRenderable
{
public:
	cRootRenderable() : cRenderable(nullptr)
	{
	}
	virtual void render(bool isIdentity, const Mat3& mat, const AARect& rect) override {}
};

void cGame::initInternal()
{
	init();
}

void cGame::tickInternal()
{
	tick();
}

void cGame::renderInternal()
{
	Vec2 windowSize = getScreenDimensions().toVec();
	windowSize *= cameraZoom;
	float halfWidth = windowSize.w * 0.5f;
	float halfHeight = windowSize.h * 0.5f;

	viewMatrices[(int)RenderableAlignment::world] =
		Mat3::rotationMatrix(cameraAngle)
		.translateBy(-halfWidth - cameraPos.x, -halfHeight - cameraPos.y)
		.scaleBy(1.0f / windowSize.w, 1.0f / windowSize.h)
		.translateBy(0.5f)
		.scaleBy(2.0f);

	Mat3 center = viewMatrices[(int)RenderableAlignment::center] =
		Mat3::translationMatrix(-halfWidth, -halfHeight)
		.scaleBy(1.0f / windowSize.w, 1.0f / windowSize.h)
		.translateBy(0.5f)
		.scaleBy(2.0f);

	Vec2 viewWindowSize = getScreenDimensions().toVec();
	float viewHalfWidth = viewWindowSize.w * 0.5f;
	float viewHalfHeight = viewWindowSize.h * 0.5f;

	Mat3 viewCenter = 
		Mat3::translationMatrix(-viewHalfWidth, -viewHalfHeight)
		.scaleBy(1.0f / viewWindowSize.w, 1.0f / viewWindowSize.h)
		.translateBy(0.5f)
		.scaleBy(2.0f);

	viewMatrices[(int)RenderableAlignment::left] = viewCenter * Mat3::translationMatrix(-1.0f, 0.0f);
	viewMatrices[(int)RenderableAlignment::right] = viewCenter * Mat3::translationMatrix(1.0f, 0.0f);
	viewMatrices[(int)RenderableAlignment::top] = viewCenter * Mat3::translationMatrix(0.0f, 1.0f);
	viewMatrices[(int)RenderableAlignment::bottom] = viewCenter * Mat3::translationMatrix(0.0f, -1.0f);

	viewMatrices[(int)RenderableAlignment::topLeft] = viewCenter * Mat3::translationMatrix(-1.0f, 1.0f);
	viewMatrices[(int)RenderableAlignment::topRight] = viewCenter * Mat3::translationMatrix(1.0f, 1.0f);
	viewMatrices[(int)RenderableAlignment::bottomLeft] = viewCenter * Mat3::translationMatrix(-1.0f, -1.0f);
	viewMatrices[(int)RenderableAlignment::bottomRight] = viewCenter * Mat3::translationMatrix(1.0f, -1.0f);

	int lastPostProcess = -1;
	int nextBackBufferIndex = 1;

	for (int i = postProcesses.size() - 1; i >= 0; i--)
	{
		if (postProcesses[i]->isEnabled())
		{
			lastPostProcess = i;
			break;
		}
	}

	if (lastPostProcess >= 0)
	{
		backBuffer = coral.tempFrameBuffer[0];
	}
	else
	{
		backBuffer = 0;
	}

	resetToBackBuffer();
	bool postProcessDone = lastPostProcess == -1;
	cRenderable *cur = first->next;
	while(cur)
	{
		if (cur->visible && cur->getLevel() < maxRenderLevel)
		{
			cur->render();
		}
		cRenderable *prev = cur;
		cur = cur->next;

		if (postProcessDone == false && (cur == nullptr || cur->level >= postProcessEndLevel))
		{
			postProcessDone = true;
			for (int i = 0; i < postProcesses.size(); i++)
			{
				if (postProcesses[i]->isEnabled())
				{
					glBindFramebuffer(GL_FRAMEBUFFER, i == lastPostProcess ? 0 : coral.tempFrameBuffer[nextBackBufferIndex]);
					lastShader = postProcesses[i]->shader;
					postProcesses[i]->bind();
					glBindBuffer(GL_ARRAY_BUFFER, postProcessQuad);

					lastShader->bindUV(0, 0);
					if (nextBackBufferIndex == 0)
					{
						nextBackBufferIndex = 1;
						glBindTexture(GL_TEXTURE_2D, coral.tempFrameBufferTexture[1]);
					}
					else
					{
						nextBackBufferIndex = 0;
						glBindTexture(GL_TEXTURE_2D, coral.tempFrameBufferTexture[0]);
					}

					lastShader->setTexture0(0);

					glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
				}
			}
		}
	}

	if (maxRenderLevel == INT_MAX)
	{
		debugRenderer.render();
	}

	render();
}

cGame::cGame()
{
	maxRenderLevel = INT_MAX;
	cameraPos.setZero();
	cameraZoom = 1.0f;
	cameraAngle = 0.0f;

	slowdown = 1.0f;

	postProcessEndLevel = 0xFFFFFFF;

	GLfloat vertexData[] =
	{
		-1.0f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f,  -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	};

	glGenBuffers(1, &quad);
	glBindBuffer(GL_ARRAY_BUFFER, quad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	first = new cRootRenderable();
	first->level = -1;
}

cGame::~cGame()
{
	lastShader = nullptr;
	if (first->next)
	{
		assert(!"there are some non-removed renderables");
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
	return IntVec2(coral.windowWidth, coral.windowHeight);
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

void cGame::resetToBackBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, backBuffer);
}

void cGame::addPostProcess(cPostProcess *postProcess, int level)
{
	postProcesses.insert(postProcess, level);
}

void cGame::removePostProcess(cPostProcess *postProcess)
{
	postProcesses.remove(postProcess);
}

void cGame::windowResized(int width, int height)
{

}

Vec2 cGame::getRelativeMousePos(const Vec2& mousePos, RenderableAlignment alignment) const
{
	// todo most of these ignores camera pos

	IntVec2 screenSize = getScreenDimensions();
	switch (alignment)
	{
	case RenderableAlignment::world:
		return mousePos - Vec2(screenSize.w * 0.5f, screenSize.h * 0.5f) + getCameraPos();
	case RenderableAlignment::topLeft:
		return mousePos - Vec2(0.0f, (float)screenSize.h);
	case RenderableAlignment::top:
		return mousePos - Vec2((float)screenSize.w * 0.5f, (float)screenSize.h);
	case RenderableAlignment::topRight:
		return mousePos - Vec2((float)screenSize.w, (float)screenSize.h);
	case RenderableAlignment::left:
		return mousePos - Vec2(0.0f, (float)screenSize.h * 0.5f);
	case RenderableAlignment::center:
		return (mousePos - Vec2(screenSize.w * 0.5f, screenSize.h * 0.5f)) * getCameraZoom();
	case RenderableAlignment::right:
		return mousePos - Vec2((float)screenSize.w, (float)screenSize.h * 0.5f);
	case RenderableAlignment::bottomLeft:
		return mousePos;
	case RenderableAlignment::bottom:
		return mousePos - Vec2(screenSize.w * 0.5f, 0.0f);
	case RenderableAlignment::bottomRight:
		return mousePos - Vec2((float)screenSize.w, 0.0f);
	}
	assert(false);
	return Vec2::zero();
}

AARect cGame::getScreenRect() const
{
	Vec2 screenMin = getCameraPos() - getScreenDimensions().toVec() * 0.5f * getCameraZoom();
	Vec2 screenMax = getCameraPos() + getScreenDimensions().toVec() * 0.5f * getCameraZoom();

	return AARect(screenMin, screenMax);
}
