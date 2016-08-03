/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#include "RookPrivatePCH.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Camera/PlayerCameraManager.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMesh.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMeshSocket.h"
#include "OpenALSoft.h"
#include "RookUtils.h"
#include "RookListenerController.h"

URookListenerController::URookListenerController() {
	RegisterListener();
	CheckInitListenerType();
}

bool URookListenerController::IsTickable() const {
	return IsTicking;
}

TStatId URookListenerController::GetStatId() const {
	return this->GetStatID();
}

void URookListenerController::Tick( float DeltaTime ) {
	if (!bWasRegistered)
		RegisterListener();

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

void URookListenerController::SetListenerActiveState( const bool bIsActive ) {
	IsTicking = bIsActive;
}

bool URookListenerController::IsActive() const {
	return IsTicking;
}

void URookListenerController::ChangeListenerType( const EListenerType NewListenerType ) {
	EListenerType TemporaryListenerType = ListenerType;
	ListenerType = NewListenerType;
	if ( !IsFollowerValid() ) {
		UE_LOG( RookLog, Warning, TEXT("While setting new listener type actor to follow was NOT valid! Please set proper actor - to do that use SetActorOnListener, SetSocketOnListener or SetCameraOnListener. Listener type was not change.") );
		ListenerType = TemporaryListenerType;
		return;
	}
	if ( TemporaryListenerType == EListenerType::FollowMeshSocket && NewListenerType != EListenerType::FollowMeshSocket ) {
		SocketActor->ConditionalBeginDestroy();
		SocketActor = nullptr;
	}
}

FVector	URookListenerController::GetListenerLocation() {
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
		GetDefaultCamera();
	}
}

void URookListenerController::GetDefaultCamera() {
	UWorld* World = RookUtils::Instance().GetWorld();
#if WITH_EDITOR
	World = RookUtils::Instance().GetWorld( EWorldType::PIE );
#endif
	if  ( World ) {
			for ( TActorIterator<APlayerCameraManager> Itr( World ); Itr; ++Itr ) {
				CameraToFollow = Cast<APlayerCameraManager>( *Itr );
			}
		}
}

void URookListenerController::UpdateListenerLocation( const TWeakObjectPtr<class AActor> Actor ) {
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

	if ( Actor->GetActorLocation() == LastListenerLocation && TempListenerForwardVector == LastListenerForwardVector && TempListenerUpVector == LastListenerUpVector )
		return;
	
	LastListenerLocation = Actor->GetActorLocation();
	LastListenerForwardVector = TempListenerForwardVector;
	LastListenerUpVector = TempListenerUpVector;

	if ( !bHasEndPlayDeleagate ) {
		bHasEndPlayDeleagate = true;
		Actor->OnEndPlay.AddDynamic( this, &URookListenerController::OnEndPlay );
	}
	
	//UE_LOG(RookLog, Log, TEXT("x: %f, y:%f, z:%f "), LastListenerLocation.X, LastListenerLocation.Y, LastListenerLocation.Z);
	
	OpenALSoft::Instance().UpdateAudioListenerePosition( LastListenerLocation, LastListenerForwardVector, LastListenerUpVector );

	if ( bUseVelocity ) {
		OpenALSoft::Instance().UpdateAudioListenerVelocity( Actor->GetVelocity() );
	}
}

bool URookListenerController::IsFollowerValid() {
	switch ( ListenerType ) {
		case EListenerType::FollowActor:
			return ActorToFollow.IsValid();			
		case EListenerType::FollowMeshSocket:			
			return SocketAttachment.MeshSocketToFollow.IsValid();			
		case EListenerType::FollowCamera:
			if ( CameraToFollow.IsValid() ) {
				return true;
			} else {
				GetDefaultCamera();
			}
			return false;
	}
	return false;
}

void URookListenerController::ValidatedSocketActor() {	
	UWorld* World = RookUtils::Instance().GetWorld();
#if WITH_EDITOR
	World = RookUtils::Instance().GetWorld( EWorldType::PIE );
#endif

	if ( !SocketActor.IsValid() && World) {
		SocketActor = NewObject<AActor>( World );
		SocketAttachment.MeshSocketToFollow->AttachActor( SocketActor.Get(), SocketAttachment.CharacterWithSocket->GetMesh() );
	}
}

void URookListenerController::RegisterListener() {
	if ( RookInterface.IsValid() ) {
		if ( !RookInterface->Listeners.Contains( this ) ) {
			RookInterface->Listeners.Add( this );
			bWasRegistered = true;
		}
	} else {
		TSharedPtr<IModuleInterface> RookModule = FModuleManager::Get().GetModule("Rook");

		if ( RookModule.IsValid() ) {
			RookInterface = StaticCastSharedPtr<IRook>( RookModule );
			RookInterface->Listeners.Add( this );
			bWasRegistered = true;
		}
	}
}

void URookListenerController::OnEndPlay( AActor* Actor, EEndPlayReason::Type EndPlayReason ) {
	bHasEndPlayDeleagate = false;
	if ( Actor ) 
		Actor->OnEndPlay.RemoveDynamic( this, &URookListenerController::OnEndPlay );

	if ( RookInterface.IsValid() )
		RookInterface->OnEndPlay.Broadcast();
}

URookListenerController::~URookListenerController() {
	if ( ActorToFollow.IsValid() ) {
		ActorToFollow->OnEndPlay.RemoveDynamic( this, &URookListenerController::OnEndPlay );
	}
	ActorToFollow = nullptr;

	if ( CameraToFollow.IsValid() ) {
		CameraToFollow->OnEndPlay.RemoveDynamic( this, &URookListenerController::OnEndPlay );
	}
	CameraToFollow = nullptr;

	SocketAttachment.CharacterWithSocket = nullptr;
	SocketAttachment.MeshSocketToFollow = nullptr;
	if ( SocketActor.IsValid() ) {
		SocketActor->OnEndPlay.RemoveDynamic( this, &URookListenerController::OnEndPlay );		
		SocketActor->ConditionalBeginDestroy();
	}
	
	if (RookInterface.IsValid() ) {		
		RookInterface->Listeners.Remove( this );
	}
}