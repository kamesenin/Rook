#pragma once
#include <iostream>
#include <string>
#include "RookEnums.h"
#include "RookStructs.h"
#include "../OpenALSoft/include/AL/al.h"
#include "../OpenALSoft/include/AL/alc.h"
#include "../OpenALSoft/include/AL/efx-creative.h"
#include "../OpenALSoft/include/AL/efx.h"
#include "../OpenALSoft/include/AL/alext.h"
#include "../OpenALSoft/include/AL/efx-presets.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////
//Open AL Soft wrapping
//////////////////////////////////////////////////////////////////////////

typedef			void ( *DefAlBufferData )( ALuint BID, ALenum Format, const ALvoid* Data, ALsizei Size, ALsizei Freq );
typedef			ALCenum ( *DefAlcCloseDevice)( ALCdevice* Device );
typedef			ALCcontext* ( *DefAlcCreateContext )( ALCdevice* Device, const ALCint* Attributelist );
typedef			void ( *DefAlcDestroyContext )( ALCcontext* Context );
typedef			ALCboolean ( *DefAlcMakeContextCurrent )( ALCcontext* Context );
typedef			ALCdevice* ( *DefAlcOpenDevice )( const ALCchar* DeviceName );
typedef			void ( *DefAlDeleteBuffers )( ALsizei N, const ALuint* Buffers );
typedef			void ( *DefAlDeleteSources )( ALsizei N, const ALuint* Sources );
typedef			void ( *DefAlDistanceModel )( ALenum DistanceModel );
typedef			void ( *DefAlGenBuffers )( ALsizei N, ALuint* Buffers );
typedef			void ( *DefAlGenSources )( ALsizei N, ALuint* Sources );
typedef			void ( *DefAlListener3f )( ALenum Param, ALfloat Value1, ALfloat Value2, ALfloat Value3 );
typedef			void ( *DefAlListenerfv )( ALenum Param, const ALfloat* Values );
typedef			void ( *DefAlSource3f )( ALuint SID, ALenum Param, ALfloat Value1, ALfloat Value2, ALfloat Value3 );
typedef			void ( *DefAlSourcef )( ALuint SID, ALenum Param, ALfloat Value );
typedef			void ( *DefAlSourcei )( ALuint SID, ALenum Param, ALint Value );
typedef			void ( *DefAlSourceiv )( ALuint Source, ALenum Param, const ALint* Values );
typedef			void ( *DefAlSource3i )( ALuint Source, ALenum Param, ALint Value1, ALint Value2, ALint Value3 );
typedef			void ( *DefAlSourcePlay )( ALuint SID );
typedef			void ( *DefAlSourceStop )( ALuint SID );
typedef			void ( *DefAlSourcePause )( ALuint SID );
typedef			void ( *DefAlGetSourcei )( ALuint SID, ALenum Param, ALint* Value );
typedef			void ( *DefAlGetSourcei )( ALuint SID, ALenum Param, ALint* Value );
typedef			void ( *DefAlGetSourcef )( ALuint SID, ALenum Param, ALfloat* Value );
typedef			ALboolean ( *DefAlIsSource )( ALuint Source );
typedef			ALenum ( *DefAlGetEnumValue )( const ALchar* Ename );
typedef			ALenum ( *DefAlEnable )( ALenum Capability );

typedef			ALenum ( *DefAlGetError )( void );

typedef			ALboolean ( *DefAlIsExtensionPresent )( const ALCchar* Extname );
typedef			ALCboolean ( *DefAlcIsExtensionPresent )( ALCdevice* Device, const ALCchar* Extname );
typedef			const ALCchar* ( *DefAlcGetString )( ALCdevice* Device, ALCenum Param );
typedef			void ( *DefAlcGetIntegerv )( ALCdevice* Device, ALCenum Param, ALCsizei Size, ALCint* Data );

typedef			void ( *DefAlGenEffects )( ALsizei N, ALuint* Effects );
typedef			void ( *DefAlDeleteEffects )( ALsizei N, const ALuint* Effects );
typedef			ALCboolean ( *DefAlIsEffect )( ALuint Effect );
typedef			void ( *DefAlGenAuxiliaryEffectSlots )( ALsizei N, ALuint* EffectSlots );
typedef			void ( *DefAlDeleteAuxiliaryEffectSlots )( ALsizei N, const ALuint* EffectSlots );
typedef			ALCboolean ( *DefAlIsAuxiliaryEffectSlot )( ALuint EffectSlot );

