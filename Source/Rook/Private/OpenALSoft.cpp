/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#include "OpenALSoft.h"
#include "RookPrivatePCH.h"
#include "RookUtils.h"
#include "RookAudioDataLoader.h"
#include "RookAudioDataLoadingTask.h"

OpenALSoft::OpenALSoft()
	: bHasDeviceBeenSet(false)
	, CurrentLiveSourceCount(0)
	, MaximumAvailableAudioChannels(0)
	, bHasReachPlayLimit(false)
	, OALGenBuffers(nullptr)
	, OALBufferData(nullptr)
	, bWasDllLoaded(false)
	, OALGetError(nullptr)
	, OALEnable(nullptr)
	, OALOpenDevice(nullptr)
	, OALCloseDevice(nullptr)
	, OALCreateContext(nullptr)
	, OALDestroyContext(nullptr)
	, OALMakeContextCurrent(nullptr)
	, OALDeleteBuffers(nullptr)
	, OALListener3f(nullptr)
	, OALListenerfv(nullptr)
	, OALGenSources(nullptr)
	, OALDeleteSoruces(nullptr)
	, OALSource3f(nullptr)
	, OALSourcef(nullptr)
	, OALSourcei(nullptr)
	, OALSourceiv(nullptr)
	, OALSource3i(nullptr)
	, OALSourcePlay(nullptr)
	, OALSourceStop(nullptr)
	, OALSourcePause(nullptr)
	, OALIsSource(nullptr)
	, OALIsExtensionPresent(nullptr)
	, OALIsAlcExtensionPresent(nullptr)
	, OALGetString(nullptr)
	, OALGetIntegerv(nullptr)
	, OALGetSourcei(nullptr)
	, OALGetSourcef(nullptr)
	, OALGetEnumValue(nullptr)
	, OALEnableHrtf(nullptr)
	, OALGenEffects(nullptr)
	, OALDeleteEffects(nullptr)
	, OALIsEffect(nullptr)
	, OALGenAuxiliaryEffectSlot(nullptr)
	, OALDeleteAuxiliaryEffectSlot(nullptr)
	, OALIsAuxiliaryEffectSlot(nullptr)
	, OALAuxiliaryEffectSloti(nullptr)
	, OALEffecti(nullptr)
	, OALEffectiv(nullptr)
	, OALEffectf(nullptr)
	, OALEffectfv(nullptr)
	, OALGenFilters(nullptr)
	, OALDeleteFilters(nullptr)
	, OALFilteri(nullptr)
	, OALFilteriv(nullptr)
	, OALFilterf(nullptr)
	, OALFilterfv(nullptr)
	, OALDopplerFactor(nullptr)
	, OALDopplerVelocity(nullptr)
	, OALDistanceModel(nullptr)
	, bCanPlayAudio(false)
	, TemporaryAudioSourcePosition(FVector::ZeroVector)
	, TemporaryAudioGain(0.0f)
	, EAXSlotsCapacity(4)
	, VolumeMultiplier(1.0f)
	, DopplerFactor(5.0f)
	, DataLoader(nullptr)
{
	ProcessDLL();

	if (bWasDllLoaded)
	{
		SetAudioDeviceAndCurrentContext();
		CreateAudioSourcePool();
		CreateBandpassPool();
		CreateLowpassPool();
	}
}

OpenALSoft& OpenALSoft::Instance()
{
	static OpenALSoft PointerOALSoft;
	return PointerOALSoft;
}

