#include "LightLowDiscrepancySequence.h"

unsigned NthPrimeNumber[] = { 2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,51,59,61,67,71,73,79,83,89,97 };

double RadicalInverse(unsigned Base, unsigned i)
{
	double Digit, Radical, Inverse;
	Digit = Radical = 1.0 / (double)Base;
	Inverse = 0.0;
	while (i)
	{
		Inverse += Digit * (double)(i % Base);
		Digit *= Radical;
		i /= Base;
	}
	return Inverse;
}

double Halton(unsigned Dimension, unsigned Index)
{
	return RadicalInverse(NthPrimeNumber[Dimension], Index);
}
