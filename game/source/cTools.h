#pragma once

#include "cVec.h"
#include "cMat.h"
#include "cAARect.h"
#include "cVector.h"
#include <stdlib.h>
#include <string>
#include <cassert>
#include <algorithm>
#include <fstream>

#define SAFE_DELETE(x) do{delete x; x = nullptr;} while(0);
#define SAFE_DELETE_ARRAY(x) do{delete[] x; x = nullptr;} while(0);
#ifndef offsetof
#define offsetof(st, m) ((size_t)&(((st *)0)->m))
#endif

#define TEMP_FOLDER "resources/temp/"

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

template <class T>
inline const T& min(const T& a, const T& b)
{
	return a < b ? a : b;
}

template <class T>
inline const T& min(const T& a, const T& b, const T& c)
{
	return a < b ? min(a, c) : min(b, c);
}

template <class T>
inline const T& max(const T& a, const T& b)
{
	return a > b ? a : b;
}


template <class T>
inline const T& max(const T& a, const T& b, const T& c)
{
	return a > b ? max(a, c) : max(b, c);
}


template <class T>
inline void swapt(T& a, T& b)
{
	T temp = a;
	a = b;
	b = temp;
}

#define E_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062

const float pi = (float)E_PI;
const float pi_2 = (float)(E_PI * 2.0);
const float pi_d2 = (float)(E_PI * 0.5);
const float pi_3d2 = (float)(E_PI * 1.5);

const double d_pi = E_PI;
const double d_pi_2 = (E_PI * 2.0);
const double d_pi_d2 = (E_PI * 0.5);
const double d_pi_3d2 = (E_PI * 1.5);

template <class T>
inline const T& clamped(const T& a, const T& min, const T& max)
{
	return a < min ? min : (a > max ? max : a);
}


template <class T>
inline const T& clamp(T& a, const T& min, const T& max)
{
	return a = clamped(a, min, max);
}

template <class T>
inline const T& saturate(T& a)
{
	return a = clamp(a, 0.0f, 1.0f);
}

template <class T>
inline const T& abs(const T& a)
{
	return a > 0 ? a : -a;
}

template <class T>
inline T lerp(const T& a, const T& b, float lerpAmount)
{
	return a + (b - a) * lerpAmount;
}

float angleDiff(float a, float b);

float approachAngle(float moveAngle, float wantedAngle, float rotation);

inline bool textFileRead(std::string path, std::string &data)
{
	data = "";
	std::ifstream t(path);
	if (!t.good()) {
		return false;
	}
	t.seekg(0, std::ios::end);
	data.reserve((unsigned int)t.tellg());
	t.seekg(0, std::ios::beg);

	data.assign((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
	return true;
}

void sleepMS(int ms);

float randFloat();
int randInt();
bool randBool();
int randInt(int limit);
int randInt(int begin, int end);
float randFloat(float limit);
float randFloat(float begin, float end);

inline void fixFilePath(std::string& path)
{
	size_t pos = path.find_last_of("~");
	if (pos != std::string::npos)
	{
		path = path.substr(pos + 1);
	}
	std::replace(path.begin(), path.end(), '\\', '/');
	if (path[0] == '.')
	{
		path = path.substr(1);
	}
	if (path[0] == '/')
	{
		path = path.substr(1);
	}
	if (path[path.size() - 1] == '/')
	{
		path = path.substr(0, path.size() - 1);
	}
}

inline void fixFolderPath(std::string& path)
{
	fixFilePath(path);
	if (path[path.size() - 1] != '/')
	{
		path = path + '/';
	}
}

void printStack();
void printExceptionStack(void* pExp);
void doBreak();

template<class TContainer>
bool beginsWith(const TContainer& input, const TContainer& match)
{
	return input.size() >= match.size()
		&& equal(match.begin(), match.end(), input.begin());
}