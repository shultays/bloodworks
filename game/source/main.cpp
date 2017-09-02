
#include <string>
#include <iostream>

#define GL3_PROTOTYPES 1
#include <GL/glew.h>
#include <SDL.h>

#include "cGlobals.h"
#include "Bloodworks.h"
#include "cPackHelper.h"

std::string programName = "Bloodworks";

SDL_Window *mainWindow;
SDL_GLContext mainContext;

SDL_GameController *controller0 = nullptr;
SDL_Joystick *joystick0 = nullptr;

bool SetOpenGLAttributes();
void CheckSDLError(int line);
void RunGame();
void Cleanup();

bool hasError = false;
Coral coral;

LONG __stdcall CrashHandler(PEXCEPTION_POINTERS pExceptionInfo)
{

	out.close();
	exit(0);
	return EXCEPTION_EXECUTE_HANDLER;
}

void addController(int id)
{
	if (controller0)
	{
		SDL_GameControllerClose(controller0);
	}

	controller0 = SDL_GameControllerOpen(0);
	if (controller0 == nullptr)
	{
		joystick0 = SDL_JoystickOpen(0);
	}
}

bool Init()
{
	srand((int)time((time_t)0));
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0)
	{
		out << "Failed to init SDL\n";
		hasError = true;
		return false;
	}

	int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
	mainWindow = SDL_CreateWindow(programName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		800, 600, flags);

	if (!mainWindow)
	{
		out << "Unable to create window\n";
		CheckSDLError(__LINE__);
		return false;
	}

	SDL_JoystickEventState(true);
	if (SDL_NumJoysticks() > 0)
	{
		addController(0);
	}

	lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::jit, sol::lib::os);

	mainContext = SDL_GL_CreateContext(mainWindow);
	SetOpenGLAttributes();
	glewInit();
	return true;
}

bool SetOpenGLAttributes()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	return true;
}


void InitGame()
{
	coral.init();
	game = new Bloodworks();
	game->initInternal();
}


#ifdef _WIN32
#include "StackWalker.h"

LONG WINAPI ExpFilter(EXCEPTION_POINTERS* pExp, DWORD dwExpCode)
{
	printExceptionStack(pExp);
	return EXCEPTION_EXECUTE_HANDLER;
}
#endif

int runMain()
{
	printStack(true);
	bool hasException = false;

#ifdef _WIN32
	__try
	{
#endif
		if (!Init())
			return -1;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		SDL_GL_SwapWindow(mainWindow);
		if (SDL_GL_SetSwapInterval(-1) != 0)
		{
			SDL_GL_SetSwapInterval(0);
		}
		debugRenderer.init();
		InitGame();

		RunGame();

		Cleanup();
#ifdef _WIN32
	}
	__except (ExpFilter(GetExceptionInformation(), GetExceptionCode()))
	{
		hasException = true;
	}
	if (hasException)
	{
		if (mainWindow)
		{
			SDL_HideWindow(mainWindow);
		}
		return -1;
	}
#endif
	out << "Closing the game...\n";
	return 0;
}

void checkOldOutput()
{
	std::ifstream input;
	input.open(STD_OUTPUT);
	SetUnhandledExceptionFilter(CrashHandler);
	if (input.is_open())
	{
		input.close();
		std::rename(STD_OUTPUT, STD_OUTPUT_COPY);
	}
}


int main(int argc, char *argv[])
{
	checkOldOutput();
	out.open(STD_OUTPUT);
	int ret = runMain();
	out.close();
	if (ret == 0 && hasError == false)
	{
		cPackHelper::deleteFile(STD_OUTPUT);
	}
	else
	{
		if (Coral::isDebuggerPresent())
		{
			getc(stdin);
		}
		else
		{
			fflush(stdin);
			printf("Looks like we have a problem, do you want to send log file? (Y/N)\n");
			char c = getc(stdin);
			if (c == 'Y' || c == 'y')
			{
				printf("Please input how this crash happened and press enter:\n");
				std::string message;
				while (message == "")
				{
					std::getline(std::cin, message);
				}

				SendReport(message);
				printf("\n\nThanks for reporting the crash!\n");
				cPackHelper::deleteFile(STD_OUTPUT);
				getc(stdin);
			}
		}
	}
	return 0;
}

