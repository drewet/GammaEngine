#include <time.h>

#include <mutex>
#include <map>

#define __DBG_CLASS
#include "Debug.h"


namespace GE {


static std::mutex mMutex;
static std::map< pthread_t, std::ostringstream* > mOutput;
static std::map< pthread_t, uint64_t > mTicks;

Debug::Debug()
{
}


Debug::~Debug()
{
}


uint64_t Debug::GetTicks()
{
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	return now.tv_sec + now.tv_nsec/100;
}

} // namespace GE
