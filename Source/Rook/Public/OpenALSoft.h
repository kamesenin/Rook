/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
/////////////////////////////////////////////////////////////
/// Main OpenAL Soft class
////////////////////////////////////////////////////////////

#pragma once
#include <iostream>
#include <string>
#include "RookEnums.h"
#include "RookStructs.h"
#include "OpenALSoft/include/AL/al.h"
#include "OpenALSoft/include/AL/alc.h"
#include "OpenALSoft/include/AL/efx-creative.h"
#include "OpenALSoft/include/AL/efx.h"
#include "OpenALSoft/include/AL/alext.h"
#include "OpenALSoft/include/AL/efx-presets.h"

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

class OpenALSoft 
{
public:
	/** Istance of OpenAL Soft class */
	static OpenALSoft&			Instance();
	~OpenALSoft(){};
	/**
	Set up audio source data and then plays it.
	@param SourceData - its model from Audio Controller containing data to set up new audio source
	@return boolean - true if data was set up proper and audio started to play
	*/
	bool						Play( const FAudioSourceModel SourceData );
	/**
	Plays audio from given track position
	@param AudioSourceUID - audio source unique id
	@param Seconds - position on track in seconds
	*/
	void						PlayAt( const uint32 AudioSourceUID, const float Seconds );
	/**
	Resumes audio after it was paused
	@param AudioSourceUID - audio source unique id
	*/
	void						PlayAfterPause( const uint32 AudioSourceUID );
	/**
	Stops audio
	@param AudioSourceUID - audio source unique id
	*/
	void						Stop( const uint32 AudioSourceUID );
	/**
	Pause audio
	@param AudioSourceUID - audio source unique id
	*/
	void						Pause( const uint32 AudioSourceUID );
	/**
	Changes audio source gain (volume)
	@param AudioSourceUID - audio source unique id
	@param Gain - value in range 0.0f - 100.0f
	*/
	void						ChangeAudioSourceGain( const uint32 AudioSourceUID, float Gain );
	/**
	Helper function. Returns current gain of audio source
	@param AudioSourceUID - audio source unique id
	@return current gain
	*/
	float						GetAudioSourceGain( const uint32 AudioSourceUID );
	/**
	Remove audio source from pool and delete data in OpenAL Soft
	@param AudioSourceUID - audio source unique id
	@return true if removing when successful
	*/
	bool						RemoveAudioSource( const uint32 AudioSourceUID );
	/**
	Check if audio source with given id is used
	@param AudioSourceUID - audio source unique id
	@return true if audio source exists
	*/
	bool						IsAnAudioSource( const uint32 AudioSourceUID );
	/**
	Helper function. Checks if with given id is playing
	@param AudioSourceUID - audio source unique id
	@return true if its playing
	*/
	bool						IsAudioSourcePlaying( const uint32 AudioSourceUID );
	/**
	Helper function. Gives more detail infromation of current audio source state
	@param AudioSourceUID - audio source unique id
	@return Playing, Stopped or Paused or None if audio source does not exists
	*/
	EAudioState					AudioSourceState( const uint32 AudioSourceUID );
	/**
	Changing looping state of audio source
	@param AudioSourceUID - audio source unique id
	@param bShouldLoop - true for looping ,false for stop looping
	*/
	void						SetLooping ( const uint32 AudioSourceUID, const bool bShouldLoop );
	/**
	Changes current pitch of audio source
	@param AudioSourceUID - audio source unique id
	@param Pitch - value of the pitch which should be set up
	*/
	void						ChangeAudioSourcePitch( const uint32 AudioSourceUID, const float Pitch );
	/**
	Updates audio source position
	@param AudioSourceUID - audio source unique id
	@param AudioSourcePosition - vector with current audio source position
	*/
	void						UpdateAudioSourcePosition( const uint32 AudioSourceUID, const FVector AudioSourcePosition );
	/**
	Updates audio source velocity
	@param AudioSourceUID - audio source unique id
	@param Velocity - vector with current audio source velocity
	*/
	void						UpdateAudioSourceVelocity( const uint32 AudioSourceUID, const FVector Velocity );
	/**
	Changes overall doppler effect
	@param DopplerFactor - value which will be apply for doppler effect
	*/
	void						ChangeDopplerFactor( const float DopplerFactor );
	/**
	Helper function. Returns current position on audio track
	@return current position on audio track
	*/
	float						CurrentPositionOnAudioTrack( const uint32 AudioSourceUID );
	/** Basic setup function for audio listener */
	void						SetUpAudioListener();
	/**
	Updates audio listener position
	@param ListenerPosition - vector of listener position
	@param ListenerForwardVector - listener forward vector
	@param ListenerUpVector - listener up vector
	*/
	void						UpdateAudioListenerePosition( const FVector ListenerPosition, const FVector ListenerForwardVector, const FVector ListenerUpVector );
	/**
	Updates audio listener velocity
	@param Velocity - velocity vector
	*/
	void						UpdateAudioListenerVelocity( const FVector Velocity );
	
