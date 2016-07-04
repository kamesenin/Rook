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