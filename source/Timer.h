//----------------------------------------------------------------------------------------
/**
* \file       Timer.h
* \author     Jakub Bokšanský
* \date       2012/07/03
* \brief      Timer
*
*  High Precision Windows Timer.
*
*/
//----------------------------------------------------------------------------------------

#pragma once
#include <windows.h>

class Timer
{
	LARGE_INTEGER lFreq, lStart;

public:
	Timer()
	{
		QueryPerformanceFrequency(&lFreq);
		QueryPerformanceCounter(&lStart);
	}

	inline void Start()
	{
		QueryPerformanceCounter(&lStart);
	}

	inline double Stop()
	{
		LARGE_INTEGER lEnd;
		QueryPerformanceCounter(&lEnd);
		return (double(lEnd.QuadPart - lStart.QuadPart) / lFreq.QuadPart);
	}

};