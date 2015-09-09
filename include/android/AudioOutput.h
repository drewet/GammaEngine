#ifndef GE_AUDIOOUTPUT_H
#define GE_AUDIOOUTPUT_H

#include <vector>
#include <string>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include "Time.h"

namespace GE {

class AudioOutput : public GE::Time
{
public:
	typedef enum {
		Front_Left = SL_SPEAKER_FRONT_LEFT,
		Front_Right = SL_SPEAKER_FRONT_RIGHT
	} Speaker;
	AudioOutput( uint32_t sample_rate, uint32_t bps, uint32_t input_channels, bool blocking = false, int speakers = Front_Left | Front_Right );
	~AudioOutput();

	void Pause();
	void Resume();

	void PushData( uint16_t* data, uint32_t size );

	bool isPlaying() const;

	static std::vector< std::pair< int, std::string > > DevicesList();

private:
	static void sBqPlayerCallback( SLAndroidSimpleBufferQueueItf bq, void* p );
	void WaitThreadLock();

	// engine interfaces
	SLObjectItf mEngineObject;
	SLEngineItf mEngine;

	// output mix interfaces
	SLObjectItf mOutputMixObject;

	// buffer queue player interfaces
	SLObjectItf mBqPlayerObject;
	SLPlayItf mBqPlayerPlay;
	SLAndroidSimpleBufferQueueItf mBqPlayerBufferQueue;
	SLEffectSendItf mBqPlayerEffectSend;

	// buffer indexes
	int mCurrentOutputIndex;

	// current buffer half (0, 1)
	int mCurrentOutputBuffer;
	
	// buffers
	short* mOutBuf;
	short* mOutputBuffer[2];

	// size of buffers
	int mOutBufSamples;

	// locks
	int  mLastOutlock;
	int  mOutlock;

	double mTime;
	int mOutchannels;
	int mSr;
	SLuint32 mCpos;
	SLuint32 mBlkpos;
};
}

#endif // GE_AUDIOOUTPUT_H