	/**
	Changes bandpass on audio source
	@param AudioSourceUID - audio source unique id
	@param HFGain - bandpass high frequency gain
	@param LFGain - bandpass low frequency gain
	*/
	void						ChangeBandpassFilter( const uint32 AudioSourceUID, const float HFGain, const float LFGain );
	/**
	Changes lowpass on audio source
	@param AudioSourceUID - audio source unique id
	@param Gain - lowpass gain
	@param LFGain - lowpass high frequency gain
	*/
	void						ChangeLowpassFilter( const uint32 AudioSourceUID, const float Gain, const float HFGain );
	/**
	Sets EAX reverb effect on given audio source.
	@param EAX - enum for EAX reverb effect
	@param AudioSourceUID - audio source unique id
	@param EAXGain - gain of EAX
	*/
	void						SetEAXOnAudioSource( const EEAX EAX, const uint32 AudioSourceUID, const float EAXGain = 0.32f );
	/**
	Sets EAX reverb effect on every audio source which uses it.
	@param EAX - enum for EAX reverb effect
	@param EAXGain - gain of EAX
	*/
	void						SetMainReverb( const EEAX EAX, const float EAXGain = 0.32f );
	/**
	Clears data in buffer tmap and delete buffer data in OpenAL Soft
	@param SoundWaveUID - sound wave asset unique id
	*/
	void						CleanAudioDataInBuffer( const uint32 SoundWaveUID );
	/** Set overall volume multiplier - it affect all 3D sounds
	@param NewVolMultiplier - value from 0.0f to 1.0f
	*/
	void						SetVolumeMulitiplier( float NewVolMultiplier );
	/** Helper function if user want to check if given audio source produce error
	@param AudioSourceUID - audio source unique id
	@return true if there was an error
	*/
	bool						HadError( const uint32 AudioSourceUID ) const;
	/** Clears error entry of give audio source
	@param AudioSourceUID - audio source unique id
	*/
	void						CleanErrorEntry( const uint32 AudioSourceUID );
	/** Returns current available audio sources in pool */
	uint16						GetNumberOfAvailableAudioSourcesInPool() const;
	/** Clear data, remove audio source, buffers, destroy context and closes device */
	void						CloseDeviceAndDestroyCurrentContext();
	/** Helper for checking if OpenAL Soft Dll was loaded */
	bool						IsDLLLoaded() const;
public:
	/** Was audio device was set up proper */
	bool						bHasDeviceBeenSet;
	/** Current number of audio sources in use */
	uint16						CurrentLiveSourceCount;
	/** Maximum number of 3D audio source that can play in one tick */
	uint16						MaximumAvailableAudioChannels;
	/** Helper boolean. True if audio source pool is empty */
	bool						bHasReachPlayLimit;
	/** Buffer tmap. Key is audio asset unique id, value is OpenAL Soft data */
	TMap< uint32, ALuint >		Buffers;
	/** OpenAL Soft buffer generator */
	DefAlGenBuffers				OALGenBuffers;
	/** OpenAL Soft buffer data generator */
	DefAlBufferData				OALBufferData;
private:
	//////////////////////////////////////////////////////////////////////////
	//DLL
	//////////////////////////////////////////////////////////////////////////
		
	void							ProcessDLL();	
	void							FreeOpenALDll();
	void*							OALDLLHandler;
	bool							bWasDllLoaded;
	
	DefAlGetError					OALGetError;
	DefAlEnable						OALEnable;