void OpenALSoft::ProcessDLL()
{
	FString DLLPath = FPaths::EngineDir();
	DLLPath += "Binaries/ThirdParty/OpenAL/1.17.2/";
	bool bUseEnginePath = true;
	//use only if DLL is not in engine path! If You want to use it in shipped 
	//version, You have to place it there. 
	FString BackUpDLLPath = FPaths::ProjectPluginsDir();

#if PLATFORM_WINDOWS & PLATFORM_64BITS
	DLLPath += "Win64/OpenALSoft.dll";
	BackUpDLLPath += "/Rook/OpenALSoftDll/WIN64/OpenALSoft.1.17.2.dll";
#elif PLATFORM_WINDOWS & PLATFORM_32BITS
	//win32 dll
#elif PLATFORM_MAC
	//mac dll
#elif PLATFORM_LINUX
	//linux dll
#else
	= "";
#   if !PLATFORM_LINUX
	UE_LOG(RookLog, Warning, TEXT("No Open AL Soft DLL was found."));
#   endif
	return;
#endif

#if WITH_EDITOR
	bUseEnginePath = FPaths::FileExists(DLLPath);
#endif
	
	OALDLLHandler = bUseEnginePath ? FPlatformProcess::GetDllHandle(*DLLPath) : FPlatformProcess::GetDllHandle(*BackUpDLLPath);
	
	if (OALDLLHandler) 
	{
		static const FString ErrorName = "alGetError";
		static const FString EnableName = "alEnable";

		static const FString OpenDeviceName = "alcOpenDevice";
		static const FString CloseDeviceName = "alcCloseDevice";
		static const FString CreateContextName = "alcCreateContext";
		static const FString DestroyContextName = "alcDestroyContext";
		static const FString MakeContextCurrentName = "alcMakeContextCurrent";

		static const FString GenBuffersName = "alGenBuffers";
		static const FString DeleteBuffersName = "alDeleteBuffers";
		static const FString BufferName = "alBufferData";

		static const FString Listener3fName = "alListener3f";
		static const FString ListenerfvName = "alListenerfv";

		static const FString GenSourcesName = "alGenSources";
		static const FString DeleteSourcesName = "alDeleteSources";
		static const FString Soruce3fName = "alSource3f";
		static const FString SorucefName = "alSourcef";
		static const FString SoruceivName = "alSourceiv";
		static const FString SoruceiName = "alSourcei";
		static const FString Soruce3iName = "alSource3i";
		static const FString SorucePlayName = "alSourcePlay";
		static const FString SoruceStopName = "alSourceStop";
		static const FString SorucePauseName = "alSourcePause";
		static const FString IsSourceName = "alIsSource";

		static const FString IsExtensionPresentName = "alIsExtensionPresent";
		static const FString IsAlcExtensionPresentName = "alcIsExtensionPresent";

		static const FString GetStringName = "alcGetString";
		static const FString GetIntegervName = "alcGetIntegerv";
		static const FString GetSourceiName = "alGetSourcei";
		static const FString GetSourcefName = "alGetSourcef";
		static const FString GetEnumValueName = "alGetEnumValue";

		static const FString HrtfName = "alcDeviceEnableHrtf";

		static const FString GenEffectName = "alGenEffects";
		static const FString DeleteEffectName = "alDeleteEffects";
		static const FString IsEffectName = "alIsEffect";
		static const FString AlGenAuxiliaryEffectSlotsName = "alGenAuxiliaryEffectSlots";
		static const FString DeleteAuxiliaryEffectSlotsName = "alDeleteAuxiliaryEffectSlots";
		static const FString IsAuxiliaryEffectSlotName = "alIsAuxiliaryEffectSlot";
		static const FString AuxiliaryEffectSlotiName = "alAuxiliaryEffectSloti";

		static const FString EffectiName = "alEffecti";
		static const FString EffectivName = "alEffectiv";
		static const FString EffectfName = "alEffectf";
		static const FString EffectfvName = "alEffectfv";

		static const FString GenFiltersName = "alGenFilters";
		static const FString DeleteFiltersName = "alDeleteFilters";
		static const FString FilteriName = "alFilteri";
		static const FString FilterivName = "alFilteriv";
		static const FString FilterfName = "alFilterf";
		static const FString FilterfvName = "alFilterfv";

		static const FString DopplerFactorName = "alDopplerFactor";
		static const FString DopplerVelocityName = "alDopplerVelocity";

		static const FString DistanceModelName = "alDistanceModel";
		static const FString SoruceDistanceModelName = "alSourceDistanceModel";

		OALGetError = (DefAlGetError)FPlatformProcess::GetDllExport(OALDLLHandler, *ErrorName);
		OALEnable = (DefAlEnable)FPlatformProcess::GetDllExport(OALDLLHandler, *EnableName);

		OALOpenDevice = (DefAlcOpenDevice)FPlatformProcess::GetDllExport(OALDLLHandler, *OpenDeviceName);
		OALCloseDevice = (DefAlcCloseDevice)FPlatformProcess::GetDllExport(OALDLLHandler, *CloseDeviceName);
		OALCreateContext = (DefAlcCreateContext)FPlatformProcess::GetDllExport(OALDLLHandler, *CreateContextName);
		OALDestroyContext = (DefAlcDestroyContext)FPlatformProcess::GetDllExport(OALDLLHandler, *DestroyContextName);
		OALMakeContextCurrent = (DefAlcMakeContextCurrent)FPlatformProcess::GetDllExport(OALDLLHandler, *MakeContextCurrentName);

		OALGenBuffers = (DefAlGenBuffers)FPlatformProcess::GetDllExport(OALDLLHandler, *GenBuffersName);
		OALDeleteBuffers = (DefAlDeleteBuffers)FPlatformProcess::GetDllExport(OALDLLHandler, *DeleteBuffersName);
		OALBufferData = (DefAlBufferData)FPlatformProcess::GetDllExport(OALDLLHandler, *BufferName);

		OALListener3f = (DefAlListener3f)FPlatformProcess::GetDllExport(OALDLLHandler, *Listener3fName);
		OALListenerfv = (DefAlListenerfv)FPlatformProcess::GetDllExport(OALDLLHandler, *ListenerfvName);

		OALGenSources = (DefAlGenSources)FPlatformProcess::GetDllExport(OALDLLHandler, *GenSourcesName);
		OALDeleteSoruces = (DefAlDeleteSources)FPlatformProcess::GetDllExport(OALDLLHandler, *DeleteSourcesName);
		OALSource3f = (DefAlSource3f)FPlatformProcess::GetDllExport(OALDLLHandler, *Soruce3fName);
		OALSourcef = (DefAlSourcef)FPlatformProcess::GetDllExport(OALDLLHandler, *SorucefName);
		OALSourcei = (DefAlSourcei)FPlatformProcess::GetDllExport(OALDLLHandler, *SoruceiName);
		OALSourceiv = (DefAlSourceiv)FPlatformProcess::GetDllExport(OALDLLHandler, *SoruceivName);
		OALSource3i = (DefAlSource3i)FPlatformProcess::GetDllExport(OALDLLHandler, *Soruce3iName);
		OALSourcePlay = (DefAlSourcePlay)FPlatformProcess::GetDllExport(OALDLLHandler, *SorucePlayName);
		OALSourceStop = (DefAlSourcePlay)FPlatformProcess::GetDllExport(OALDLLHandler, *SoruceStopName);
		OALSourcePause = (DefAlSourcePause)FPlatformProcess::GetDllExport(OALDLLHandler, *SorucePauseName);
		OALIsSource = (DefAlIsSource)FPlatformProcess::GetDllExport(OALDLLHandler, *IsSourceName);

		OALIsExtensionPresent = (DefAlIsExtensionPresent)FPlatformProcess::GetDllExport(OALDLLHandler, *IsExtensionPresentName);
		OALIsAlcExtensionPresent = (DefAlcIsExtensionPresent)FPlatformProcess::GetDllExport(OALDLLHandler, *IsAlcExtensionPresentName);

		OALGetString = (DefAlcGetString)FPlatformProcess::GetDllExport(OALDLLHandler, *GetStringName);
		OALGetIntegerv = (DefAlcGetIntegerv)FPlatformProcess::GetDllExport(OALDLLHandler, *GetIntegervName);
		OALGetSourcei = (DefAlGetSourcei)FPlatformProcess::GetDllExport(OALDLLHandler, *GetSourceiName);
		OALGetSourcef = (DefAlGetSourcef)FPlatformProcess::GetDllExport(OALDLLHandler, *GetSourcefName);
		OALGetEnumValue = (DefAlGetEnumValue)FPlatformProcess::GetDllExport(OALDLLHandler, *GetEnumValueName);

		OALEnableHrtf = (DefAlcDeviceEnableHrtf)FPlatformProcess::GetDllExport(OALDLLHandler, *HrtfName);

		OALGenEffects = (DefAlGenEffects)FPlatformProcess::GetDllExport(OALDLLHandler, *GenEffectName);
		OALDeleteEffects = (DefAlDeleteEffects)FPlatformProcess::GetDllExport(OALDLLHandler, *DeleteEffectName);
		OALIsEffect = (DefAlIsEffect)FPlatformProcess::GetDllExport(OALDLLHandler, *IsEffectName);
		OALGenAuxiliaryEffectSlot = (DefAlGenAuxiliaryEffectSlots)FPlatformProcess::GetDllExport(OALDLLHandler, *AlGenAuxiliaryEffectSlotsName);
		OALDeleteAuxiliaryEffectSlot = (DefAlDeleteAuxiliaryEffectSlots)FPlatformProcess::GetDllExport(OALDLLHandler, *DeleteAuxiliaryEffectSlotsName);
		OALIsAuxiliaryEffectSlot = (DefAlIsAuxiliaryEffectSlot)FPlatformProcess::GetDllExport(OALDLLHandler, *IsAuxiliaryEffectSlotName);
		OALAuxiliaryEffectSloti = (DefAlAuxiliaryEffectSloti)FPlatformProcess::GetDllExport(OALDLLHandler, *AuxiliaryEffectSlotiName);

		OALEffecti = (DefAlEffecti)FPlatformProcess::GetDllExport(OALDLLHandler, *EffectiName);
		OALEffectiv = (DefAlEffectiv)FPlatformProcess::GetDllExport(OALDLLHandler, *EffectivName);
		OALEffectf = (DefAlEffectf)FPlatformProcess::GetDllExport(OALDLLHandler, *EffectfName);
		OALEffectfv = (DefAlEffectfv)FPlatformProcess::GetDllExport(OALDLLHandler, *EffectfvName);

		OALGenFilters = (DefAlGenFilters)FPlatformProcess::GetDllExport(OALDLLHandler, *GenFiltersName);
		OALDeleteFilters = (DefAlDeleteFilters)FPlatformProcess::GetDllExport(OALDLLHandler, *DeleteFiltersName);
		OALFilteri = (DefAlFilteri)FPlatformProcess::GetDllExport(OALDLLHandler, *FilteriName);
		OALFilteriv = (DefAlFilteriv)FPlatformProcess::GetDllExport(OALDLLHandler, *FilterivName);
		OALFilterf = (DefAlFilterf)FPlatformProcess::GetDllExport(OALDLLHandler, *FilterfName);
		OALFilterfv = (DefAlFilterfv)FPlatformProcess::GetDllExport(OALDLLHandler, *FilterfvName);


		OALDopplerFactor = (DefAlDopplerFactor)FPlatformProcess::GetDllExport(OALDLLHandler, *DopplerFactorName);
		OALDopplerVelocity = (DefAlDopplerVelocity)FPlatformProcess::GetDllExport(OALDLLHandler, *DopplerVelocityName);

		OALDistanceModel = (DefAlDistanceModel)FPlatformProcess::GetDllExport(OALDLLHandler, *DistanceModelName);
		bWasDllLoaded = true;
		UE_LOG(RookLog, Warning, TEXT("OpenAL Soft: DLL has been loaded."));
	}
	else 
	{
		UE_LOG(RookLog, Warning, TEXT("OpenAL Soft: DLL has NOT been loaded."));
	}
}

