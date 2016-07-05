#include "RookPrivatePCH.h"
#include "RookListenerControllerBPL.h"
#include "RookListenerController.h"

bool URookListenerControllerBPL::SetActiveListener( TSubclassOf<URookListenerController> ListenerBlueprint ) {
	if ( !ListenerBlueprint )
		return false;

	for ( TObjectIterator<URookListenerController> Itr; Itr; ++Itr ) {
		Itr->SetListenerActiveState( false );
	}
	TWeakObjectPtr<URookListenerController> TemporaryListenerController = Cast<URookListenerController>( ListenerBlueprint->ClassDefaultObject );
	TemporaryListenerController->SetListenerActiveState( true );
	TemporaryListenerController = nullptr;
	return true;
}

bool URookListenerControllerBPL::SetListenerType( TSubclassOf<URookListenerController> ListenerBlueprint, EListenerType NewListenerType ) {
	if ( !ListenerBlueprint )
		return false;

	TWeakObjectPtr<URookListenerController> TemporaryListenerController = Cast<URookListenerController>(ListenerBlueprint->ClassDefaultObject);
	TemporaryListenerController->ChangeListenerType( NewListenerType );
	TemporaryListenerController = nullptr;
	return true;
}

bool URookListenerControllerBPL::SetActorOnListener( TSubclassOf<URookListenerController> ListenerBlueprint, AActor* NewActor ) {
	if ( !ListenerBlueprint || !NewActor )
		return false;

	TWeakObjectPtr<URookListenerController> TemporaryListenerController = Cast<URookListenerController>( ListenerBlueprint->ClassDefaultObject );
	TemporaryListenerController->ActorToFollow = NewActor;
	TemporaryListenerController = nullptr;
	return true;
}

bool URookListenerControllerBPL::SetSocketOnListener( TSubclassOf<URookListenerController> ListenerBlueprint, FSocketAttachment NewSocketAttachment ) {
	if ( !ListenerBlueprint )
		return false;

	TWeakObjectPtr<URookListenerController> TemporaryListenerController = Cast<URookListenerController>( ListenerBlueprint->ClassDefaultObject );
	TemporaryListenerController->SocketAttachment = NewSocketAttachment;
	TemporaryListenerController = nullptr;
	return true;
}

bool URookListenerControllerBPL::SetCameraOnListener( TSubclassOf<URookListenerController> ListenerBlueprint, APlayerCameraManager* NewCamera ) {
	if ( !ListenerBlueprint || !NewCamera )
		return false;

	TWeakObjectPtr<URookListenerController> TemporaryListenerController = Cast<URookListenerController>( ListenerBlueprint->ClassDefaultObject );
	TemporaryListenerController->CameraToFollow = NewCamera;
	TemporaryListenerController = nullptr;
	return true;
}

bool URookListenerControllerBPL::SetFreezRotationOnListener(TSubclassOf<URookListenerController> ListenerBlueprint, bool bFreezRotation, FFreezRotation NewFreezRotation) {
	if ( !ListenerBlueprint )
		return false;

	TWeakObjectPtr<URookListenerController> TemporaryListenerController = Cast<URookListenerController>( ListenerBlueprint->ClassDefaultObject );
	TemporaryListenerController->bFreezRotation = bFreezRotation;
	if ( bFreezRotation )
		TemporaryListenerController->FreezRotation = NewFreezRotation;
	TemporaryListenerController = nullptr;
	return true;
}

bool URookListenerControllerBPL::ShouldListenerUseVelocity( TSubclassOf<URookListenerController> ListenerBlueprint, bool bShouldUseVelocity ) {
	if ( !ListenerBlueprint )
		return false;

	TWeakObjectPtr<URookListenerController> TemporaryListenerController = Cast<URookListenerController>( ListenerBlueprint->ClassDefaultObject );
	TemporaryListenerController->bUseVelocity = bShouldUseVelocity;
	TemporaryListenerController = nullptr;
	return true;
}