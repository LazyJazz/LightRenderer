#pragma once
#include<ctime>
#include<queue>
using namespace std;
class FPSCounter
{
private:
	queue<clock_t> Frames;
public:
	void AddFrame();
	int GetFPS();
	int FPS();
};