bool OpenALSoft::IsDLLLoaded() const 
{
	return bWasDllLoaded;
}

void OpenALSoft::FreeOpenALDll() 
{
	bWasDllLoaded = false;
	FPlatformProcess::FreeDllHandle(OALDLLHandler);
}

bool OpenALSoft::Play(const FAudioSourceModel SourceData) 
{
	if (AudioSourcesPool.Num() > 0) 
	{
		if (bHasReachPlayLimit) { bHasReachPlayLimit = false; }				
	}
	else 
	{
		bHasReachPlayLimit = true;
		return false;
	}

	bool bShouldPlay = false;
	bool bHasBufferData = false;
	bool bHasAudioSourcePlayed = false;

	uint32 TemporaryAudioAssetUID = 0;
	ALuint TemporaryBuffer = AL_NONE;

	if (SourceData.CurrentAudioSourceAsset.IsValid()) 
	{
		TemporaryAudioAssetUID = SourceData.CurrentAudioSourceAsset->GetUniqueID();
		bHasBufferData = Buffers.Contains(TemporaryAudioAssetUID);

		if (bHasBufferData) { bShouldPlay = Buffers[TemporaryAudioAssetUID] != 0; }			
		
		if (bShouldPlay) { TemporaryBuffer = Buffers[TemporaryAudioAssetUID]; }				
	}

	if (bHasBufferData) 
	{
		if (bShouldPlay) 
		{
			TemporaryAudioSource = AudioSourcesPool[0];
			AudioSourcesPool.RemoveAt(0);

			TemporaryAudioSourcePosition = SourceData.AudioSourceLocation;
			TemporaryAudioGain = SourceData.AudioSourceGain;

			TemporaryAudioGain = TemporaryAudioGain < 0.005f ? 0.005f : TemporaryAudioGain;

			SetAudioSourceData();

			OALSourcei(TemporaryAudioSource, AL_BUFFER, TemporaryBuffer);

			ALenum errorType = OALGetError();

			if (!CatchError()) 
			{
				AudioSourceGain.Add(SourceData.AudioSourceID, TemporaryAudioGain);

				if (!AudioSources.Contains(SourceData.AudioSourceID)) { AudioSources.Add(SourceData.AudioSourceID, TemporaryAudioSource); }
								
				OALSourcei(TemporaryAudioSource, AL_LOOPING, AL_FALSE);
				
				if (SourceData.bUseRandomPitch) { OALSourcef(TemporaryAudioSource, AL_PITCH, SourceData.AudioSourceRandomPitch); }
								
				if (SourceData.AudioSourceEAX != EEAX::None) 
				{
					if (SourceData.AudioSourceEAXGain > 0.0f) 
					{
						SetEAXOnAudioSource(SourceData.AudioSourceEAX, SourceData.AudioSourceID, SourceData.AudioSourceEAXGain);
					}
					else 
					{
						SetEAXOnAudioSource(SourceData.AudioSourceEAX, SourceData.AudioSourceID);
					}
				}

				OALSourcePlay(TemporaryAudioSource);
				CurrentLiveSourceCount = AudioSources.Num();
				bHasAudioSourcePlayed = true;
				CatchError("playing source");
			}
			else 
			{
				if (SourceError.Contains(SourceData.AudioSourceID)) 
				{
					OALSourcei(TemporaryAudioSource, AL_BUFFER, 0);
					OALDeleteSoruces((ALuint)1, &TemporaryAudioSource);
					OALDeleteBuffers((ALuint)1, &TemporaryBuffer);
					TemporaryAudioSource = AL_NONE;
					SourceError[SourceData.AudioSourceID] = true;
				}
				else
				{
					SourceError.Add(SourceData.AudioSourceID, true);
				}

				CatchError("setting up source");
			}
		}
		else
		{
			LoadAudioAsset(SourceData.CurrentAudioSourceAsset, false);
		}
	}
	else
	{
		LoadAudioAsset(SourceData.CurrentAudioSourceAsset, true);
	}

	if (CatchError()) { bHasAudioSourcePlayed = false; }		
	
	return bHasAudioSourcePlayed;
}

void OpenALSoft::PlayAt(const uint32 AudioSourceUID, const float Seconds) 
{
	if (bWasDllLoaded && AudioSources.Contains(AudioSourceUID) && bCanPlayAudio) 
	{
		OALSourceStop(AudioSources[AudioSourceUID]);
		OALSourcef(AudioSources[AudioSourceUID], AL_SEC_OFFSET, Seconds);
		OALSourcePlay(AudioSources[AudioSourceUID]);
	}
}

void OpenALSoft::PlayAfterPause(const uint32 AudioSourceUID) 
{
	if (AudioSources.Contains(AudioSourceUID) && OALIsSource(AudioSources[AudioSourceUID]))
	{
		OALSourcePlay(AudioSources[AudioSourceUID]);
	}			
}

void OpenALSoft::Stop(const uint32 AudioSourceUID) 
{
	if (bWasDllLoaded && AudioSources.Contains(AudioSourceUID)) { OALSourceStop(AudioSources[AudioSourceUID]); }			
}

void OpenALSoft::Pause(const uint32 AudioSourceUID) 
{
	if (bWasDllLoaded && AudioSources.Contains(AudioSourceUID) && bCanPlayAudio) { OALSourcePause(AudioSources[AudioSourceUID]); }		
}

void OpenALSoft::ChangeAudioSourceGain(const uint32 AudioSourceUID, float Gain) 
{
	if (bWasDllLoaded && AudioSources.Contains(AudioSourceUID) && AudioSourceGain.Contains(AudioSourceUID)) 
	{
		bool bProceed = Gain != AudioSourceGain[AudioSourceUID];

		if (bProceed) 
		{
			Gain = Gain < 0.001f ? 0.001f : Gain;
			AudioSourceGain[AudioSourceUID] = Gain;
			
			OALSourcef(AudioSources[AudioSourceUID], AL_GAIN, (Gain)*VolumeMultiplier);
		}
	}
}

