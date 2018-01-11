#include "cTools.h"

#include <stdio.h>
#include <fstream>
#include <streambuf>
#include <SDL.h>
#include <algorithm>
#define CURL_STATICLIB 
#include <curl/curl.h>

#ifdef WINDOWS
#undef APIENTRY
#include "windows.h"
#endif

#ifdef _WIN32
#include "StackWalker.h"


std::string lastEntry;

bool hadGLError = false;

class StackWalkerToConsole : public StackWalker
{
protected:
	bool dummyPrint;
	virtual void OnOutput(LPCSTR szText)
	{
		if (dummyPrint == false)
		{
			out << szText;
		}
	}
public:
	StackWalkerToConsole() : StackWalker(RetrieveNone)
	{

	}
	void print(bool dummyPrint)
	{
		this->dummyPrint = dummyPrint;
		if (dummyPrint == false)
		{
			out << "\n\nStack Trace Begin\n--------------------\n";
		}
		StackWalker::ShowCallstack();
		if (dummyPrint == false)
		{
			out << "\n--------------------\nStack Trace End\n";
		}
	}

	void printExceptionStack(EXCEPTION_POINTERS* pExp)
	{
		dummyPrint = false;
		out << "\n\nException Stack Trace Begin\n--------------------\n";
		ShowCallstack(GetCurrentThread(), pExp->ContextRecord);
		out << "\n--------------------\nException Stack Trace End\n";
	}
} stack;

#endif

void fixAngle(float& a)
{
	while (a > pi)
	{
		a -= pi_2;
	}
	while (a < -pi)
	{
		a += pi_2;
	}
}

float angleDiff(float a, float b)
{
	float diff = a - b;
	while (diff > pi)
	{
		diff -= pi_2;
	}
	while (diff < -pi)
	{
		diff += pi_2;
	}
	return diff;
}

float approachAngle(float moveAngle, float wantedAngle, float rotation)
{
	if (wantedAngle != moveAngle)
	{
		float diff = angleDiff(moveAngle, wantedAngle);

		if (diff < -rotation)
		{
			moveAngle += rotation;
		}
		else if (diff > rotation)
		{
			moveAngle -= rotation;
		}
		else
		{
			moveAngle = wantedAngle;
		}
	}

	return moveAngle;
}

void sleepMS(int ms) 
{
#ifdef WINDOWS
	Sleep(ms);
#else
	SDL_Delay(ms);
#endif
}


int randInt()
{
	return rand();
}

float randFloat()
{
	return randInt() / (float)RAND_MAX;
}

bool randBool()
{
	return (randInt() & 1) == 0;
}

int randInt(int limit)
{
	return randInt() % limit;
}

int randInt(int begin, int end)
{
	return randInt() % (end - begin + 1) + begin;
}

float randFloat(float limit)
{
	return randFloat() * limit;
}

float randFloat(float begin, float end)
{
	return randFloat() * (end - begin) + begin;
}

void printStack(bool dummyPrint)
{
#ifdef _WIN32
	stack.print(dummyPrint);
#endif
}

void printExceptionStack(void* pExp)
{
#ifdef _WIN32
	stack.printExceptionStack((EXCEPTION_POINTERS*)pExp);
#endif
}

void doBreak()
{
#ifdef _WIN32
	__debugbreak();
#endif
}

void SendReport(const std::string& message, bool useCopy)
{
	std::string readBuffer;

	out.close();

	std::string report;
	if (useCopy)
	{
		textFileRead(STD_OUTPUT_COPY, report);
	}
	else
	{
		textFileRead(STD_OUTPUT, report);
	}
	CURL *curl = curl_easy_init();
	if (curl)
	{
		struct curl_httppost *formpost = NULL;
		struct curl_httppost *lastptr = NULL;
		struct curl_slist *headerlist = NULL;

		curl_formadd(&formpost,
			&lastptr,
			CURLFORM_COPYNAME, "message",
			CURLFORM_COPYCONTENTS, message.c_str(),
			CURLFORM_END);

		curl_formadd(&formpost,
			&lastptr,
			CURLFORM_COPYNAME, "report",
			CURLFORM_COPYCONTENTS, report.c_str(),
			CURLFORM_END);

		curl_easy_setopt(curl, CURLOPT_URL, "http://bloodworks.enginmercan.com/send_report.php");
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			std::cout << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
		}

		curl_easy_cleanup(curl);
		curl_formfree(formpost);
		curl_slist_free_all(headerlist);
	}
}