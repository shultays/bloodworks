#pragma once

#define MOUSE_START 512
#define KEY_COUNT 520

class cInput
{
private:
	friend class Coral;
	void init();

	
	bool prevKeyStates[KEY_COUNT];
	bool keyStates[KEY_COUNT];
	float keyTime[KEY_COUNT];

public:
	float x, y;

	void pressKey(int key);
	void releaseKey(int key);
	void tick();


	bool isKeyDown(int key)
	{
		return keyStates[key];
	}

	bool isKeyUp(int key)
	{
		return !keyStates[key];
	}

	bool isKeyPressed(int key)
	{
		return keyStates[key] && !prevKeyStates[key];
	}

	bool isKeyReleased(int key)
	{
		return !keyStates[key] && prevKeyStates[key];
	}

	float getKeyTime(int key)
	{
		return keyTime[key];
	}

	bool isMouseDown(int key)
	{
		return isKeyDown(MOUSE_START + key);
	}

	bool isMouseUp(int key)
	{
		return isKeyUp(MOUSE_START + key);
	}

	bool isMousePressed(int key)
	{
		return isKeyPressed(MOUSE_START + key);
	}

	bool isMouseReleased(int key)
	{
		return isKeyReleased(MOUSE_START + key);
	}

	void setMousePos(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

};