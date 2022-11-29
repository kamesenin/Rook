/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#include "RookEAXVolumeBox.h"
#include "RookPrivatePCH.h"
#include <Components/BoxComponent.h>

//TODO:
//@25.01.19 - exchange physical based detection to InBox detection

ARookEAXVolumeBox::ARookEAXVolumeBox(const class FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
	, EAXVolumeBox(nullptr)
	, VolumeEAX(EEAX::None)
	, RookInterface(nullptr)
{
	if (!EAXVolumeBox) 
	{
		EAXVolumeBox = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("RookEAXVolumeBox"));
		EAXVolumeBox->SetHiddenInGame(true);
		EAXVolumeBox->SetVisibility(true);
		RootComponent = EAXVolumeBox;
	}

	RookInterface = (IRook*)FModuleManager::Get().GetModule("Rook");
}

void ARookEAXVolumeBox::BeginPlay() 
{
	if (EAXVolumeBox) 
	{
		EAXVolumeBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ARookEAXVolumeBox::OnOverlap);
		EAXVolumeBox->OnComponentEndOverlap.AddUniqueDynamic(this, &ARookEAXVolumeBox::EndOfOverlap);
	}
}

void ARookEAXVolumeBox::EndPlay(const EEndPlayReason::Type EndPlayReason) 
{
	if (EAXVolumeBox && EAXVolumeBox->IsValidLowLevel()) 
	{
		EAXVolumeBox->OnComponentBeginOverlap.RemoveDynamic(this, &ARookEAXVolumeBox::OnOverlap);
		EAXVolumeBox->OnComponentEndOverlap.RemoveDynamic(this, &ARookEAXVolumeBox::EndOfOverlap);
	}
}

void ARookEAXVolumeBox::OnOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (RookInterface) { RookInterface->OnEAXOverlap.Broadcast(OtherActor->GetUniqueID(), VolumeEAX); }		
}

void ARookEAXVolumeBox::EndOfOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) 
{
	if (RookInterface) { RookInterface->OnEAXOverlap.Broadcast(OtherActor->GetUniqueID(), EEAX::None); }		
}

ARookEAXVolumeBox::~ARookEAXVolumeBox()
{
	RookInterface = nullptr;
}