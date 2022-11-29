/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#include "RookAudioDataLoadingTask.h"
#include "RookPrivatePCH.h"
#include "Engine.h"
#include "OpenAlSoft.h"
#include "Runtime/Engine/Public/AudioDevice.h"
#include "Runtime/Engine/Public/AudioDecompress.h"
#include "Developer/TargetPlatform/Public/Interfaces/IAudioFormat.h"
#include "Runtime/Engine/Public/VorbisAudioInfo.h"
#if !WITH_OGGVORBIS
#include "Runtime/Engine/Public/ADPCMAudioInfo.h"
#include "Runtime/Engine/Public/OpusAudioInfo.h"
#endif

void RookAudioDataLoadingTask::DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
{
	LoadData();
	AudioAssets.Empty();
}

void RookAudioDataLoadingTask::LoadData()
{
	if (!OpenALSoft::Instance().IsDLLLoaded()) { return; }

	if (AudioAssets.Num() > 0)
	{
		ALenum AudioFormat;
		ALuint AudioBuffer = AL_NONE;
		uint32 WaveID;
		FSoundQualityInfo QualityInfo;

		for (TWeakObjectPtr<USoundWave> SoundWave : AudioAssets)
		{
			if (SoundWave->NumChannels > 1) { continue; }
			WaveID = SoundWave->GetUniqueID();			
			
#if WITH_OGGVORBIS
			FByteBulkData* Bulk(SoundWave->GetCompressedData(TEXT("OGG")));
			if (Bulk)
			{
				if (Bulk->IsAvailableForUse())
				{
					SoundWave->ResourceSize = Bulk->GetBulkDataSize();
					Bulk->GetCopy((void**)&SoundWave->ResourceData, true);
				}
				else
				{
#if WITH_EDITOR
					UE_LOG(RookLog, Warning, TEXT("While loading Audio asset could not use bulk"));
#endif
				}
			}
			else
			{
#if WITH_EDITOR
				UE_LOG(RookLog, Warning, TEXT("While loading Audio asset there was no bulk"));
#endif
			}
			Bulk = nullptr;
#else
			if (SoundWave->IsStreaming())
			{
				SoundWave->InitAudioResource(TEXT("OPUS"));
			}
			else
			{
				SoundWave->InitAudioResource(TEXT("ADPCM"));
			}				
#endif				
			ICompressedAudioInfo* AudioInfo;
#if WITH_OGGVORBIS
			AudioInfo = new FVorbisAudioInfo();
#else
			if (SoundWave->IsStreaming())
			{
				AudioInfo = new FOpusAudioInfo();
			}
			else
			{
				AudioInfo = new FADPCMAudioInfo();
			}				
#endif
			QualityInfo = { 0 };

			if (SoundWave->ResourceData == nullptr)
			{
#if WITH_EDITOR
				UE_LOG(RookLog, Warning, TEXT("While loading resource data was null for %s"), *SoundWave->GetName());
#endif
				return;
			}

			AudioInfo->ReadCompressedInfo(SoundWave->ResourceData, SoundWave->ResourceSize, &QualityInfo);

			uint32 DataSize = SoundWave->RawPCMDataSize;

			uint8* RawData(new uint8[DataSize]);
			AudioInfo->ReadCompressedData(RawData, false, SoundWave->RawPCMDataSize);
			//TODO: make selection between MONO8 and MONO16
			AudioFormat = AL_FORMAT_MONO16;
			ALsizei AudioSampleRate = (int32)SoundWave->GetSampleRateForCurrentPlatform();

			OpenALSoft::Instance().OALGenBuffers((ALuint)1, &AudioBuffer);
			OpenALSoft::Instance().OALBufferData(AudioBuffer, AudioFormat, (void*)RawData, DataSize, AudioSampleRate);
			OpenALSoft::Instance().Buffers.Add(WaveID, AudioBuffer);

			delete[] RawData;
			delete AudioInfo;			
		}
	}
}
