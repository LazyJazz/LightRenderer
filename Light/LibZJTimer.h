#pragma once
#include<Windows.h>
#include<ctime>
class Timer
{
private:
	double time_stamp;
public:
	Timer() :time_stamp(0.0) {};
	void Sleep(double ms);
};