typedef			void ( *DefAlEffecti )( ALuint Effect, ALenum Param, ALint iValue );
typedef			void ( *DefAlEffectiv )( ALuint Effect, ALenum Param, const ALint* piValues );
typedef			void ( *DefAlEffectf )( ALuint Effect, ALenum Param, ALfloat flValue );
typedef			void ( *DefAlEffectfv )( ALuint Effect, ALenum Param, const ALfloat* pflValues );
typedef			void ( *DefAlAuxiliaryEffectSloti )( ALuint EffectSlot, ALenum Param, ALint iValue );

typedef			void ( *DefAlGenFilters ) ( ALsizei N, ALuint* Filters );
typedef			void ( *DefAlDeleteFilters )( ALsizei N, const ALuint* Filters );
typedef			void ( *DefAlFilteri )( ALuint Filter, ALenum Param, ALint iValue );
typedef			void ( *DefAlFilteriv )( ALuint Filter, ALenum Param, const ALint* piValues );
typedef			void ( *DefAlFilterf )( ALuint Filter, ALenum Param, ALfloat flValue );
typedef			void ( *DefAlFilterfv )( ALuint Filter, ALenum Param, const ALfloat* pflValues );

typedef			void ( *DefAlDopplerFactor )( ALfloat Value );
typedef			void ( *DefAlDopplerVelocity )( ALfloat Value );

typedef			ALCboolean ( *DefAlcDeviceEnableHrtf )( ALCdevice* Device, ALboolean Enable );

class OpenALSoft {
public:
	static OpenALSoft&			Instance();
	~OpenALSoft(){};

	bool						Play( FAudioSourceData SourceData );
	void						PlayAt(uint32 AudioSourceUID, float Seconds);
	void						Stop( uint32 AudioSourceUID );
	void						Pause( uint32 AudioSourceUID );
	void						ChangeAudioSourceGain( uint32 AudioSourceUID, float Gain, bool bForceGain = false );
	float						GetAudioSourceGain( uint32 AudioSourceUID );
	bool						RemoveAudioSource( uint32 AudioSourceUID );
	bool						IsAnAudioSource( uint32 AudioSourceUID );
	bool						IsAudioSourcePlaying( uint32 AudioSourceUID );
	void						SetLooping ( uint32 AudioSourceUID, bool bShouldLoop );
	void						ChangeAudioSourcePitch( uint32 AudioSourceUID, float Pitch );
	void						UpdateAudioSourcePosition( uint32 AudioSourceUID, FVector AudioSourcePosition );
	void						UpdateAudioSourceVelocity( uint32 AudioSourceUID, FVector Velocity );
	void						ChangeAudioSourceDopplerFactor( float DopplerFactor );
	float						CurrentPositionOnAudioTrack( uint32 AudioSourceUID );

	void						SetUpAudioListener();
	void						UpdateAudioListenerePosition( FVector ListenerPosition, FVector ListenerForwardVector, FVector ListenerUpVector );
	void						UpdateAudioListenerVelocity( FVector Velocity );
	
	void						ChangeBandpassFilter( uint32 AudioSourceUID, float HFGain, float LFGain );
	void						ChangeLowpassFilter( uint32 AudioSourceUID, float Gain, float HFGain );
	
	void						SetEAXOnAudioSource( EEAX EAX, uint32 AudioSourceUID, float EAXGain = 0.32f );
	void						SetMainReverb( EEAX EAX, float EAXGain = 0.32f );

	void						CleanAudioDataInBuffer( uint32 AudioSourceUID );
	
	void						SetVolumeMulitiplier( float NewVolMultiplier );
	bool						HadError( uint32 AudioSourceUID ) const;
	void						CleanErrorEntry( uint32 AudioSourceUID );
	uint16						GetNumberOfAudioSourcesInPool() const;

	void						CloseDeviceAndDestroyCurrentContext();
	bool						IsDLLLoaded() const;
public:
	bool						bHasDeviceBeenSet = false;
	uint16						CurrentLiveSourceCount = 0;
	uint16						MaximumAvailableAudioChannels = 0;
	bool						bHasReachPlayLimit = false;
	TMap< uint32, ALuint >		Buffers;
	DefAlGenBuffers				OALGenBuffers = nullptr;
	DefAlBufferData				OALBufferData = nullptr;
private:
	//////////////////////////////////////////////////////////////////////////
	//DLL
	//////////////////////////////////////////////////////////////////////////
	
	FString							ErrorName = "alGetError";
	FString							EnableName = "alEnable";
	
