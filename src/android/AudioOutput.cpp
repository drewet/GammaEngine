#include <stdlib.h>
#include "android/AudioOutput.h"
#include "Thread.h"
#include "Debug.h"

#define ASSERT( result ) \
	if ( result != SL_RESULT_SUCCESS ) { gDebug() << "Assertion failed at line " << __LINE__ << " ! : " << result << "\n"; exit(0); }

using namespace GE;

void AudioOutput::sBqPlayerCallback( SLAndroidSimpleBufferQueueItf bq, void* p )
{
	((AudioOutput*)p)->mOutlock++;
}

void AudioOutput::WaitThreadLock()
{
	while ( mOutlock == mLastOutlock ) {
		Time::Sleep( 1 );
	}
	mLastOutlock = mOutlock;
}

AudioOutput::AudioOutput( uint32_t sample_rate, uint32_t bps, uint32_t input_channels, bool blocking, int speakers )
	: Time()
{
	fDebug( sample_rate, bps, input_channels, blocking, speakers );

	SLresult result;

	mOutchannels = input_channels;
	mSr = sample_rate;
	mOutlock = 0;
	mLastOutlock = 0;
	mOutBufSamples = 8192;
	mOutBuf = (short*)Instance::baseInstance()->Malloc( 2 * mOutBufSamples * sizeof(short) );
	mOutputBuffer[0] = &mOutBuf[0];
	mOutputBuffer[1] = &mOutBuf[mOutBufSamples];

	mCurrentOutputIndex = 0;
	mCurrentOutputBuffer = 0;
	mCpos = 0;
	mBlkpos = 0;
	
	// Create Engine
	result = slCreateEngine( &mEngineObject, 0, nullptr, 0, nullptr, nullptr );
	ASSERT( result );
	result = (*mEngineObject)->Realize( mEngineObject, SL_BOOLEAN_FALSE );
	ASSERT( result );
	result = (*mEngineObject)->GetInterface( mEngineObject, SL_IID_ENGINE, &mEngine );
	ASSERT( result );

//	GetAvailableAudioOutputs
	
	// Open Output Mix
	const SLInterfaceID ids[] = { SL_IID_VOLUME };
	const SLboolean req[] = { SL_BOOLEAN_FALSE };
	result = (*mEngine)->CreateOutputMix( mEngine, &mOutputMixObject, 1, ids, req );
	ASSERT( result );

	result = (*mOutputMixObject)->Realize( mOutputMixObject, SL_BOOLEAN_FALSE );
	ASSERT( result );
	
	SLDataLocator_AndroidSimpleBufferQueue loc_bufq = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2 };
	SLDataFormat_PCM format_pcm = { SL_DATAFORMAT_PCM, 2, sample_rate*1000, SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16, (uint32_t)speakers, SL_BYTEORDER_LITTLEENDIAN };

	SLDataSource audioSrc = { &loc_bufq, &format_pcm };
	SLDataLocator_OutputMix loc_outmix = { SL_DATALOCATOR_OUTPUTMIX, mOutputMixObject };
	SLDataSink audioSnk = { &loc_outmix, nullptr };

	const SLInterfaceID ids1[] = { SL_IID_ANDROIDSIMPLEBUFFERQUEUE };
	const SLboolean req1[] = { SL_BOOLEAN_TRUE };
	result = (*mEngine)->CreateAudioPlayer( mEngine, &mBqPlayerObject, &audioSrc, &audioSnk, 1, ids1, req1 );
	ASSERT( result );
	
	result = (*mBqPlayerObject)->Realize( mBqPlayerObject, SL_BOOLEAN_FALSE );
	ASSERT( result );

	result = (*mBqPlayerObject)->GetInterface( mBqPlayerObject, SL_IID_PLAY, &mBqPlayerPlay );
	ASSERT( result );

	result = (*mBqPlayerObject)->GetInterface( mBqPlayerObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &mBqPlayerBufferQueue );
	ASSERT( result );

	result = (*mBqPlayerBufferQueue)->RegisterCallback( mBqPlayerBufferQueue, sBqPlayerCallback, this );
	ASSERT( result );

	mOutlock++;
	(*mBqPlayerPlay)->SetPlayState( mBqPlayerPlay, SL_PLAYSTATE_PLAYING );
}


AudioOutput::~AudioOutput()
{
}


std::vector< std::pair< int, std::string > > AudioOutput::DevicesList()
{
	std::vector< std::pair< int, std::string > > ret;
	return ret;
}


void AudioOutput::PushData( uint16_t* data, uint32_t size )
{
	fDebug( data, size );
/*
	short *outBuffer, *inBuffer;
	uint32_t i;
	int bufsamps = mOutBufSamples;
	int index = mCurrentOutputIndex;
	if ( bufsamps == 0 ) {
		return;
	}
	outBuffer = mOutputBuffer[ mCurrentOutputBuffer ];

	for ( i = 0; i < size; i++ ) {
		outBuffer[index] = ((short*)data)[i];
		index++;
		if ( index >= mOutBufSamples ) {
			(*mBqPlayerBufferQueue)->Enqueue( mBqPlayerBufferQueue, outBuffer, bufsamps * sizeof(short) );
			mCurrentOutputBuffer = ( mCurrentOutputBuffer + 1 ) % 2;
			index = 0;
			outBuffer = mOutputBuffer[ mCurrentOutputBuffer ];
			WaitThreadLock();
		}
	}
	mCurrentOutputIndex = index;
*/
	(*mBqPlayerBufferQueue)->Enqueue( mBqPlayerBufferQueue, data, size * sizeof(short) );
	WaitThreadLock();
	mTime += (double) size / ( mSr * mOutchannels );
}


void AudioOutput::Pause()
{
	(*mBqPlayerPlay)->SetPlayState( mBqPlayerPlay, SL_PLAYSTATE_PAUSED );
}


void AudioOutput::Resume()
{
	(*mBqPlayerPlay)->SetPlayState( mBqPlayerPlay, SL_PLAYSTATE_PLAYING );
}


bool AudioOutput::isPlaying() const
{
	return true;
}
