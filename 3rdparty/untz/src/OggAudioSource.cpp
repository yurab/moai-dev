//
//  OggAudioSource.cpp
//  Part of UNTZ
//
//  Created by Robert Dalton Jr. (bob@retronyms.com) on 06/01/2011.
//  Copyright 2011 Retronyms. All rights reserved.
//

#include "OggAudioSource.h"


OggAudioSource::OggAudioSource()
{
    mpOggInfo = NULL;
	mInFile = 0;
}

OggAudioSource::~OggAudioSource()
{
	close();
}

bool OggAudioSource::init(const RString& path, bool loadIntoMemory)
{
	if(mLoadedInMemory && loadIntoMemory)
		return true;

	mPath = path;
	mInFile = fopen(mPath.c_str(), "rb");

	if(mInFile == NULL)
	{
		std::cerr << "Cannot open " << mPath.c_str() << " for reading..." << std::endl;
		return false;
	}

	// Try opening the given file
	if(ov_open(mInFile, &mOggFile, NULL, 0) != 0)
	{
		std::cerr << "Error opening " << mPath.c_str() << " for decoding..." << std::endl;
		return false;
	}

	// Get some information about the OGG file
	mpOggInfo = ov_info(&mOggFile, -1);

	mLength = ov_time_total(&mOggFile, -1);
	mSampleRate = mpOggInfo->rate;
	mNumChannels = mpOggInfo->channels;
	mBitsPerSample = mpOggInfo->bitrate_upper;
	
	return BufferedAudioSource::init(path, loadIntoMemory);
}

void OggAudioSource::close()
{
    BufferedAudioSource::close();
    
	if(mInFile)
	{
		ov_clear(&mOggFile);
		mInFile = 0;
	}
}

void OggAudioSource::setDecoderPosition(Int64 startFrame)
{
	RScopedLock l(&mDecodeLock);

	int status = ov_pcm_seek(&mOggFile, startFrame * getNumChannels());
	if(startFrame < getLength() * getSampleRate())
		mEOF = false;
}

void OggAudioSource::doneDecoding()
{
	close();
}

double OggAudioSource::getLength() 
{ 
	return mLength;
}

double OggAudioSource::getSampleRate() 
{
	return mSampleRate;
}

UInt32 OggAudioSource::getNumChannels()
{
	return mNumChannels;
}

UInt32 OggAudioSource::getBitsPerSample()
{
	return mBitsPerSample;
}

Int64 OggAudioSource::decodeData(float* buffer, UInt32 numFrames)
{
	RScopedLock l(&mDecodeLock);

	int bitStream;
	float** data = 0;

	long framesRead = ov_read_float(&mOggFile, &data, numFrames, &bitStream);	
	if(framesRead > 0)
	{
		for(int i = 0; i < getNumChannels(); ++i)
		{
			int channels = getNumChannels();
			float *pTemp = &buffer[i];
			for(int j = 0; j < framesRead; ++j)
			{
				*pTemp = data[i][j];
				pTemp += channels;
			}
		}
	}
	else
		mEOF = true;

	return framesRead;
}
