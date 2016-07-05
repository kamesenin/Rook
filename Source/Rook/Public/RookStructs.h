#pragma once
#include "Engine.h"
#include "Runtime/Core/Public/Containers/ContainersFwd.h"
#include "RookEnums.h"
#include "RookStructs.generated.h"

USTRUCT(BlueprintType)
struct FAudioSourceData {
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	uint32				AudioSourceID;

	UPROPERTY()
	float				AudioSourceGain;

	UPROPERTY()
	FVector				AudioSourcePosition;

	UPROPERTY()
	bool				bIsAudioSourceLooping = false;	

	UPROPERTY()
	bool				bIsAudioSourceUsingRandomPitch = false;

	UPROPERTY()
	float				AudioSourceRandomPitch = 1.0f;

	UPROPERTY()
	EEAX				AudioSourceEAX = EEAX::None;

	UPROPERTY()
	float				AudioSourceEAXGain = 0.0f;

	UPROPERTY()
	class USoundWave*	AudioSourceAsset;
};

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

USTRUCT(BlueprintType)
struct FSocketAttachment {
	GENERATED_USTRUCT_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	TWeakObjectPtr<class USkeletalMeshSocket>	MeshSocketToFollow = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	TWeakObjectPtr<class ACharacter>		CharacterWithSocket = nullptr;
};