	DefAlcOpenDevice				OALOpenDevice;
	DefAlcCloseDevice				OALCloseDevice;
	DefAlcCreateContext				OALCreateContext;
	DefAlcDestroyContext			OALDestroyContext;
	DefAlcMakeContextCurrent		OALMakeContextCurrent;
		
	DefAlDeleteBuffers				OALDeleteBuffers;

	DefAlListener3f					OALListener3f;
	DefAlListenerfv					OALListenerfv;

	DefAlGenSources					OALGenSources;
	DefAlDeleteSources				OALDeleteSoruces;
	DefAlSource3f					OALSource3f;
	DefAlSourcef					OALSourcef;
	DefAlSourcei					OALSourcei;
	DefAlSourceiv					OALSourceiv;
	DefAlSource3i					OALSource3i;
	DefAlSourcePlay					OALSourcePlay;
	DefAlSourceStop					OALSourceStop;
	DefAlSourcePause				OALSourcePause;
	DefAlIsSource					OALIsSource;

	DefAlIsExtensionPresent			OALIsExtensionPresent;
	DefAlcIsExtensionPresent		OALIsAlcExtensionPresent;

	DefAlcGetString					OALGetString;
	DefAlcGetIntegerv				OALGetIntegerv;
	DefAlGetSourcei					OALGetSourcei;
	DefAlGetSourcef					OALGetSourcef;
	DefAlGetEnumValue				OALGetEnumValue;

	DefAlcDeviceEnableHrtf			OALEnableHrtf;

	DefAlGenEffects					OALGenEffects;
	DefAlDeleteEffects				OALDeleteEffects;
	DefAlIsEffect					OALIsEffect;
	DefAlGenAuxiliaryEffectSlots	OALGenAuxiliaryEffectSlot;
	DefAlDeleteAuxiliaryEffectSlots	OALDeleteAuxiliaryEffectSlot;
	DefAlIsAuxiliaryEffectSlot		OALIsAuxiliaryEffectSlot;
	DefAlAuxiliaryEffectSloti		OALAuxiliaryEffectSloti;

	DefAlEffecti					OALEffecti;
	DefAlEffectiv					OALEffectiv;
	DefAlEffectf					OALEffectf;
	DefAlEffectfv					OALEffectfv;

	DefAlGenFilters					OALGenFilters;
	DefAlDeleteFilters				OALDeleteFilters;
	DefAlFilteri					OALFilteri;
	DefAlFilteriv					OALFilteriv;
	DefAlFilterf					OALFilterf;
	DefAlFilterfv					OALFilterfv;

	DefAlDopplerFactor				OALDopplerFactor;
	DefAlDopplerVelocity			OALDopplerVelocity;

	DefAlDistanceModel				OALDistanceModel;

	//////////////////////////////////////////////////////////////////////////
	static OpenALSoft				PointerOALSoft;
	OpenALSoft();
	OpenALSoft( const OpenALSoft& );
	OpenALSoft& operator= ( const OpenALSoft& );

