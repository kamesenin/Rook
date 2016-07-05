#include "RookPrivatePCH.h"
#include "OpenALSoft.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Camera/PlayerCameraManager.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMesh.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMeshSocket.h"
#include "RookListenerController.h"

URookListenerController::URookListenerController() {
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
	SetActive( false, false );
}

void URookListenerController::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) {
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
	
	if ( !IsFollowerValid() ) {
		return;
	} else {
		switch ( ListenerType ) {
			case EListenerType::FollowActor:
				UpdateListenerPosition( ActorToFollow );
				break;
			case EListenerType::FollowMeshSocket:
				ValidatedSocketActor();
				UpdateListenerPosition( SocketActor );
				break;
			case EListenerType::FollowCamera:
				UpdateListenerPosition( CameraToFollow );
				break;
		}		
	}		 
}

void URookListenerController::SetListenerActiveState( bool bActive ) {
	SetActive( bActive, false );
	SetComponentTickEnabled( bActive );	
}

void URookListenerController::ChangeListenerType( EListenerType NewListenerType ) {
	ListenerType = NewListenerType;
}

void URookListenerController::InitializeComponent() {
	Super::InitializeComponent();

	if ( ActorToFollow.IsValid() ) {
		ListenerType = EListenerType::FollowActor;
	}

	if ( SocketAttachment.MeshSocketToFollow.IsValid() && SocketAttachment.CharacterWithSocket.IsValid() ) {
		ListenerType = EListenerType::FollowMeshSocket;
	}

	if ( ListenerType == EListenerType::FollowCamera && !CameraToFollow.IsValid()) {
		for ( TActorIterator<APlayerCameraManager> Itr( GetWorld() ); Itr; ++Itr ) {
			CameraToFollow = Cast<APlayerCameraManager>( *Itr );
		}
	}
}

void URookListenerController::UninitializeComponent() {
	Super::UninitializeComponent();
}

void URookListenerController::UpdateListenerPosition( TWeakObjectPtr<class AActor> Actor ) {
	FVector TempListenerForwardVector = Actor->GetActorForwardVector();
	FVector TempListenerUpVector = Actor->GetActorUpVector();
	FRotator TempRotator = Actor->GetActorRotation();
	
	if ( bFreezRotation ) {
		if ( FreezRotation.FreezXAxies == EFreezRotationState::Freez ) {
			TempRotator.Roll = FreezRotation.XAxiesRotation;
		}

		if ( FreezRotation.FreezYAxies == EFreezRotationState::Freez ) {
			TempRotator.Pitch = FreezRotation.YAxiesRotation;
		}

		if ( FreezRotation.FreezZAxies == EFreezRotationState::Freez ) {
			TempRotator.Yaw = FreezRotation.ZAxiesRotation;
		}

		TempListenerForwardVector = FRotationMatrix( TempRotator ).GetScaledAxis( EAxis::X );
		TempListenerUpVector = FRotationMatrix( TempRotator ).GetScaledAxis( EAxis::Z );
	}
	OpenALSoft::Instance().UpdateAudioListenerePosition( Actor->GetActorLocation(), TempListenerForwardVector, TempListenerUpVector );

	if ( bUseVelocity ) {
		OpenALSoft::Instance().UpdateAudioListenerVelocity( Actor->GetVelocity() );
	}
}

bool URookListenerController::IsFollowerValid() const {
	switch ( ListenerType ) {
		case EListenerType::FollowActor:
			return ActorToFollow.IsValid();
			break;
		case EListenerType::FollowMeshSocket:			
			return SocketAttachment.MeshSocketToFollow.IsValid();
			break;
		case EListenerType::FollowCamera:
			return CameraToFollow.IsValid();
			break;
	}
	return false;
}

void URookListenerController::ValidatedSocketActor() {	
	if ( !SocketActor.IsValid() ) {	
		SocketActor = NewObject<AActor>( GetWorld() );		
		SocketAttachment.MeshSocketToFollow->AttachActor( SocketActor.Get(), SocketAttachment.CharacterWithSocket->GetMesh() );
	}
}

URookListenerController::~URookListenerController() {
	ActorToFollow = nullptr;
	CameraToFollow = nullptr;
	SocketAttachment.CharacterWithSocket = nullptr;
	SocketAttachment.MeshSocketToFollow = nullptr;
	if ( SocketActor.IsValid() ) {
		SocketActor->ConditionalBeginDestroy();
	}
}