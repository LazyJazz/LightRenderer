#pragma once
#include<random>
using namespace std;
class RandomDevice
{
public:
	mt19937_64 base_random_device;
	uniform_real_distribution<> real_d;
	uniform_int_distribution<unsigned int> int_d;
	uniform_int_distribution<unsigned long long> longlong_d;
	RandomDevice();
	RandomDevice(unsigned int seed);
	double rand_double();
	unsigned int rand_int();
	unsigned long long rand_long_long();
	double rand_double(double L, double R);
	unsigned int rand_int(unsigned int L, unsigned int R);
	unsigned long long rand_long_long(unsigned long long L, unsigned long long R);
};