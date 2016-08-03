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
struct FFreezRotation {
	GENERATED_USTRUCT_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	EFreezRotationState		FreezXAxies = EFreezRotationState::NoFreez;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	float					XAxiesRotation = 0.0f;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	EFreezRotationState		FreezYAxies = EFreezRotationState::NoFreez;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	float					YAxiesRotation = 0.0f;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	EFreezRotationState		FreezZAxies = EFreezRotationState::NoFreez;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	float					ZAxiesRotation = 0.0f;
};

/** Rook struct for attaching Listener Controller to a providen socket. Not only proper socket has to be set, but also a proper Character */
USTRUCT( BlueprintType )
struct FSocketAttachment {
	GENERATED_USTRUCT_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	TWeakObjectPtr<class USkeletalMeshSocket>	MeshSocketToFollow = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	TWeakObjectPtr<class ACharacter>		CharacterWithSocket = nullptr;
};

/** Rook struct for setting mono (3D) audio asset. SurfaceAffectingAudio is optional, however it can be use for ex. dynamic footsteps */
USTRUCT( BlueprintType )
struct FMonoAudioModel {
	GENERATED_USTRUCT_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	class USoundWave*							AudioAsset = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	TEnumAsByte<EPhysicalSurface>				SurfaceAffectingAudio = EPhysicalSurface::SurfaceType_Default;
};

/** Rook struct for setting multichannel (+1) audio asset. Audio gain is in decibels - range from 0 to -100 */
USTRUCT(BlueprintType)
struct FMultichannelAudioModel {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rook Audio")
	class USoundWave*							AudioAsset = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rook Audio")
	float										Decibels = 1.0f;
};

/** Rook struct for constructing new audio source */
USTRUCT( BlueprintType )
struct FAudioSourceModel {
	GENERATED_USTRUCT_BODY()

	/** Audio Bus */
	EAudioBus										AudioBus = EAudioBus::SFX3D;
	/**
	Multichannel audio (more than 1 channel).
	Audio classes which should use it:
	Ambience, EFX, Foley, HUD, Music, SFX, Voice.
	*/
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	TArray<FMultichannelAudioModel>					MultiChannelAssets;

	/**
	Mono audio is used for 3D Audio.
	Audio classes which should use it:
	EFX3D, Foley3D, SFX3D, Voice3D.
	*/
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	TArray<FMonoAudioModel>							MonoAssets;

	/** 
	Sets what kinda playback option use wants
	Playback options: Single, Loop, Random, SingleRandom, Sequence,	SingleSequence
	*/
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	EPlayback										PlaybackOption = EPlayback::Single;
	/** Should audio source use random pitch ? */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	bool											bUseRandomPitch = false;

	/** If using random pitch please provide top value for randomnes */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	float											TopRandomPitchValue = 1.5f;

	/** If using random pitch please provide bottom value for randomnes */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	float											BottomRandomPitchValue = 0.5f;

	/** EAX reverb effect which audio source should use. By defualt it is not using it */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	EEAX											AudioSourceEAX = EEAX::None;

	/** If there is EAX reverb set user can set gain (volume) of it */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	float											AudioSourceEAXGain = 0.0f;

	/**
	This curve is used for setting volume by calculating distance between parent
	actor and audio listener.
	*/
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	class UCurveFloat*								VolumeOverDistanceCurve = nullptr;

	/**
	This curve is used for setting lowpass by calculating distance between parent
	actor and audio listener.
	*/
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	class UCurveVector*								LowpassOverDistanceCurve = nullptr;

	/**
	This curve is used for setting bandpass by calculating distance between parent
	actor and audio listener.
	*/
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	class UCurveVector*								BandpassOverDistanceCurve = nullptr;

	/** Should audio source by affected by attachment velocity */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	bool											bUseVelocity = false;

	/** Raytrace is very primitive way to calculate dynamic audio gain and lowpass. */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	bool											bUseRaytrace = false;
	
	/** Helps to limit playing same Audio Controller on different actors */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rook Audio")
	uint8											PlayLimit = 10;

	/** Internal unique id of audio source */
	UPROPERTY()
	uint32											AudioSourceID = 0;

	/** Parent id is a helper for finding/checking parent actor of audio source */
	UPROPERTY()
	uint32											ParentID;

	/** Initial audio source gain */
	UPROPERTY()
	float											AudioSourceGain = 0.0f;

	/** Initial audio source position */
	FVector											AudioSourcePosition;

	/** Initial audio source pitch */
	UPROPERTY()
	float											AudioSourceRandomPitch = 1.0f;

	/** Mono audio asset used while setting new OpenAL Soft source and track current position on track */
	UPROPERTY()
	TWeakObjectPtr<class USoundWave>				MonoAudioSourceAsset;

	/** Helper Rook enum. Audio can have states like: Playing, Stopped, Paused, WasPlaying */
	UPROPERTY()
	EAudioState										AudioState = EAudioState::None;
		
	/** Helper boolean. If source is finished and in sequence, it will automaticly play next track in line */
	UPROPERTY()
	bool											bInSequence = false;

	/** Initial audio type - simplifies check without going trough audio bus */
	UPROPERTY()
	EAudioType										AudioType = EAudioType::is2D;
};