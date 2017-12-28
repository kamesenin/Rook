/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#pragma once
#include "Runtime/Engine/Classes/Engine/EngineBaseTypes.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "RookAudioDataLoader.generated.h"

UCLASS()
class ROOK_API URookAudioDataLoader : public UObject, public FTickableGameObject 
{
	GENERATED_BODY()
public:
	URookAudioDataLoader() {};
	virtual ~URookAudioDataLoader() {};

	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	virtual void Tick(float DeltaTime) override;

	FGraphEventArray LoadingGraph;
};