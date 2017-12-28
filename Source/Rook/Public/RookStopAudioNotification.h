/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#pragma once
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "RookAudioController.h"
#include "RookStopAudioNotification.generated.h"

UCLASS( Blueprintable, meta = ( DisplayName = "RookStopAudioNotification" ) )
class ROOK_API URookStopAudioNotification : public UAnimNotify 
{
	GENERATED_BODY()

public:
	URookStopAudioNotification();
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook|Animation Notification" )
	FString NotifyName;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook|Animation Notification" )
	TSubclassOf<URookAudioController> AudioBlueprint;

	FString GetNotifyName_Implementation() const;
private:
	virtual void Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation ) override;
};