	FString							OpenDeviceName = "alcOpenDevice";
	FString							CloseDeviceName = "alcCloseDevice";
	FString							CreateContextName = "alcCreateContext";
	FString							DestroyContextName = "alcDestroyContext";
	FString							MakeContextCurrentName = "alcMakeContextCurrent";

	FString							GenBuffersName = "alGenBuffers";
	FString							DeleteBuffersName = "alDeleteBuffers";
	FString							BufferName = "alBufferData";
	
	FString							Listener3fName = "alListener3f";
	FString							ListenerfvName = "alListenerfv";
	
	FString							GenSourcesName = "alGenSources";
	FString							DeleteSourcesName = "alDeleteSources";
	FString							Soruce3fName = "alSource3f";
	FString							SorucefName = "alSourcef";
	FString							SoruceivName = "alSourceiv";
	FString							SoruceiName = "alSourcei";
	FString							Soruce3iName = "alSource3i";
	FString							SorucePlayName = "alSourcePlay";
	FString							SoruceStopName = "alSourceStop";
	FString							SorucePauseName = "alSourcePause";
	FString							IsSourceName = "alIsSource";

	FString							IsExtensionPresentName = "alIsExtensionPresent";
	FString							IsAlcExtensionPresentName = "alcIsExtensionPresent";

	FString							GetStringName = "alcGetString";
	FString							GetIntegervName = "alcGetIntegerv";
	FString							GetSourceiName = "alGetSourcei";
	FString							GetSourcefName = "alGetSourcef";
	FString							GetEnumValueName = "alGetEnumValue";
	
	FString							HrtfName = "alcDeviceEnableHrtf";
	
	FString							GenEffectName = "alGenEffects";
	FString							DeleteEffectName = "alDeleteEffects";
	FString							IsEffectName = "alIsEffect";
	FString							AlGenAuxiliaryEffectSlotsName = "alGenAuxiliaryEffectSlots";
	FString							DeleteAuxiliaryEffectSlotsName = "alDeleteAuxiliaryEffectSlots";
	FString							IsAuxiliaryEffectSlotName = "alIsAuxiliaryEffectSlot";
	FString							AuxiliaryEffectSlotiName = "alAuxiliaryEffectSloti";

	FString							EffectiName = "alEffecti";
	FString							EffectivName = "alEffectiv";
	FString							EffectfName = "alEffectf";
	FString							EffectfvName = "alEffectfv";

	FString							GenFiltersName = "alGenFilters";
	FString							DeleteFiltersName = "alDeleteFilters";
	FString							FilteriName = "alFilteri";
	FString							FilterivName = "alFilteriv";
	FString							FilterfName = "alFilterf";
	FString							FilterfvName = "alFilterfv";
	
	FString							DopplerFactorName = "alDopplerFactor";
	FString							DopplerVelocityName = "alDopplerVelocity";

	FString							DistanceModelName = "alDistanceModel";
	FString							SoruceDistanceModelName = "alSourceDistanceModel";
	
	void							ProcessDLL();	
	void							FreeOpenALDll();
	FString							PathToLibrary;
	void*							OALDLLHandler;
	bool							bWasDllLoaded = false;
	
	DefAlGetError					OALGetError = nullptr;
	DefAlEnable						OALEnable = nullptr;

	DefAlcOpenDevice				OALOpenDevice = nullptr;
	DefAlcCloseDevice				OALCloseDevice = nullptr;
	DefAlcCreateContext				OALCreateContext = nullptr;
	DefAlcDestroyContext			OALDestroyContext = nullptr;
	DefAlcMakeContextCurrent		OALMakeContextCurrent = nullptr;
		
	DefAlDeleteBuffers				OALDeleteBuffers = nullptr;

	DefAlListener3f					OALListener3f = nullptr;
	DefAlListenerfv					OALListenerfv = nullptr;

	DefAlGenSources					OALGenSources = nullptr;
	DefAlDeleteSources				OALDeleteSoruces = nullptr;
	DefAlSource3f					OALSource3f = nullptr;
	DefAlSourcef					OALSourcef = nullptr;
	DefAlSourcei					OALSourcei = nullptr;
	DefAlSourceiv					OALSourceiv = nullptr;
	DefAlSource3i					OALSource3i = nullptr;
	DefAlSourcePlay					OALSourcePlay = nullptr;
	DefAlSourceStop					OALSourceStop = nullptr;
	DefAlSourcePause				OALSourcePause = nullptr;
	DefAlIsSource					OALIsSource = nullptr;

