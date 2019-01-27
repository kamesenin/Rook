/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#include "RookPrivatePCH.h"
#include "RookStopAudioNotification.h"
#include "RookAudioControllerBPL.h"

URookStopAudioNotification::URookStopAudioNotification() 
{
	NotifyName = UAnimNotify::GetNotifyName_Implementation();
}

void URookStopAudioNotification::Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation ) 
{
	if (AudioBlueprint) { URookAudioControllerBPL::Stop(AudioBlueprint); }		
}

FString URookStopAudioNotification::GetNotifyName_Implementation() const 
{
	return NotifyName;
}