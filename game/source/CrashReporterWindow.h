#pragma once


#include "cGlobals.h"

class Bloodworks;
class cRenderableContainer;
class cTextRenderable;
class cButton;
class cTickBox;


class CrashReportWindow
{
	Bloodworks *bloodworks;

	cButton *send;
	cButton *cancel;
	cTickBox *remember;
	cRenderableContainer *window;
	bool closed;
	bool shouldSend;
	bool remeberChoice;
public:
	bool isClosed() const
	{
		return closed;
	}
	bool sendClicked() const
	{
		return shouldSend;
	}
	bool dontAskAgain() const
	{
		return remeberChoice;
	}
	CrashReportWindow(Bloodworks* bloodworks);
	~CrashReportWindow();
	void tick();
};