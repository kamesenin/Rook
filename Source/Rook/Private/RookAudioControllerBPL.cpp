#include "RookPrivatePCH.h"
/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#include "RookAudioControllerBPL.h"
#include "RookAudioController.h"
#include "RookEnums.h"

URookAudioController* URookAudioControllerBPL::Play(TSubclassOf<URookAudioController> AudioBlueprint, AActor* Parent, FName Tag)
{
	if (!AudioBlueprint || !Parent)
		return nullptr;

	TWeakObjectPtr<URookAudioController> TemporaryAudioController = Cast<URookAudioController>(AudioBlueprint->ClassDefaultObject);
	TemporaryAudioController->Play(Parent, Tag);
	return TemporaryAudioController.Get();
}

bool URookAudioControllerBPL::Stop(TSubclassOf<URookAudioController> AudioBlueprint)
{
	if (!AudioBlueprint)
		return false;

	TWeakObjectPtr<URookAudioController> TemporaryAudioController = Cast<URookAudioController>(AudioBlueprint->ClassDefaultObject);
	TemporaryAudioController->Stop();
	TemporaryAudioController = nullptr;
	return true;
}

bool URookAudioControllerBPL::Pause(TSubclassOf<URookAudioController> AudioBlueprint)
{
	if (!AudioBlueprint)
		return false;

	TWeakObjectPtr<URookAudioController> TemporaryAudioController = Cast<URookAudioController>(AudioBlueprint->ClassDefaultObject);
	TemporaryAudioController->Pause();
	TemporaryAudioController = nullptr;
	return true;
}

bool URookAudioControllerBPL::ChangeSurfaceType(TSubclassOf<class URookAudioController> AudioBlueprint, TEnumAsByte<EPhysicalSurface> NewSurface)
{
	if (!AudioBlueprint)
		return false;

	TWeakObjectPtr<URookAudioController> TemporaryAudioController = Cast<URookAudioController>(AudioBlueprint->ClassDefaultObject);
	TemporaryAudioController->ChangeSurface(NewSurface);
	TemporaryAudioController = nullptr;
	return true;
}

bool URookAudioControllerBPL::ChangeEAXType(TSubclassOf<class URookAudioController> AudioBlueprint, EEAX NewEAX)
{
	if (!AudioBlueprint)
		return false;

	TWeakObjectPtr<URookAudioController> TemporaryAudioController = Cast<URookAudioController>(AudioBlueprint->ClassDefaultObject);
	TemporaryAudioController->ChangeEAX(NewEAX);
	TemporaryAudioController = nullptr;
	return true;
}

bool URookAudioControllerBPL::DebugLines(TSubclassOf<class URookAudioController> AudioBlueprint, EEnableState UseDebuLines)
{
	if (!AudioBlueprint)
		return false;

	TWeakObjectPtr<URookAudioController> TemporaryAudioController = Cast<URookAudioController>(AudioBlueprint->ClassDefaultObject);
	TemporaryAudioController->bUseDebugSpheres = UseDebuLines == EEnableState::Enable;
	TemporaryAudioController = nullptr;
	return true;
}

bool URookAudioControllerBPL::ChangeDecibelsOnActiveMultichannel(TSubclassOf<class URookAudioController> AudioBlueprint, float dB)
{
	if (!AudioBlueprint)
		return false;

	TWeakObjectPtr<URookAudioController> TemporaryAudioController = Cast<URookAudioController>(AudioBlueprint->ClassDefaultObject);
	TemporaryAudioController->ChangeDecibelsOnActiveMultichannel(dB);
	TemporaryAudioController = nullptr;
	return true;
}