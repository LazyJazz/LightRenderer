#include "LibZJTimer.h"

void Timer::Sleep(double ms)
{
	ms *= CLOCKS_PER_SEC * 0.001;
	time_stamp += ms;
	int wc = time_stamp, nc = clock();
	if (nc > wc)
	{
		time_stamp = nc;
	}
	else if (nc < wc)
	{
		::Sleep(wc-nc);
		while (clock() < wc);
	}
}
