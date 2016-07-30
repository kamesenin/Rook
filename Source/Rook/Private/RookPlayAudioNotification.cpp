/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#include "RookPrivatePCH.h"
#include "RookPlayAudioNotification.h"
#include "RookAudioControllerBPL.h"

URookPlayAudioNotification::URookPlayAudioNotification() {
	NotifyName = UAnimNotify::GetNotifyName_Implementation();
}

void URookPlayAudioNotification::Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation ) {
	if ( AudioBlueprint )	
		URookAudioControllerBPL::Play( AudioBlueprint, MeshComp->GetOwner() );
}

FString URookPlayAudioNotification::GetNotifyName_Implementation() const {
	return NotifyName;
}