float OpenALSoft::GetAudioSourceGain(const uint32 AudioSourceUID) 
{
	ALfloat TemporaryGain = 0.0f;
	
	if (AudioSources.Contains(AudioSourceUID)) { OALGetSourcef(AudioSources[AudioSourceUID], AL_GAIN, &TemporaryGain); }		
	
	return TemporaryGain;
}

bool OpenALSoft::RemoveAudioSource(const uint32 AudioSourceUID) 
{
	if (AudioSources.Contains(AudioSourceUID)) 
	{
		if (OALIsSource(AudioSources[AudioSourceUID])) 
		{
			ALint PlayingState;
			OALGetSourcei(AudioSources[AudioSourceUID], AL_SOURCE_STATE, &PlayingState);

			if (PlayingState == AL_PLAYING) { OALSourceStop(AudioSources[AudioSourceUID]); }				
			
			OALSourcei(AudioSources[AudioSourceUID], AL_BUFFER, 0);
		}

		AudioSourcesPool.Add(AudioSources[AudioSourceUID]);
		AudioSources.Remove(AudioSourceUID);
		AudioSources.Compact();

		AudioSourceGain.Remove(AudioSourceUID);
		AudioSourceGain.Compact();

		if (LowpassFilters.Contains(AudioSourceUID)) 
		{
			LowpassPool.Add(LowpassFilters[AudioSourceUID]);
			LowpassFilters.Remove(AudioSourceUID);
			LowpassFilters.Compact();
		}

		if (AudioSourceEAX.Contains(AudioSourceUID)) 
		{
			AudioSourceEAX.Remove(AudioSourceUID);
			AudioSourceEAX.Compact();
		}
	}
	else 
	{
		return false;
	}

	if (bHasReachPlayLimit && AudioSourcesPool.Num() > 0) { bHasReachPlayLimit = false; }			
	
	return true;
}

bool OpenALSoft::IsAnAudioSource(const uint32 AudioSourceUID) 
{
	if (AudioSources.Num() > 0) { return AudioSources.Contains(AudioSourceUID); }		
	
	return false;
}

bool OpenALSoft::IsAudioSourcePlaying(const uint32 AudioSourceUID) 
{
	if (AudioSources.Contains(AudioSourceUID)) 
	{
		ALint PlayingState;
		OALGetSourcei(AudioSources[AudioSourceUID], AL_SOURCE_STATE, &PlayingState);

		if (PlayingState == AL_PLAYING) { return true; }					
	}
	return false;
}

EAudioState OpenALSoft::AudioSourceState(const uint32 AudioSourceUID) 
{
	if (AudioSources.Contains(AudioSourceUID)) 
	{
		ALint PlayingState;
		OALGetSourcei(AudioSources[AudioSourceUID], AL_SOURCE_STATE, &PlayingState);

		if (PlayingState == AL_PLAYING) 
		{
			return EAudioState::Playing;
		}
		else if (PlayingState == AL_STOPPED) 
		{
			return EAudioState::Stopped;
		}
		else if (PlayingState == AL_PAUSED) 
		{
			return EAudioState::Paused;
		}
	}
	return EAudioState::None;
}

void OpenALSoft::SetLooping(const uint32 AudioSourceUID, const bool bShouldLoop) 
{
	if (AudioSources.Contains(AudioSourceUID) == false) { return; }
	
	if (bShouldLoop) 
	{
		OALSourcei(AudioSources[AudioSourceUID], AL_LOOPING, AL_TRUE);
	}
	else 
	{
		OALSourcei(AudioSources[AudioSourceUID], AL_LOOPING, AL_FALSE);
	}	
}

void OpenALSoft::ChangeAudioSourcePitch(const uint32 AudioSourceUID, const float Pitch) 
{
	if (AudioSources.Contains(AudioSourceUID)) { OALSourcef(AudioSources[AudioSourceUID], AL_PITCH, Pitch); }		
}

void OpenALSoft::UpdateAudioSourcePosition(const uint32 AudioSourceUID, const FVector AudioSourcePosition)
{
	if (AudioSources.Contains(AudioSourceUID)) 
	{
		OALSource3f(AudioSources[AudioSourceUID], AL_POSITION, AudioSourcePosition.X, AudioSourcePosition.Y, AudioSourcePosition.Z);
		UpdateEAXReverbGain();
	}
}

void OpenALSoft::UpdateAudioSourceVelocity(const uint32 AudioSourceUID, const FVector Velocity) 
{
	if (AudioSources.Contains(AudioSourceUID)) { OALSource3f(AudioSources[AudioSourceUID], AL_VELOCITY, Velocity.X, Velocity.Y, Velocity.Z); }
}

void OpenALSoft::ChangeDopplerFactor(const float InDopplerFactor) 
{
	OALDopplerFactor(InDopplerFactor);
}

float OpenALSoft::CurrentPositionOnAudioTrack(const uint32 AudioSourceUID) 
{
	ALfloat CurrentTime = 0.0f;
	
	if (AudioSources.Contains(AudioSourceUID)) 
	{
		ALint PlayingState;
		OALGetSourcei(AudioSources[AudioSourceUID], AL_SOURCE_STATE, &PlayingState);

		if (PlayingState == AL_PLAYING) { OALGetSourcef(AudioSources[AudioSourceUID], AL_SEC_OFFSET, &CurrentTime); }				
	}
	return CurrentTime;
}

void OpenALSoft::SetUpAudioListener() 
{
	ALfloat TemporaryListenerOrientation[] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };

	OALListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	OALListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	OALListenerfv(AL_ORIENTATION, TemporaryListenerOrientation);
}

void OpenALSoft::UpdateAudioListenerePosition(const FVector ListenerPosition, const FVector ListenerForwardVector, const FVector ListenerUpVector) 
{
	if (bWasDllLoaded) 
	{
		ALfloat ListenerOrientation[] = { ListenerForwardVector.X, ListenerForwardVector.Y, -ListenerForwardVector.Z, ListenerUpVector.X, ListenerUpVector.Y, -ListenerUpVector.Z };

		OALListener3f(AL_POSITION, ListenerPosition.X, ListenerPosition.Y, ListenerPosition.Z);
		OALListenerfv(AL_ORIENTATION, ListenerOrientation);
		UpdateEAXReverbGain();
	}
}

void OpenALSoft::UpdateAudioListenerVelocity(const FVector Velocity) 
{
	if (bWasDllLoaded) { OALListener3f(AL_VELOCITY, Velocity.X, Velocity.Z, Velocity.Y); }		
}

