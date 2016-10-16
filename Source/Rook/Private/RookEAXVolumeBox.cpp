/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#include "RookPrivatePCH.h"
#include "RookEAXVolumeBox.h"
#include <Components/BoxComponent.h>

ARookEAXVolumeBox::ARookEAXVolumeBox( const class FObjectInitializer& ObjectInitializer ) : Super( ObjectInitializer ) {
	if ( !EAXVolumeBox ) {
		EAXVolumeBox = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>( this, TEXT("RookEAXVolumeBox") );
		EAXVolumeBox->SetHiddenInGame( true );
		EAXVolumeBox->SetVisibility( true );
		RootComponent = EAXVolumeBox;
	}	

	TSharedPtr<IModuleInterface> RookModule = FModuleManager::Get().GetModule("Rook");

	if ( RookModule.IsValid() ) 
		RookInterface = StaticCastSharedPtr<IRook>( RookModule );	
}

void ARookEAXVolumeBox::BeginPlay() {
	if ( EAXVolumeBox ) {
		EAXVolumeBox->OnComponentBeginOverlap.AddUniqueDynamic( this, &ARookEAXVolumeBox::OnOverlap );
		EAXVolumeBox->OnComponentEndOverlap.AddUniqueDynamic( this, &ARookEAXVolumeBox::EndOfOverlap );
	}	
}

void ARookEAXVolumeBox::EndPlay( const EEndPlayReason::Type EndPlayReason ) {
	if ( EAXVolumeBox && EAXVolumeBox->IsValidLowLevel() ) {
		EAXVolumeBox->OnComponentBeginOverlap.RemoveDynamic( this, &ARookEAXVolumeBox::OnOverlap );
		EAXVolumeBox->OnComponentEndOverlap.RemoveDynamic( this, &ARookEAXVolumeBox::EndOfOverlap );
	}
}

void ARookEAXVolumeBox::OnOverlap( UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult ) {
	if ( RookInterface.IsValid() )
		RookInterface->OnEAXOverlap.Broadcast( OtherActor->GetUniqueID(), VolumeEAX );
}

void ARookEAXVolumeBox::EndOfOverlap( UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex ) {
	if ( RookInterface.IsValid() )
		RookInterface->OnEAXOverlap.Broadcast( OtherActor->GetUniqueID(), EEAX::None );
}

ARookEAXVolumeBox::~ARookEAXVolumeBox() {

}