	DefAlIsExtensionPresent			OALIsExtensionPresent = nullptr;
	DefAlcIsExtensionPresent		OALIsAlcExtensionPresent = nullptr;

	DefAlcGetString					OALGetString = nullptr;
	DefAlcGetIntegerv				OALGetIntegerv = nullptr;
	DefAlGetSourcei					OALGetSourcei = nullptr;
	DefAlGetSourcef					OALGetSourcef = nullptr;
	DefAlGetEnumValue				OALGetEnumValue = nullptr;

	DefAlcDeviceEnableHrtf			OALEnableHrtf = nullptr;

	DefAlGenEffects					OALGenEffects = nullptr;
	DefAlDeleteEffects				OALDeleteEffects = nullptr;
	DefAlIsEffect					OALIsEffect = nullptr;
	DefAlGenAuxiliaryEffectSlots	OALGenAuxiliaryEffectSlot = nullptr;
	DefAlDeleteAuxiliaryEffectSlots	OALDeleteAuxiliaryEffectSlot = nullptr;
	DefAlIsAuxiliaryEffectSlot		OALIsAuxiliaryEffectSlot = nullptr;
	DefAlAuxiliaryEffectSloti		OALAuxiliaryEffectSloti = nullptr;

	DefAlEffecti					OALEffecti = nullptr;
	DefAlEffectiv					OALEffectiv = nullptr;
	DefAlEffectf					OALEffectf = nullptr;
	DefAlEffectfv					OALEffectfv = nullptr;

	DefAlGenFilters					OALGenFilters = nullptr;
	DefAlDeleteFilters				OALDeleteFilters = nullptr;
	DefAlFilteri					OALFilteri = nullptr;
	DefAlFilteriv					OALFilteriv = nullptr;
	DefAlFilterf					OALFilterf = nullptr;
	DefAlFilterfv					OALFilterfv = nullptr;

	DefAlDopplerFactor				OALDopplerFactor = nullptr;
	DefAlDopplerVelocity			OALDopplerVelocity = nullptr;

	DefAlDistanceModel				OALDistanceModel = nullptr;

	//////////////////////////////////////////////////////////////////////////
	static OpenALSoft				PointerOALSoft;
	OpenALSoft();
	OpenALSoft( const OpenALSoft& );
	OpenALSoft& operator= ( const OpenALSoft& );

	void							SetAudioDeviceAndCurrentContext();
	bool							bCanPlayAudio = false;
	ALCdevice*						AudioDevice;
	ALCcontext*						AudioContext;	

	void							CreateAudioSourcePool();
	void							SetAudioSourceData();	
	TMap< uint32, ALuint >			AudioSources;
	TMap< uint32, float >			AudioSourceGain;
	TMap< uint32, bool >			SourceError;
	TMap< uint32, float >			SourceFilterGain;
	ALuint							TemporaryAudioSource;
	FVector							TemporaryAudioSourcePosition;
	float							TemporaryAudioGain = 0.0f;
	TArray< ALuint >				AudioSourcesPool;

	void							CreateBandpassPool();
	void							SetBandpassFilterForAudioSource( uint32 AudioSourceUID );
	void							CreateLowpassPool();
	void							SetLowpassFilterForAudioSource( uint32 AudioSourceUID );
	ALuint							AudioFilter;
	TMap< uint32, ALuint >			BandpassFilters;
	TArray< ALuint >				BandpassPool;	
	TMap< uint32, ALuint >			LowpassFilters;
	TArray< ALuint >				LowpassPool;
	
	void							SetEFXEAXReverb( EFXEAXREVERBPROPERTIES* EAXReverb, ALuint UIDEffect, float CustomGain, float ReverbGain );
	void							UpdateEAXReverb();
	EFXEAXREVERBPROPERTIES*			MainReverb = nullptr;
	TMap< uint32, EEAX >			AudioSourceEAX;
	TMap< uint32, float >			AudioSourceEAXCustomGain;
	EEAX							PreviousEAX = EEAX::None;
	TMap< EEAX, ALuint >			EAXEffectSlots;
	TMap< EEAX, ALuint >			EAXEffects;
	TMap< EEAX, ALuint >			EAXFilters;
	uint8							EAXSlotsCapacity = 4;
	void							LoadAudioAsset( class USoundWave* AudioAsset, bool bAddToMap );
	bool							CatchError( FString ActionName = "" );
	float							VolumeMultiplier = 1.0f;
	float							DopplerFactor = 5.0f;

	UPROPERTY()
	TWeakObjectPtr<class URookAudioDataLoader>		DataLoader = nullptr;	
};