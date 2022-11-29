/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#include "RookPlayAudioNotification.h"
#include "RookPrivatePCH.h"
#include "RookAudioControllerBPL.h"

URookPlayAudioNotification::URookPlayAudioNotification()
{
	NotifyName = UAnimNotify::GetNotifyName_Implementation();
}

void URookPlayAudioNotification::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (AudioBlueprint) { URookAudioControllerBPL::Play(AudioBlueprint, MeshComp->GetOwner()); }		
}

FString URookPlayAudioNotification::GetNotifyName_Implementation() const
{
	return NotifyName;
}