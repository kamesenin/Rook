/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#pragma once
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "RookAudioController.h"
#include "RookPlayAudioNotification.generated.h"

UCLASS( Blueprintable, meta = ( DisplayName = "RookPlayAudioNotification") )
class ROOK_API URookPlayAudioNotification : public UAnimNotify 
{
	GENERATED_BODY()

public:
	URookPlayAudioNotification();

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook|Animation Notification" )
	FString NotifyName;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook|Animation Notification" )
	TSubclassOf<URookAudioController> AudioBlueprint;

	FString GetNotifyName_Implementation() const;
private:
	virtual void Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation ) override;
};