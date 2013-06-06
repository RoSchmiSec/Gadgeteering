#ifdef _WIN32
#include <Windows.h>
#else
#include <time.h>
#endif


#include <iostream>
#include "../Gadgeteering/System.hpp"

using namespace GHI;

void System::Sleep(int time)
{
#ifdef _WIN32
	::Sleep(time);
#else
	usleep(1000 * time);
#endif
}

#ifndef WIN32
long int EpochWhenStarted = 0;
#endif

int System::TimeElapsed()
{
#ifdef _WIN32
	return GetTickCount();
#else
#ifdef _POSIX_TIMERS
	timespec t;
	clockgettime(CLOCK_MONOTONIC, &t);

	return ((t.tv_sec * 1000) + (t.tv_nsec / 1000000));
#else
#error "Your system does not support POSIX compliant timers"
#endif
#endif
}

long System::TimeElapsed64()
{
#ifdef _WIN32
	return GetTickCount64();
#else
#ifdef _POSIX_TIMERS
	timespec t;
	clockgettime(CLOCK_MONOTONIC, &t);

	return ((t.tv_sec * 1000) + (t.tv_nsec / 1000000));
#else
#error "Your system does not support POSIX compliant timers"
#endif
#endif
}