	/** Set up current audio context and audio device */
	void							SetAudioDeviceAndCurrentContext();
	/** Helper boolean */
	bool							bCanPlayAudio;
	/** OpenAL Soft audio device */
	ALCdevice*						AudioDevice;
	/** OpenAL Soft audio context */
	ALCcontext*						AudioContext;	
	/** Function creating audio source pool based on maximum available audio channels */
	void							CreateAudioSourcePool();
	/** Creates OpenAL Soft audio source with providen data */
	void							SetAudioSourceData();
	/** TMap of current audio sources. Key is audio source unique id, value is OpenAL Soft audio source data */
	TMap< uint32, ALuint >			AudioSources;
	/** TMap of audio sources gain. Key is audio source unique id, value is current volume */
	TMap< uint32, float >			AudioSourceGain;
	/** TMap of current errors. Key is audio source unique id, value is boolean - true if it had error */
	TMap< uint32, bool >			SourceError;
	/** Helper variable while setting new audio source */
	ALuint							TemporaryAudioSource;
	/** Helper FVector while creating new audio source */
	FVector							TemporaryAudioSourcePosition;
	/** Helper variable while setting new audio source */
	float							TemporaryAudioGain;
	/** Array containing pool of OpenAL Soft audio source */
	TArray< ALuint >				AudioSourcesPool;
	/** Function creating bandpass filter pool - size depends of maximum available audio channels */
	void							CreateBandpassPool();
	/** Function sets bandpass filter on given audio source
	@param AudioSourceUID - audio source unique id
	*/
	void							SetBandpassFilterForAudioSource( const uint32 AudioSourceUID );
	/** Function removes bandpass filter on given audio source
	@param AudioSourceUID - audio source unique id
	*/
	void							RemoveBandpassFilterForAudioSource( const uint32 AudioSourceUID );
	/** Function creating lowpass filter pool - size depends of maximum available audio channels */
	void							CreateLowpassPool();
	/** Function sets lowpass filter on given audio source
	@param AudioSourceUID - audio source unique id
	*/
	void							SetLowpassFilterForAudioSource( const uint32 AudioSourceUID );
	/** Function removes lowpass filter on given audio source
	@param AudioSourceUID - audio source unique id
	*/
	void							RemoveLowpassFilterForAudioSource( const uint32 AudioSourceUID );
	/** TMap of bandpass filters in use. Key is audio source unique id, value is bandpass filter */
	TMap< uint32, ALuint >			BandpassFilters;
	/** Pool of available bandpass filters */
	TArray< ALuint >				BandpassPool;	
	/** TMap of lowpass filters in use. Key is audio source unique id, value is lowpass filter */
	TMap< uint32, ALuint >			LowpassFilters;
	/** Pool of available lowpass filters */
	TArray< ALuint >				LowpassPool;
	/** Function that sets EAX reverb effect values on given unique given effect
	@param EAXReverb - OpenAL Soft EAX reverb properties (check RookUtils TMap)
	@param UIDEffect - unique effect
	@param CustomGain - EAX reverb gain
	@param ReverbGain - used when given EAX is different than main EAX. Its audio source origin gain. Helps to mix EAX's
	*/
	void							SetEFXEAXReverb( EFXEAXREVERBPROPERTIES* EAXReverb, ALuint UIDEffect, const float CustomGain, const float ReverbGain );
	/** Updates gain on audio sources which uses EAX */
	void							UpdateEAXReverbGain();
	/** Pointer to main EAX reverb effect */
	EFXEAXREVERBPROPERTIES*			MainReverb = nullptr;
	/** TMap with audio sources using EAX. Key is unique audio source id, value is EAX enum */
	TMap< uint32, EEAX >			AudioSourceEAX;
	/** TMap with audio sources EAX gain. Key is unique audio source id, value is current EAX gain */
	TMap< uint32, float >			AudioSourceEAXCustomGain;
	/** TMap of available EAX effect slots. Maxiumum capacity is determin by EAXSlotsCapacity. It helps to reuse EAX reverb effect. Key is EAX enum, value is OpenAL Soft effect slot */
	TMap< EEAX, ALuint >			EAXEffectSlots;
	/** TMap containing EAX effects. Key is EAX enum, value is OpenAL Soft EAX effect */
	TMap< EEAX, ALuint >			EAXEffects;
	/** TMap containing EAX effect filters. Key is EAX enum, value is OpenAL Soft EAX effect filter */
	TMap< EEAX, ALuint >			EAXFilters;
	/** EAX slot capacity is very important to not overload memory/cpu usage. Recommended and reasonable amount is 4 */
	uint8							EAXSlotsCapacity;
	/** 
	Helper function for loading data of given audio asset.
	@param AudioAsset - Unreal audio asset
	@param bAddToMap - true if in buffer TMap should be already an entry with 0 (no data)
	*/
	void							LoadAudioAsset( const TWeakObjectPtr<class USoundWave> AudioAsset, const bool bAddToMap );
	/** 
	Helper function to print out errors
	@param ActionName - helps to parametrize log output
	@return true if there was an error
	*/
	bool							CatchError( const FString ActionName = "" );
	/** Helper value for global control of audio gain multiplier */
	float							VolumeMultiplier;
	/** Helper value for doppler factor when/if should be used */
	float							DopplerFactor;
	/** Weak pointer to data loader class */
	UPROPERTY()
	TWeakObjectPtr<class URookAudioDataLoader>		DataLoader;	
	/**
	Function internaly creates list of aviable audio devices. By default it will choose device which has highest 
	amount of audio channels, opens it and returns. 
	@returns AudioDevice raw pointer	
	**/
	ALCdevice* GetBestAudioDevice();
};