void OpenALSoft::SetAudioSourceData() 
{
	OALSourcef(TemporaryAudioSource, AL_PITCH, 1.0f);
	OALSourcef(TemporaryAudioSource, AL_GAIN, TemporaryAudioGain * VolumeMultiplier);
	OALSource3f(TemporaryAudioSource, AL_POSITION, TemporaryAudioSourcePosition.X, TemporaryAudioSourcePosition.Z, TemporaryAudioSourcePosition.Y);
	OALSource3f(TemporaryAudioSource, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	OALSourcei(TemporaryAudioSource, AL_DISTANCE_MODEL, AL_NONE);
}

void OpenALSoft::CreateAudioSourcePool() 
{
	if (AudioSourcesPool.Num() == 0) 
	{
		AudioSourcesPool.Reserve(MaximumAvailableAudioChannels);
		
		for (uint16 Index = 0; Index < MaximumAvailableAudioChannels; ++Index) 
		{
			OALGenSources((ALuint)1, &TemporaryAudioSource);
			AudioSourcesPool.Add(TemporaryAudioSource);
		}
	}
}

void OpenALSoft::CreateLowpassPool() 
{
	if (LowpassPool.Num() == 0) 
	{
		ALuint	TemporaryAudioFilter;
		LowpassPool.Reserve(MaximumAvailableAudioChannels);
		
		for (uint16 index = 0; index < MaximumAvailableAudioChannels; ++index) 
		{
			OALGenFilters((ALuint)1, &TemporaryAudioFilter);
			LowpassPool.Add(TemporaryAudioFilter);
		}
	}
}

void OpenALSoft::CreateBandpassPool() 
{
	if (BandpassPool.Num() == 0) 
	{
		ALuint	TemporaryAudioFilter;
		BandpassPool.Reserve(MaximumAvailableAudioChannels);
		
		for (uint16 index = 0; index < MaximumAvailableAudioChannels; ++index) 
		{
			OALGenFilters((ALuint)1, &TemporaryAudioFilter);
			BandpassPool.Add(TemporaryAudioFilter);
		}
	}
}

void OpenALSoft::ChangeBandpassFilter(const uint32 AudioSourceUID, const float HFGain, const float LFGain) 
{
	if (bWasDllLoaded && AudioSources.Contains(AudioSourceUID)) 
	{
		if (!BandpassFilters.Contains(AudioSourceUID)) { SetBandpassFilterForAudioSource(AudioSourceUID); }			

		OALFilterf(BandpassFilters[AudioSourceUID], AL_BANDPASS_GAINHF, HFGain);
		OALFilterf(BandpassFilters[AudioSourceUID], AL_BANDPASS_GAINLF, LFGain);
		OALSourcei(AudioSources[AudioSourceUID], AL_DIRECT_FILTER, BandpassFilters[AudioSourceUID]);
	}
}

void OpenALSoft::SetBandpassFilterForAudioSource(const uint32 AudioSourceUID)
{
	if (BandpassPool.Num() == 0) { return; }		

	ALuint TemporaryBandpassFilter;
	
	TemporaryBandpassFilter = BandpassPool[0];
	BandpassPool.RemoveAt(0);
	
	OALFilteri(TemporaryBandpassFilter, AL_FILTER_TYPE, AL_FILTER_BANDPASS);
	BandpassFilters.Add(AudioSourceUID, TemporaryBandpassFilter);
}

void OpenALSoft::RemoveBandpassFilterForAudioSource(const uint32 AudioSourceUID) 
{
	if (BandpassFilters.Contains(AudioSourceUID)) 
	{
		OALDeleteFilters((ALuint)1, &BandpassFilters[AudioSourceUID]);
		BandpassPool.Add(BandpassFilters[AudioSourceUID]);
		BandpassFilters.Remove(AudioSourceUID);
	}
}

void OpenALSoft::ChangeLowpassFilter(const uint32 AudioSourceUID, const float Gain, const float HFGain) 
{
	if (bWasDllLoaded && AudioSources.Contains(AudioSourceUID)) 
	{
		if (!LowpassFilters.Contains(AudioSourceUID)) { SetLowpassFilterForAudioSource(AudioSourceUID); }			
		
		OALFilterf(LowpassFilters[AudioSourceUID], AL_LOWPASS_GAIN, Gain);
		OALFilterf(LowpassFilters[AudioSourceUID], AL_LOWPASS_GAINHF, HFGain);
		OALSourcei(AudioSources[AudioSourceUID], AL_DIRECT_FILTER, LowpassFilters[AudioSourceUID]);
	}
}

void OpenALSoft::SetLowpassFilterForAudioSource(const uint32 AudioSourceUID) 
{
	if (LowpassPool.Num() == 0) { return; }		

	ALuint TemporaryLowpassFilter;
	
	TemporaryLowpassFilter = LowpassPool[0];
	LowpassPool.RemoveAt(0);
	
	OALFilteri(TemporaryLowpassFilter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
	LowpassFilters.Add(AudioSourceUID, TemporaryLowpassFilter);
}

void OpenALSoft::RemoveLowpassFilterForAudioSource(const uint32 AudioSourceUID) 
{
	if (LowpassFilters.Contains(AudioSourceUID)) 
	{
		OALDeleteFilters((ALuint)1, &LowpassFilters[AudioSourceUID]);
		LowpassPool.Add(LowpassFilters[AudioSourceUID]);
		LowpassFilters.Remove(AudioSourceUID);
	}
}

void OpenALSoft::SetEAXOnAudioSource(const EEAX EAX, const uint32 AudioSourceUID, const float EAXGain /* = 0.32f */)
{
	if (EAX == EEAX::None) 
	{
		OALSource3i(AudioSources[AudioSourceUID], AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, 0, AL_FILTER_NULL);
		return;
	}

	if (EAXEffectSlots.Num() == EAXSlotsCapacity)
	{
		TArray< EEAX > TemporarySlotKeys;
		EAXEffectSlots.GenerateKeyArray(TemporarySlotKeys);

		OALAuxiliaryEffectSloti(EAXEffectSlots[TemporarySlotKeys[0]], AL_EFFECTSLOT_EFFECT, AL_EFFECT_NULL);
		OALDeleteEffects((ALuint)1, &EAXEffects[TemporarySlotKeys[0]]);
		OALDeleteAuxiliaryEffectSlot((ALuint)1, &EAXEffectSlots[TemporarySlotKeys[0]]);

		EAXEffectSlots.Remove(TemporarySlotKeys[0]);
		EAXEffectSlots.Compact();

		EAXEffects.Remove(TemporarySlotKeys[0]);
		EAXEffects.Compact();

		EAXFilters.Remove(TemporarySlotKeys[0]);
		EAXFilters.Compact();
	}

	if (!EAXEffectSlots.Contains(EAX)) 
	{
		ALuint NewEAXSlot = (ALuint)EAXEffectSlots.Num();
		OALGenAuxiliaryEffectSlot((ALuint)1, &NewEAXSlot);
		EAXEffectSlots.Add(EAX, NewEAXSlot);
		CatchError("effect slot");
	}

	float TemporaryReverbGain = 0.0f;
	
	if (AudioSourceGain.Contains(AudioSourceUID)) 
		TemporaryReverbGain = AudioSourceGain[AudioSourceUID];
	
	if (AudioSourceEAX.Contains(AudioSourceUID)) 
	{
		AudioSourceEAX[AudioSourceUID] = EAX;
	}
	else 
	{
		AudioSourceEAX.Add(AudioSourceUID, EAX);
	}

	if (AudioSourceEAXCustomGain.Contains(AudioSourceUID))
	{
		AudioSourceEAXCustomGain[AudioSourceUID] = EAXGain;
	}
	else 
	{
		AudioSourceEAXCustomGain.Add(AudioSourceUID, EAXGain);
	}

	if (!EAXEffects.Contains(EAX)) 
	{
		ALuint NewEAXEffect = (ALuint)EAXEffects.Num();
		OALGenEffects((ALuint)1, &NewEAXEffect);
		EAXEffects.Add(EAX, NewEAXEffect);

		if (OALIsEffect(EAXEffects[EAX])) 
		{
			OALEffecti(EAXEffects[EAX], AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);
			SetEFXEAXReverb(&RookUtils::Instance().EAXReverb[EAX], EAXEffects[EAX], AudioSourceEAXCustomGain[AudioSourceUID], TemporaryReverbGain);
		}

		OALAuxiliaryEffectSloti(EAXEffectSlots[EAX], AL_EFFECTSLOT_EFFECT, EAXEffects[EAX]);
		CatchError("setting effect");
	}

	if (!EAXFilters.Contains(EAX)) 
	{
		ALuint NewEAXFilter = (ALuint)EAXFilters.Num();
		OALGenFilters((ALuint)1, &NewEAXFilter);

		OALFilteri(NewEAXFilter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);

		if (OALGetError() == AL_NO_ERROR) 
		{
			OALFilterf(NewEAXFilter, AL_LOWPASS_GAIN, 0.5f);
			OALFilterf(NewEAXFilter, AL_LOWPASS_GAINHF, 0.5f);
		}

		EAXFilters.Add(EAX, NewEAXFilter);
		CatchError("setting filter");
	}

	OALSource3i(AudioSources[AudioSourceUID], AL_AUXILIARY_SEND_FILTER, EAXEffectSlots[EAX], 0, EAXFilters[EAX]);
	CatchError("setting effect on source");
}

void OpenALSoft::SetMainReverb(const EEAX EAX, const float EAXGain /* = 0.32f */) 
{
	if (EAX != EEAX::None) 
	{
		MainReverb = &RookUtils::Instance().EAXReverb[EAX];
	}
	else 
	{
		MainReverb = nullptr;
	}
	UpdateEAXReverbGain();
}

void OpenALSoft::SetEFXEAXReverb(EFXEAXREVERBPROPERTIES* EAXReverb, ALuint UIDEffect, const float CustomGain, const float ReverbGain) 
{
	if (EAXReverb) 
	{
		OALGetError();

		float RevDensity = 0.0f;
		float RevDiffusion = 0.0f;
		float RevGainHF = 0.0f;
		float RevGainLF = 0.0f;
		float RevDecayTime = 0.0f;
		float RevDecayHFRatio = 0.0f;
		float RevDecayLFRatio = 0.0f;
		float RevReflectionsGain = 0.0f;
		float RevReflectionsDelay = 0.0f;
		float RevLateReverbGain = 0.0f;
		float RevLateReverbDelay = 0.0f;
		float EchoTime = 0.0f;
		float EchoDepth = 0.0f;
		float ModulationTime = 0.0f;
		float ModulationDepth = 0.0f;
		float AirAbsorptionGainHF = 0.0f;
		float HFReference = 0.0f;
		float LFReference = 0.0f;
		float RoomRolloffFactor = 0.0f;
		int DecayHFLimit = 0;

		if (MainReverb == nullptr || MainReverb == EAXReverb) 
		{
			RevDensity = EAXReverb->flDensity;
			RevDiffusion = EAXReverb->flDiffusion;
			RevGainHF = EAXReverb->flGainHF;
			RevGainLF = EAXReverb->flGainLF;
			RevDecayTime = EAXReverb->flDecayTime;
			RevDecayHFRatio = EAXReverb->flDecayHFRatio;
			RevDecayLFRatio = EAXReverb->flDecayLFRatio;
			RevReflectionsGain = EAXReverb->flReflectionsGain;
			RevReflectionsDelay = EAXReverb->flReflectionsDelay;
			RevLateReverbGain = EAXReverb->flLateReverbGain;
			RevLateReverbDelay = EAXReverb->flLateReverbDelay;
			EchoTime = EAXReverb->flEchoTime;
			EchoDepth = EAXReverb->flEchoDepth;
			ModulationTime = EAXReverb->flModulationTime;
			ModulationDepth = EAXReverb->flModulationDepth;
			AirAbsorptionGainHF = EAXReverb->flAirAbsorptionGainHF;
			HFReference = EAXReverb->flHFReference;
			LFReference = EAXReverb->flLFReference;
			RoomRolloffFactor = EAXReverb->flRoomRolloffFactor;
			DecayHFLimit = EAXReverb->iDecayHFLimit;
		}
		else
		{
			float TempRevGain = ReverbGain * 0.01f;
			float MainReverbGain = 1.0f - TempRevGain;

			RevDensity = EAXReverb->flDensity*TempRevGain + MainReverb->flDensity*MainReverbGain;
			RevDiffusion = EAXReverb->flDiffusion*TempRevGain + MainReverb->flDiffusion*MainReverbGain;
			RevGainHF = EAXReverb->flGainHF*TempRevGain + MainReverb->flGainHF*MainReverbGain;
			RevGainLF = EAXReverb->flGainLF*TempRevGain + MainReverb->flGainLF*MainReverbGain;
			RevDecayTime = EAXReverb->flDecayTime*TempRevGain + MainReverb->flDecayTime*MainReverbGain;
			RevDecayHFRatio = EAXReverb->flDecayHFRatio*TempRevGain + MainReverb->flDecayHFRatio*MainReverbGain;
			RevDecayLFRatio = EAXReverb->flDecayLFRatio*TempRevGain + MainReverb->flDecayLFRatio*MainReverbGain;
			RevReflectionsGain = EAXReverb->flReflectionsGain*TempRevGain + MainReverb->flReflectionsGain*MainReverbGain;
			RevReflectionsDelay = EAXReverb->flReflectionsDelay*TempRevGain + MainReverb->flReflectionsDelay*MainReverbGain;
			RevLateReverbGain = EAXReverb->flLateReverbGain*TempRevGain + MainReverb->flLateReverbGain*MainReverbGain;
			RevLateReverbDelay = EAXReverb->flLateReverbDelay*TempRevGain + MainReverb->flLateReverbDelay*MainReverbGain;
			EchoTime = EAXReverb->flEchoTime*TempRevGain + MainReverb->flEchoTime*MainReverbGain;
			EchoDepth = EAXReverb->flEchoDepth*TempRevGain + MainReverb->flEchoDepth*MainReverbGain;
			ModulationTime = EAXReverb->flModulationTime*TempRevGain + MainReverb->flModulationTime*MainReverbGain;
			ModulationDepth = EAXReverb->flModulationDepth*TempRevGain + MainReverb->flModulationDepth*MainReverbGain;
			AirAbsorptionGainHF = EAXReverb->flAirAbsorptionGainHF*TempRevGain + MainReverb->flAirAbsorptionGainHF*MainReverbGain;
			HFReference = EAXReverb->flHFReference*TempRevGain + MainReverb->flHFReference*MainReverbGain;
			LFReference = EAXReverb->flLFReference*TempRevGain + MainReverb->flLFReference*MainReverbGain;
			RoomRolloffFactor = EAXReverb->flRoomRolloffFactor*TempRevGain + MainReverb->flRoomRolloffFactor*MainReverbGain;
			DecayHFLimit = EAXReverb->iDecayHFLimit;
		}

		OALEffectf(UIDEffect, AL_EAXREVERB_DENSITY, RevDensity);
		OALEffectf(UIDEffect, AL_EAXREVERB_DIFFUSION, RevDiffusion);
		OALEffectf(UIDEffect, AL_EAXREVERB_GAIN, CustomGain);
		OALEffectf(UIDEffect, AL_EAXREVERB_GAINHF, RevGainHF);
		OALEffectf(UIDEffect, AL_EAXREVERB_GAINLF, RevGainLF);
		OALEffectf(UIDEffect, AL_EAXREVERB_DECAY_TIME, RevDecayTime);
		OALEffectf(UIDEffect, AL_EAXREVERB_DECAY_HFRATIO, RevDecayHFRatio);
		OALEffectf(UIDEffect, AL_EAXREVERB_DECAY_LFRATIO, RevDecayLFRatio);
		OALEffectf(UIDEffect, AL_EAXREVERB_REFLECTIONS_GAIN, RevReflectionsGain);
		OALEffectf(UIDEffect, AL_EAXREVERB_REFLECTIONS_DELAY, RevReflectionsDelay);
		OALEffectfv(UIDEffect, AL_EAXREVERB_REFLECTIONS_PAN, EAXReverb->flReflectionsPan);
		OALEffectf(UIDEffect, AL_EAXREVERB_LATE_REVERB_GAIN, RevLateReverbGain);
		OALEffectf(UIDEffect, AL_EAXREVERB_LATE_REVERB_DELAY, RevLateReverbDelay);
		OALEffectfv(UIDEffect, AL_EAXREVERB_LATE_REVERB_PAN, EAXReverb->flLateReverbPan);
		OALEffectf(UIDEffect, AL_EAXREVERB_ECHO_TIME, EchoTime);
		OALEffectf(UIDEffect, AL_EAXREVERB_ECHO_DEPTH, EchoDepth);
		OALEffectf(UIDEffect, AL_EAXREVERB_MODULATION_TIME, ModulationTime);
		OALEffectf(UIDEffect, AL_EAXREVERB_MODULATION_DEPTH, ModulationDepth);
		OALEffectf(UIDEffect, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, AirAbsorptionGainHF);
		OALEffectf(UIDEffect, AL_EAXREVERB_HFREFERENCE, HFReference);
		OALEffectf(UIDEffect, AL_EAXREVERB_LFREFERENCE, LFReference);
		OALEffectf(UIDEffect, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, RoomRolloffFactor);
		OALEffecti(UIDEffect, AL_EAXREVERB_DECAY_HFLIMIT, DecayHFLimit);
	}
}

void OpenALSoft::UpdateEAXReverbGain() 
{
	for (TPair< uint32, EEAX >& EAX : AudioSourceEAX) 
	{
		float ReverbGain = 0.0f;
		
		if (AudioSources.Contains(EAX.Key)) { ReverbGain = AudioSources[EAX.Key]; }			
		
		SetEFXEAXReverb(&RookUtils::Instance().EAXReverb[EAX.Value], EAXEffects[EAX.Value], AudioSourceEAXCustomGain[EAX.Key], ReverbGain);
		OALAuxiliaryEffectSloti(EAXEffectSlots[EAX.Value], AL_EFFECTSLOT_EFFECT, EAXEffects[EAX.Value]);
		OALSource3i(AudioSources[EAX.Key], AL_AUXILIARY_SEND_FILTER, EAXEffectSlots[EAX.Value], 0, EAXFilters[EAX.Value]);
	}
}

void OpenALSoft::CleanAudioDataInBuffer(const uint32 AudioSourceUID) 
{
	if (Buffers.Contains(AudioSourceUID)) 
	{
		OALDeleteBuffers((ALuint)1, &Buffers[AudioSourceUID]);
		Buffers[AudioSourceUID] = 0;
	}
}

void OpenALSoft::SetVolumeMulitiplier(const float NewVolMultiplier)
{
	if (VolumeMultiplier != NewVolMultiplier) 
	{
		VolumeMultiplier = NewVolMultiplier;
		
		for (auto GainIterator = AudioSourceGain.CreateIterator(); GainIterator; ++GainIterator)
		{
			ChangeAudioSourceGain(GainIterator.Key(), GainIterator.Value());
		}				
	}
}

bool OpenALSoft::HadError(const uint32 AudioSourceUID) const 
{
	if (SourceError.Contains(AudioSourceUID)) { return SourceError[AudioSourceUID]; }		
	
	return false;
}

void OpenALSoft::CleanErrorEntry(const uint32 AudioSourceUID) 
{
	if (SourceError.Contains(AudioSourceUID)) { SourceError.Remove(AudioSourceUID); }		
}

uint16 OpenALSoft::GetNumberOfAvailableAudioSourcesInPool() const
{
	return MaximumAvailableAudioChannels - AudioSourcesPool.Num();
}

void OpenALSoft::LoadAudioAsset(const TWeakObjectPtr<class USoundWave> AudioAsset, const bool bAddToMap) 
{
	if (!AudioAsset.IsValid()) { return; }		
	
	if (DataLoader == nullptr)
	{
		URookAudioDataLoader* TemporaryDataLoader(nullptr);
		
		for (TObjectIterator<URookAudioDataLoader> Itr; Itr; ++Itr)
		{
			TemporaryDataLoader = Cast<URookAudioDataLoader>(*Itr);
		}			
		
		if (TemporaryDataLoader == nullptr) 
		{
			DataLoader = NewObject<URookAudioDataLoader>();
		}
		else 
		{
			DataLoader = TemporaryDataLoader;
		}
	}

	if (bAddToMap) { Buffers.Add(AudioAsset->GetUniqueID(), 0); }		
	
	if (DataLoader.IsValid())
	{
		TWeakObjectPtr<USoundWave> AssetToLoad(AudioAsset);
		TArray<TWeakObjectPtr<USoundWave>> TempDataToLoad;
		TempDataToLoad.Add(AssetToLoad);
		
		DataLoader->LoadingGraph.Add(TGraphTask<RookAudioDataLoadingTask>::CreateTask(nullptr, ENamedThreads::AnyThread).ConstructAndDispatchWhenReady(TempDataToLoad));
		TempDataToLoad.Empty();
	}
	else 
	{
		DataLoader = nullptr;
	}
}

bool OpenALSoft::CatchError(const FString ActionName)
{
	ALenum ErrorType = OALGetError();
	bool bHasError = false;
	
	if (ErrorType != AL_NO_ERROR) 
	{
		bHasError = true;
		
		switch (ErrorType)
		{
			case AL_INVALID_NAME:
				UE_LOG(RookLog, Warning, TEXT("OpenAL Error: OAL invalid name while %s"), *ActionName);
				break;

			case AL_INVALID_ENUM:
				UE_LOG(RookLog, Warning, TEXT("OpenAL Error: OAL invalid enum while %s"), *ActionName);
				break;

			case AL_INVALID_VALUE:
				UE_LOG(RookLog, Warning, TEXT("OpenAL Error: OAL invalid value while %s"), *ActionName);
				break;

			case AL_INVALID_OPERATION:
				UE_LOG(RookLog, Warning, TEXT("OpenAL Error: OAL invalid operator while %s"), *ActionName);
				break;

			case AL_OUT_OF_MEMORY:
				UE_LOG(RookLog, Warning, TEXT("OpenAL Error: OAL out of memory while %s"), *ActionName);
				break;
		}
	}
	OALGetError();
	return bHasError;
}

void OpenALSoft::SetAudioDeviceAndCurrentContext() 
{
	AudioDevice = GetBestAudioDevice();
	
	if (AudioDevice) 
	{
		ALint AudioContextAttributes[2] = { ALC_MAX_AUXILIARY_SENDS , 1 };

		AudioContext = OALCreateContext(AudioDevice, AudioContextAttributes);

		if (OALMakeContextCurrent(AudioContext)) 
		{
			OALDopplerFactor(DopplerFactor);
			OALEnable(AL_SOURCE_DISTANCE_MODEL);
			bCanPlayAudio = true;
		}
		else 
		{
			UE_LOG(RookLog, Warning, TEXT("No Current Audio Context!"));
		}
	}
	else
	{
		UE_LOG(RookLog, Warning, TEXT("There is no Audio Device"));
	}
}

ALCdevice* OpenALSoft::GetBestAudioDevice()
{
	const ALCchar* TemporaryDeviceName(OALGetString(NULL, ALC_DEVICE_SPECIFIER));
	const ALCchar* TemporaryNextDeviceName(TemporaryDeviceName + 1);
	uint16 TemporaryNameSize = 0;
	TMap< uint16, ALCdevice* > TemporaryDeviceMap;
	ALCdevice* TempoaryAudioDevice;
	TArray<ALCint> ALCAttributes;
	ALCint AttributesSize;

	while (TemporaryDeviceName && *TemporaryDeviceName != '\0' && TemporaryNextDeviceName && *TemporaryNextDeviceName != '\0')
	{
		//UE_LOG( RookLog, Log, TEXT("Device : %s"), TemporaryDeviceName );
		TempoaryAudioDevice = OALOpenDevice(TemporaryDeviceName);
		ALCAttributes.Empty();
		AttributesSize = 0;
		OALGetIntegerv(TempoaryAudioDevice, ALC_ATTRIBUTES_SIZE, 1, &AttributesSize);

		ALCAttributes.SetNumZeroed(AttributesSize);
		OALGetIntegerv(TempoaryAudioDevice, ALC_ALL_ATTRIBUTES, AttributesSize, &ALCAttributes[0]);

		bool bNextAttribute = false;

		for (ALCint Attribute : ALCAttributes)
		{
			if (!bNextAttribute) 
			{
				if (Attribute == ALC_MONO_SOURCES) { bNextAttribute = true; }					
			}
			else
			{
				TemporaryDeviceMap.Add(Attribute, TempoaryAudioDevice);
				break;
			}
		}

		TemporaryNameSize = strlen(TemporaryDeviceName);
		TemporaryDeviceName += TemporaryNameSize + 1;
		TemporaryNextDeviceName += TemporaryNameSize + 2;
	}
	ALCAttributes.Empty();

	TempoaryAudioDevice = nullptr;

	TemporaryDeviceMap.KeySort([](uint16 KeyA, uint16 KeyB) { return KeyA < KeyB; });
	TArray<uint16> MaximumChannelArray;
	TArray<ALCdevice*> AudioDeviceArray;
	TemporaryDeviceMap.GenerateKeyArray(MaximumChannelArray);
	TemporaryDeviceMap.GenerateValueArray(AudioDeviceArray);
	TemporaryDeviceMap.Empty();

	if (AudioDeviceArray.Num() != 0) 
	{
		if (AudioDeviceArray.Num() > 1) 
		{
			for (int32 Index = 1; Index < AudioDeviceArray.Num(); ++Index) 
			{
				OALCloseDevice(AudioDeviceArray[Index]);
			}						
		}
	}
	else 
	{
		return nullptr;
	}

	MaximumAvailableAudioChannels = MaximumChannelArray[0];
	MaximumChannelArray.Empty();
	return AudioDeviceArray[0];
}

void OpenALSoft::CloseDeviceAndDestroyCurrentContext() 
{
	if (Buffers.Num() > 0) 
	{
		for (TPair<uint32, ALuint>& Buffer : Buffers)
		{
			OALDeleteBuffers((ALuint)1, &Buffer.Value);
		}				
	}

	if (AudioSourcesPool.Num() > 0) 
	{
		for (int32 Index = 0; Index < AudioSourcesPool.Num(); ++Index) 
		{
			if (OALIsSource(AudioSourcesPool[Index])) { OALDeleteSoruces((ALuint)1, &AudioSourcesPool[Index]); }					
		}
	}

	if (LowpassPool.Num() > 0) 
	{
		for (int32 Index = 0; Index < LowpassPool.Num(); ++Index)
		{
			OALDeleteFilters((ALuint)1, &LowpassPool[Index]);
		}				
	}

	if (BandpassPool.Num() > 0) 
	{
		for (int32 Index = 0; Index < BandpassPool.Num(); ++Index)
		{
			OALDeleteFilters((ALuint)1, &BandpassPool[Index]);
		}					
	}

	if (EAXEffectSlots.Num() > 0) 
	{
		for (TPair< EEAX, ALuint >& EAXEffectSlot : EAXEffectSlots)
		{
			if (OALIsAuxiliaryEffectSlot(EAXEffectSlot.Value)) { OALDeleteAuxiliaryEffectSlot((ALuint)1, &EAXEffectSlot.Value); }				
		}
	}

	if (EAXEffects.Num() > 0) 
	{
		for (TPair< EEAX, ALuint >& EAXEffect : EAXEffects)
		{
			if (OALIsEffect(EAXEffect.Value)) { OALDeleteEffects((ALuint)1, &EAXEffect.Value); }				
		}
	}

	if (EAXFilters.Num() > 0) 
	{
		for (TPair< EEAX, ALuint >& EAXFilter : EAXFilters) { OALDeleteFilters((ALuint)1, &EAXFilter.Value); }			
	}

	MainReverb = nullptr;
	Buffers.Empty();
	AudioSources.Empty();
	AudioSourceGain.Empty();
	SourceError.Empty();
	AudioSourcesPool.Empty();
	LowpassPool.Empty();
	LowpassFilters.Empty();
	BandpassPool.Empty();
	BandpassFilters.Empty();
	EAXEffectSlots.Empty();
	EAXEffects.Empty();
	EAXFilters.Empty();
	AudioSourceEAX.Empty();
	AudioSourceEAXCustomGain.Empty();

	if (bWasDllLoaded) 
	{
		OALMakeContextCurrent(nullptr);
		OALDestroyContext(AudioContext);
		OALCloseDevice(AudioDevice);
		AudioDevice = nullptr;
	}
	UE_LOG(RookLog, Log, TEXT("OpenAL Soft: Open AL data cleaned."));
}