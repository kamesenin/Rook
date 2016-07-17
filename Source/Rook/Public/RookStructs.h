#pragma once
#include "Engine.h"
#include "Runtime/Core/Public/Containers/ContainersFwd.h"
#include "RookEnums.h"
#include "RookStructs.generated.h"

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

USTRUCT( BlueprintType )
struct FSocketAttachment {
	GENERATED_USTRUCT_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	TWeakObjectPtr<class USkeletalMeshSocket>	MeshSocketToFollow = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	TWeakObjectPtr<class ACharacter>		CharacterWithSocket = nullptr;
};

USTRUCT( BlueprintType )
struct FMonoAudioModel {
	GENERATED_USTRUCT_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	TWeakObjectPtr<class USoundWave>			AudioAsset = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	TEnumAsByte<EPhysicalSurface>				SurfaceAffectingAudio = EPhysicalSurface::SurfaceType_Default;
};

USTRUCT( BlueprintType )
struct FAudioSourceModel {
	GENERATED_USTRUCT_BODY()
	/**
	Multichannel audio (more than 1 channel).
	Audio classes which should use it:
	Ambience, EFX, Foley, HUD, Music, SFX, Voice.
	*/
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	TArray<class USoundWave*>						MultiChannelAssets;

	/**
	Mono audio is used for 3D Audio.
	Audio classes which should use it:
	EFX3D, Foley3D, SFX3D, Voice3D.
	*/
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	TArray<FMonoAudioModel>							MonoAssets;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	EPlayback										PlaybackOption = EPlayback::None;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	bool											bUseRandomPitch = false;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	float											TopRandomPitchValue = 0.0f;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	float											BottomRandomPitchValue = 0.0f;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	EEAX											AudioSourceEAX = EEAX::None;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	float											AudioSourceEAXGain = 0.0f;

	/**
	This curve is used for setting volume by calculating distance between parent
	actor and audio listener.
	*/
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	TWeakObjectPtr<class UCurveFloat>				VolumeOverDistanceCurve = nullptr;

	/**
	This curve is used for setting lowpass by calculating distance between parent
	actor and audio listener.
	*/
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	TWeakObjectPtr<class UCurveVector>				LowpassOverDistanceCurve = nullptr;

	/**
	This curve is used for setting bandpass by calculating distance between parent
	actor and audio listener.
	*/
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	TWeakObjectPtr<class UCurveVector>				BandpassOverDistanceCurve = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	bool											bUseVelocity = false;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	bool											bUseRaytrace = false;

	UPROPERTY()
	uint32											AudioSourceID = 0;

	UPROPERTY()
	uint32											ParentID;

	UPROPERTY()
	float											AudioSourceGain = 0.0f;

	UPROPERTY()
	FVector											AudioSourcePosition;

	UPROPERTY()
	float											AudioSourceRandomPitch = 1.0f;

	UPROPERTY()
	TWeakObjectPtr<class USoundWave>				MonoAudioSourceAsset;

	UPROPERTY()
	EAudioState										AudioState = EAudioState::None;
		
	UPROPERTY()
	bool											bInRandom = false;

	UPROPERTY()
	bool											bInSequence = false;
};