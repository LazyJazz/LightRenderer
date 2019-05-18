#include "LibZJFPSCounter.h"

void FPSCounter::AddFrame()
{
	int c = clock();
	Frames.push(c);
	while (!Frames.empty())
		if (Frames.front() <= c - CLOCKS_PER_SEC)
			Frames.pop();
		else
			break;
}

int FPSCounter::GetFPS()
{
	return Frames.size();
}

int FPSCounter::FPS()
{
	AddFrame();
	return GetFPS();
}
