/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#pragma once
#include "Engine.h"
#include "Runtime/Core/Public/Containers/ContainersFwd.h"
#include "RookEnums.h"
#include "RookStructs.generated.h"
/** Rook struct for setting roatation freez. It can freez any axies with given value */
USTRUCT(BlueprintType)
struct FFreezRotation 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	EFreezRotationState		FreezXAxies;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	float					XAxiesRotation;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	EFreezRotationState		FreezYAxies;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	float					YAxiesRotation;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	EFreezRotationState		FreezZAxies;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	float					ZAxiesRotation;

	FFreezRotation()
		: FreezXAxies(EFreezRotationState::NoFreez)
		, XAxiesRotation(0.0f)
		, FreezYAxies(EFreezRotationState::NoFreez)
		, YAxiesRotation(0.0f)
		, FreezZAxies(EFreezRotationState::NoFreez)
		, ZAxiesRotation(0.0f)
	{
	}
};

/** Rook struct for attaching Listener Controller to a providen socket. Not only proper socket has to be set, but also a proper Character */
USTRUCT( BlueprintType )
struct FSocketAttachment 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	TWeakObjectPtr<class USkeletalMeshSocket>	MeshSocketToFollow;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	TWeakObjectPtr<class ACharacter>		CharacterWithSocket;

	FSocketAttachment()
		: MeshSocketToFollow(nullptr)
		, CharacterWithSocket(nullptr)
	{}
};

/** Rook struct for setting mono (3D) audio asset. SurfaceAffectingAudio is optional, however it can be use for ex. dynamic footsteps */
USTRUCT( BlueprintType )
struct FMonoAudioModel
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rook Audio")
		class USoundWave*							AudioAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rook Audio")
		TEnumAsByte<EPhysicalSurface>				SurfaceAffectingAudio;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rook Audio")
		class UCurveVector*							FadeCurve;

	FMonoAudioModel()
		: AudioAsset(nullptr)
		, SurfaceAffectingAudio(EPhysicalSurface::SurfaceType_Default)
		, FadeCurve(nullptr)
	{}
};

/** Rook struct for setting multichannel (+1) audio asset. Audio gain is in decibels - range from 0 to -100 */
USTRUCT(BlueprintType)
struct FMultichannelAudioModel 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	class USoundWave*							AudioAsset;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	float										Decibels;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	class UCurveVector*							FadeCurve;

	FMultichannelAudioModel()
		: AudioAsset(nullptr)
		, Decibels(0.0f)
		, FadeCurve(nullptr)
	{}
};

/** Rook struct for constructing new audio source */
USTRUCT( BlueprintType )
struct FAudioSourceModel 
{
	GENERATED_USTRUCT_BODY()

	/** Audio Bus */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	EAudioBus									AudioBus;
	/**
	Multichannel audio (more than 1 channel).
	Audio classes which should use it:
	Ambience, EFX, Foley, HUD, Music, SFX, Voice.
	*/
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	TArray<FMultichannelAudioModel>				MultiChannelAssets;

	/**
	Mono audio is used for 3D Audio.
	Audio classes which should use it:
	EFX3D, Foley3D, SFX3D, Voice3D.
	*/
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	TArray<FMonoAudioModel>						MonoAssets;

	/** 
	Sets what kinda playback option use wants
	Playback options: Single, Loop, Random, SingleRandom, Sequence,	SingleSequence
	*/
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	EPlayback									PlaybackOption;
	/** Should audio source use random pitch ? */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	bool										bUseRandomPitch;

	/** If using random pitch please provide top value for randomnes */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	float										TopRandomPitchValue;

	/** If using random pitch please provide bottom value for randomnes */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	float										BottomRandomPitchValue;

	/** EAX reverb effect which audio source should use. By defualt it is not using it */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	EEAX										AudioSourceEAX;

	/** If there is EAX reverb set user can set gain (volume) of it */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	float										AudioSourceEAXGain;

	/**
	This curve is used for setting volume by calculating distance between parent
	actor and audio listener.
	*/
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	class UCurveFloat*							VolumeOverDistanceCurve;

	/**
	This curve is used for setting lowpass by calculating distance between parent
	actor and audio listener.
	*/
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	class UCurveVector*							LowpassOverDistanceCurve;

	/**
	This curve is used for setting bandpass by calculating distance between parent
	actor and audio listener.
	*/
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	class UCurveVector*							BandpassOverDistanceCurve;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	class UCurveVector*							FadeCurve;

	/** Should audio source by affected by attachment velocity */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	bool										bUseVelocity;

