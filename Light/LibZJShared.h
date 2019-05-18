#pragma once
#include<Windows.h>
#include<iostream>
#include<random>


template<typename T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}