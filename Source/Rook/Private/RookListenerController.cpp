#include "RookPrivatePCH.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Camera/PlayerCameraManager.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMesh.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMeshSocket.h"
#include "OpenALSoft.h"
#include "RookUtils.h"
#include "RookListenerController.h"

URookListenerController::URookListenerController() {
	CheckInitListenerType();
}

bool URookListenerController::IsTickable() const {
	return IsTicking;
}

TStatId URookListenerController::GetStatId() const {
	return this->GetStatID();
}

void URookListenerController::Tick( float DeltaTime ) {
	if ( !IsFollowerValid() ) {
		return;
	} else {
		switch ( ListenerType ) {
			case EListenerType::FollowActor:
				UpdateListenerLocation( ActorToFollow );
				break;
			case EListenerType::FollowMeshSocket:
				ValidatedSocketActor();
				UpdateListenerLocation( SocketActor );
				break;
			case EListenerType::FollowCamera:
				UpdateListenerLocation( CameraToFollow );
				break;
		}		
	}		 
}

void URookListenerController::SetListenerActiveState( bool bIsActive ) {
	IsTicking = bActive;
	bActive = bIsActive;
}

bool URookListenerController::IsActive() const {
	return bActive;
}

void URookListenerController::ChangeListenerType( EListenerType NewListenerType ) {
	ListenerType = NewListenerType;
}

FVector	URookListenerController::GetListenerLocation() const {
	if ( !IsFollowerValid() ) {
		return FVector( 0 );
	} else {
		switch ( ListenerType ) {
		case EListenerType::FollowActor:
			return ActorToFollow->GetActorLocation();
			break;
		case EListenerType::FollowMeshSocket:
			return SocketActor->GetActorLocation();
			break;
		case EListenerType::FollowCamera:
			return CameraToFollow->GetCameraLocation();
			break;
		}
	}
	return FVector( 0 );
}

void URookListenerController::CheckInitListenerType() {
	if ( ActorToFollow.IsValid() ) {
		ListenerType = EListenerType::FollowActor;
	}

	if ( SocketAttachment.MeshSocketToFollow.IsValid() && SocketAttachment.CharacterWithSocket.IsValid() ) {
		ListenerType = EListenerType::FollowMeshSocket;
	}

	if ( ListenerType == EListenerType::FollowCamera && !CameraToFollow.IsValid() ) {
		if  ( RookUtils::Instance().GetWorld() ) {
			for ( TActorIterator<APlayerCameraManager> Itr( RookUtils::Instance().GetWorld() ); Itr; ++Itr ) {
				CameraToFollow = Cast<APlayerCameraManager>( *Itr );
			}
		}		
	}
}

void URookListenerController::UpdateListenerLocation( TWeakObjectPtr<class AActor> Actor ) {
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
	if ( !SocketActor.IsValid() && RookUtils::Instance().GetWorld() ) {
		SocketActor = NewObject<AActor>( RookUtils::Instance().GetWorld() );
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