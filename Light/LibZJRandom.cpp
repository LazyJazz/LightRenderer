#include "LibZJRandom.h"

RandomDevice::RandomDevice()
{
	random_device rd;
	base_random_device = mt19937_64(rd());
}

RandomDevice::RandomDevice(unsigned int seed)
{
	base_random_device = mt19937_64(seed);
	real_d = uniform_real_distribution<>();
	int_d = uniform_int_distribution<unsigned int>();
	longlong_d = uniform_int_distribution<unsigned long long>();
}

double RandomDevice::rand_double()
{
	return real_d(base_random_device);
}

unsigned int RandomDevice::rand_int()
{
	return int_d(base_random_device);
}

unsigned long long RandomDevice::rand_long_long()
{
	return longlong_d(base_random_device);
}

double RandomDevice::rand_double(double L, double R)
{
	return rand_double()* (R - L) + L;
}

unsigned int RandomDevice::rand_int(unsigned int L, unsigned int R)
{
	return rand_int() % (R - L + 1) + L;
}

unsigned long long RandomDevice::rand_long_long(unsigned long long L, unsigned long long R)
{
	return rand_long_long() % (R - L + 1) + L;
}
