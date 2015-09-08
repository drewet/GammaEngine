#ifndef GE_AUDIOOUTPUT_H
#define GE_AUDIOOUTPUT_H

#include "Time.h"

namespace GE {

class AudioOutput : public GE::Time
{
public:
	typedef enum {
		Front_Left,
		Front_Right
	} Speaker;
	AudioOutput( uint32_t sample_rate, uint32_t bps, uint32_t input_channels, Speaker speakers );
	~AudioOutput();

private:
};
}

#endif // GE_AUDIOOUTPUT_H