void RunGame()
{
	while (coral.gameRunning)
	{

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_MOUSEMOTION)
			{
				input.setMousePos(Vec2((float)event.motion.x, (float)(coral.windowHeight - event.motion.y)), Vec2((float)event.motion.xrel, (float)-event.motion.yrel));
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				input.pressKey(event.button.button + mouse_start);
			}
			else if (event.type == SDL_MOUSEBUTTONUP)
			{
				input.releaseKey(event.button.button + mouse_start);
			}
			else if (event.type == SDL_MOUSEWHEEL)
			{
				input.mouseWhellMove(event.wheel.x, event.wheel.y);
			}
			else if (event.type == SDL_KEYDOWN)
			{
				input.pressKey(event.key.keysym.scancode);
			}
			else if (event.type == SDL_KEYUP)
			{
				input.releaseKey(event.key.keysym.scancode);
			}
			else if (event.type == SDL_WINDOWEVENT)
			{

				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_RESIZED:
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					coral.windowResized(event.window.data1, event.window.data2);
					game->windowResized(event.window.data1, event.window.data2);
					break;
				}
			}
			

			else if (event.type == SDL_CONTROLLERDEVICEADDED)
			{
				addController(0);
			}


			else if (event.type == SDL_JOYAXISMOTION)
			{
				input.setJoystickAxis(event.jaxis.which, event.jaxis.axis, event.jaxis.value);
			}
			else if (event.type == SDL_JOYBUTTONDOWN)
			{
				if (event.jbutton.button + joystick_0_start <= joystick_0_end)
				{
					input.pressKey(event.jbutton.button + joystick_0_start);
				}
			}
			else if (event.type == SDL_JOYBUTTONUP)
			{
				if (event.jbutton.button + joystick_0_start <= joystick_0_end)
				{
					input.releaseKey(event.jbutton.button + joystick_0_start);
				}
			}

			else if (event.type == SDL_JOYHATMOTION)
			{
				bool up = (event.jhat.value & SDL_HAT_UP) > 0;
				bool down = (event.jhat.value & SDL_HAT_DOWN) > 0;
				bool left = (event.jhat.value & SDL_HAT_LEFT) > 0;
				bool right = (event.jhat.value & SDL_HAT_RIGHT) > 0;

				if (up && input.isKeyUp(joystick_0_button_up))
				{
					input.pressKey(joystick_0_button_up);
				}
				else if (!up && input.isKeyDown(joystick_0_button_up))
				{
					input.releaseKey(joystick_0_button_up);
				}
				if (down && input.isKeyUp(joystick_0_button_down))
				{
					input.pressKey(joystick_0_button_down);
				}
				else if (!down && input.isKeyDown(joystick_0_button_down))
				{
					input.releaseKey(joystick_0_button_down);
				}
				if (left && input.isKeyUp(joystick_0_button_left))
				{
					input.pressKey(joystick_0_button_left);
				}
				else if (!left && input.isKeyDown(joystick_0_button_left))
				{
					input.releaseKey(joystick_0_button_left);
				}
				if (right && input.isKeyUp(joystick_0_button_right))
				{
					input.pressKey(joystick_0_button_right);
				}
				else if (!right && input.isKeyDown(joystick_0_button_right))
				{
					input.releaseKey(joystick_0_button_right);
				}
			}

			
			else if (event.type == SDL_CONTROLLERAXISMOTION)
			{
				input.setJoystickAxis(event.caxis.which, event.caxis.axis, event.caxis.value);
			}
			else if (event.type == SDL_CONTROLLERBUTTONDOWN)
			{
				input.pressKey(event.cbutton.button + joystick_0_start);
			}
			else if (event.type == SDL_CONTROLLERBUTTONUP)
			{
				input.releaseKey(event.cbutton.button + joystick_0_start);
			}

			else if(event.type == SDL_QUIT)
			{
				coral.quitGame();
			}
		}
		coral.tick();
	}
}

void Cleanup()
{
	SAFE_DELETE(game);

	//SDL_JoystickEventState(SDL_DISABLE);
	//if (joystick)
	//{
	//	SDL_JoystickClose(joystick);
	//}
	//
	SDL_GL_DeleteContext(mainContext);
	SDL_DestroyWindow(mainWindow);
	mainWindow = nullptr;
	SDL_Quit();
	debugRenderer.freeAll();
	resources.freeAll();
	coral.clear();
}

void CheckSDLError(int line = -1)
{
	std::string error = SDL_GetError();

	if (error != "")
	{
		out << "SLD Error : " << error << "\n";

		if (line != -1)
			out << "\nLine : " << line << "\n";

		SDL_ClearError();
	}
}