	/** Raytrace is very primitive way to calculate dynamic audio gain and lowpass. */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	bool										bUseRaytrace;
	
	/** Helps to limit playing same Audio Controller on different actors */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rook Audio")
	uint8										PlayLimit;

	/** Internal unique id of audio source */
	UPROPERTY()
	uint32										AudioSourceID;

	/** Parent id is a helper for finding/checking parent actor of audio source */
	UPROPERTY()
	uint32										ParentID;

	/** Initial audio source gain */
	UPROPERTY()
	float										AudioSourceGain;

	/** Initial audio source position */
	UPROPERTY()
	FVector										AudioSourceLocation;
	
	/** Initial audio source pitch */
	UPROPERTY()
	float										AudioSourceRandomPitch;

	/** Mono audio asset used while setting new OpenAL Soft source and track current position on track */
	UPROPERTY()
	TWeakObjectPtr<class USoundWave>			CurrentAudioSourceAsset;

	UPROPERTY()
	TWeakObjectPtr<class UCurveVector>			IndividualFadeCurve;

	/** Helper Rook enum. Audio can have states like: Playing, Stopped, Paused, WasPlaying */
	UPROPERTY()
	EAudioState									AudioState;
		
	/** Helper boolean. If source is finished and in sequence, it will automaticly play next track in line */
	UPROPERTY()
	bool										bInSequence;

	/** Initial audio type - simplifies check without going trough audio bus */
	UPROPERTY()
	EAudioType									AudioType;
	/** Fade factor is used for 3D audio assets */
	UPROPERTY()
	float										FadeFactor;
	/** Helper to determin if 3D audio should fade in */
	UPROPERTY()
	bool										bFadeIn;
	/** Helper. It determins at which point in time 3D source should fade out */
	UPROPERTY()
	float										StartFadeOutAt;
	/** Helper float. It track current position on track */
	UPROPERTY()
	float										OverallTime;

	UPROPERTY()
	FName										Tag;

	FAudioSourceModel()
		: AudioBus(EAudioBus::SFX3D)
		, PlaybackOption(EPlayback::Single)
		, bUseRandomPitch(false)
		, TopRandomPitchValue(1.5f)
		, BottomRandomPitchValue(0.5f)
		, AudioSourceEAX(EEAX::None)
		, AudioSourceEAXGain(0.0f)
		, VolumeOverDistanceCurve(nullptr)
		, LowpassOverDistanceCurve(nullptr)
		, BandpassOverDistanceCurve(nullptr)
		, FadeCurve(nullptr)
		, bUseVelocity(false)
		, bUseRaytrace(false)
		, PlayLimit(10)
		, AudioSourceID(0)
		, ParentID(0)
		, AudioSourceGain(0.0f)
		, AudioSourceLocation(FVector::ZeroVector)
		, AudioSourceRandomPitch(1.0f)
		, CurrentAudioSourceAsset(nullptr)
		, IndividualFadeCurve(nullptr)
		, AudioState(EAudioState::None)
		, bInSequence(false)
		, AudioType(EAudioType::is2D)
		, FadeFactor(1.0f)
		, bFadeIn(false)
		, StartFadeOutAt(0.0f)
		, OverallTime(0.0f)
		, Tag("")
	{}
};

USTRUCT( BlueprintType )
struct FMultichannelFadeModel 
{
	GENERATED_USTRUCT_BODY()
	/** Helper bool. True if multichannel audio should fade in */
	UPROPERTY()
	bool										bFadeIn;
	/** Fade factor is used for multichannel audio */
	UPROPERTY()
	float										FadeFactor;
	/** When in time multichannel audio should start to fade out */
	UPROPERTY()
	float										StartFadeOutAt;
	/** Helper weak pointer to fade vector curve */
	UPROPERTY()
	TWeakObjectPtr<class UCurveVector>			FadeCurve;
	/** Temporary time for vector curve - used for fade in and out */
	UPROPERTY()
	float										FadeTime;
	/** Overall playback time of multichannel audio source */
	UPROPERTY()
	float										OverallTime;
	/** Helper float. Holdes volume and helps to calculate fade volume */
	UPROPERTY()
	float										InternalVolume;

	FMultichannelFadeModel()
		: bFadeIn(false)
		, FadeFactor(1.0f)
		, StartFadeOutAt(0.0f)
		, FadeCurve(nullptr)
		, FadeTime(0.0f)
		, OverallTime(0.0f)
		, InternalVolume(0.0f)
	{}
};