#include "RookPrivatePCH.h"
#include "Runtime/Engine/Classes/Sound/SoundWave.h"
#include "Runtime/Engine/Classes/Curves/CurveFloat.h"
#include "OpenALSoft.h"
#include "RookAudioController.h"
#include "RookMonoSource.h"
#include "RookListenerController.h"
#include "RookUtils.h"
#include "RookAudioDataLoader.h"
#include "RookAudioDataLoadingTask.h"

URookAudioController::URookAudioController() {
}

bool URookAudioController::IsTickable() const {
	return true;
}

TStatId URookAudioController::GetStatId() const {
	return this->GetStatID();
}

void URookAudioController::Tick( float DeltaTime ) {
	if ( AudioSources.Num() > 0 ) {
		for ( TPair< uint32, FAudioSourceModel >& Kvp : AudioSources ) {
			CheckIfSourceFinishedPlaying( Kvp.Value );
			TWeakObjectPtr<AActor> TemporaryParent;
			for ( TObjectIterator<AActor> Itr; Itr; ++Itr ) {
				if ( Itr->GetUniqueID() == Kvp.Value.ParentID ) {
					TemporaryParent = Cast<AActor>(*Itr);
					break;
				}
			}

			switch ( Kvp.Value.AudioState ) {
				case EAudioState::WasPlaying:
					AfterFinishedPlaying( TemporaryParent, Kvp.Value );
					break;
				case EAudioState::Playing:
					if ( TemporaryParent.IsValid() )
						UpdateLocation( TemporaryParent, Kvp.Value );
				break;
			}			
		}
	}	
}

void URookAudioController::Play( TWeakObjectPtr<class AActor> Parent ) {
	GetAudioMaxiumDistance();
	CheckIfBufferHasAudioData();
	if ( HasActiveListenerController() && HasAudioModelCurrentSurface() && RookUtils::Instance().InSpehereRadius( Parent->GetActorLocation(), ActiveListenerController->GetListenerLocation(), MaxiumDistanceToListener ) ) {
		if ( !ShouldUnPause( Parent->GetUniqueID() ) ) {
			SetUpNewMonoAudio( Parent );
		} else {
			Pause();
		}		
	}
}

void URookAudioController::Pause() {
	if ( AudioSources.Num() > 0 ) {
		for ( TPair< uint32, FAudioSourceModel >& Kvp : AudioSources ) {
			if ( Kvp.Value.AudioState == EAudioState::Playing ) {
				Kvp.Value.AudioState = EAudioState::Paused;
				OpenALSoft::Instance().Pause( Kvp.Key );
			} else if ( Kvp.Value.AudioState == EAudioState::Playing ) {
				Kvp.Value.AudioState = EAudioState::Playing;
				OpenALSoft::Instance().PlayAfterPause( Kvp.Key );
			}			
		}
	}
}

bool URookAudioController::ShouldUnPause( uint32 ParentID ) {
	if ( AudioSources.Num() > 0 ) {
		for ( TPair< uint32, FAudioSourceModel >& Kvp : AudioSources ) {
			if ( Kvp.Value.ParentID == ParentID && Kvp.Value.AudioState == EAudioState::Paused ) {
				return true;
			}
		}
	}
	return false;
}

void URookAudioController::Stop() {
	if ( AudioSources.Num() > 0 ) {
		for ( TPair< uint32, FAudioSourceModel >& Kvp : AudioSources ) {
			OpenALSoft::Instance().RemoveAudioSource( Kvp.Key );
			AudioSources.Remove( Kvp.Key );
			AudioSources.Compact();
		}
	}
}

void URookAudioController::ChangeSurface( TEnumAsByte<EPhysicalSurface> NewSurface ) {
	CurrentAudioControllerSurafce = NewSurface;
}

void URookAudioController::ChangeEAX( EEAX NewEAX ) {
	CurrentAudioControllerEAX = NewEAX;
	if ( AudioSources.Num() > 0 ) {
		for ( TPair< uint32, FAudioSourceModel >& Kvp : AudioSources ) {
			OpenALSoft::Instance().SetEAXOnAudioSource( NewEAX, Kvp.Key );
		}
	}
}

bool URookAudioController::HasActiveListenerController() {
	if ( !ActiveListenerController.IsValid() ) {
		GetActiveListenerController();
	} else {
		if( !ActiveListenerController->IsActive() )
			GetActiveListenerController();
	}
	return ActiveListenerController.IsValid();
}

void URookAudioController::GetActiveListenerController() {
	for ( TObjectIterator<URookListenerController> Itr; Itr; ++Itr ) {
		if ( Itr->IsActive() ) {
			ActiveListenerController = Cast<URookListenerController>( *Itr );
			break;
		}			
	}
}

void URookAudioController::SetUpNewMonoAudio( TWeakObjectPtr<AActor> Parent ) {
	uint32 TemporaryNewAudioSourceID = RookUtils::Instance().GetUniqueID();
	FVector ParentLocation = Parent->GetActorLocation();
		
	AudioSourceModel.AudioSourceID = TemporaryNewAudioSourceID;
	AudioSourceModel.ParentID = Parent->GetUniqueID();
	
	if ( AudioSourceModel.VolumeOverDistanceCurve.IsValid() ) {
		float DistanceToListener = ( ParentLocation - ActiveListenerController->GetListenerLocation() ).Size();
		AudioSourceModel.AudioSourceGain = AudioSourceModel.VolumeOverDistanceCurve->GetFloatValue( DistanceToListener );
	} else {
		AudioSourceModel.AudioSourceGain = 0.0f;
	}
	
	AudioSourceModel.AudioSourcePosition = ParentLocation;
		
	if ( AudioSourceModel.bUseRandomPitch ) {
		AudioSourceModel.AudioSourceRandomPitch = FMath::FRandRange( AudioSourceModel.BottomRandomPitchValue, AudioSourceModel.TopRandomPitchValue );
	}

	if ( AudioSourceModel.AudioSourceEAX != CurrentAudioControllerEAX )
		AudioSourceModel.AudioSourceEAX = CurrentAudioControllerEAX;

	AudioSources.Add( TemporaryNewAudioSourceID, AudioSourceModel );
	AudioSourceModel.MonoAudioSourceAsset = GetMonoAudioSource( TemporaryNewAudioSourceID );	
	OpenALSoft::Instance().Play( AudioSourceModel );
}

void URookAudioController::GetAudioMaxiumDistance() {
	if ( AudioSourceModel.VolumeOverDistanceCurve.IsValid() ) {
		MaxiumDistanceToListener = AudioSourceModel.VolumeOverDistanceCurve->FloatCurve.GetLastKey().Time;
	} else {
		//find some natural value
		MaxiumDistanceToListener = 0.0f;
	}
}

void URookAudioController::CheckIfBufferHasAudioData() {
	TArray<TWeakObjectPtr<USoundWave>> AudioToLoad;
	CheckDataLoader();
	for ( FMonoAudioModel MonoAudioModel : AudioSourceModel.MonoAssets ) {
		if ( !OpenALSoft::Instance().Buffers.Contains( MonoAudioModel.AudioAsset->GetUniqueID() ) ) {
			OpenALSoft::Instance().Buffers.Add( MonoAudioModel.AudioAsset->GetUniqueID(), 0 );
			AudioToLoad.Add( MonoAudioModel.AudioAsset );
		}
	}
	if ( AudioToLoad.Num() > 0 ) {
		DataLoader->LoadingGraph.Add( TGraphTask<RookAudioDataLoadingTask>::CreateTask( nullptr, ENamedThreads::AnyThread ).ConstructAndDispatchWhenReady( AudioToLoad ) );
		AudioToLoad.Empty();
	}
}

void URookAudioController::CheckDataLoader() {
	if ( DataLoader == nullptr ) {
		URookAudioDataLoader* TemporaryDataLoader = nullptr;
		for ( TObjectIterator<URookAudioDataLoader> Itr; Itr; ++Itr ) {
			TemporaryDataLoader = Cast<URookAudioDataLoader>( *Itr );
		}

		if ( TemporaryDataLoader == nullptr ) {
			DataLoader = NewObject<URookAudioDataLoader>();
		} else {
			DataLoader = TemporaryDataLoader;
		}
		TemporaryDataLoader = nullptr;
	}
}

TWeakObjectPtr<class USoundWave> URookAudioController::GetMonoAudioSource( uint32 AudioSourceID ) {
	if ( AudioSources.Contains( AudioSourceID ) ) {
		switch ( AudioSources[AudioSourceID].PlaybackOption ) {
			case EPlayback::Single:			
			case EPlayback::Loop:
				return AudioSources[AudioSourceID].MonoAssets[0].AudioAsset;
				break;
			case EPlayback::Random:
			case EPlayback::SingleRandom:
				return GetRandomMonoAudioSource( AudioSourceID );
				break;
			case EPlayback::Sequence:
			case EPlayback::SingleSequence:
				return GetSequenceMonoAudioSource( AudioSourceID );
				break;
		}		
	}
	return nullptr;
}

TWeakObjectPtr<class USoundWave> URookAudioController::GetRandomMonoAudioSource( uint32 AudioSourceID ) {
	if ( AudioSources[AudioSourceID].MonoAssets.Num() > 0 ) {
		if ( AudioSources[AudioSourceID].MonoAssets.Num() == 1 ) {
			LastRandomIndex = 0;
			return AudioSources[AudioSourceID].MonoAssets[0].AudioAsset;
		} else {
			uint16 TemporaryRandomIndex = FMath::RandRange( 0, AudioSources[AudioSourceID].MonoAssets.Num() );
			while ( TemporaryRandomIndex == LastRandomIndex ) {
				TemporaryRandomIndex = FMath::RandRange( 0, AudioSources[AudioSourceID].MonoAssets.Num() );
			}
			LastRandomIndex = TemporaryRandomIndex;
			return AudioSources[AudioSourceID].MonoAssets[LastRandomIndex].AudioAsset;
		}		
	}
	return nullptr;
}

TWeakObjectPtr<class USoundWave> URookAudioController::GetSequenceMonoAudioSource( uint32 AudioSourceID ) {
	if ( AudioSources[AudioSourceID].MonoAssets.Num() > 0 ) {
		++LastSequenceIndex;
		if ( LastSequenceIndex > AudioSources[AudioSourceID].MonoAssets.Num() ) {
			LastSequenceIndex = 0;
		}
		return AudioSources[AudioSourceID].MonoAssets[LastSequenceIndex].AudioAsset;
	}
	return nullptr;
}

bool URookAudioController::HasAudioModelCurrentSurface() {
	if ( AudioSourceModel.MonoAssets.Num() > 0 ) {
		for ( FMonoAudioModel MonoAudioModel : AudioSourceModel.MonoAssets ) {
			if ( MonoAudioModel.SurfaceAffectingAudio == CurrentAudioControllerSurafce )
				return true;
		}
	}	
	return false;
}

void URookAudioController::CheckIfSourceFinishedPlaying( FAudioSourceModel AudioSourceModel ) {
	if ( AudioSourceModel.AudioState == EAudioState::Playing ) {
		if ( OpenALSoft::Instance().CurrentPositionOnAudioTrack( AudioSourceModel.AudioSourceID ) >= AudioSourceModel.MonoAudioSourceAsset->Duration )
			AudioSourceModel.AudioState = EAudioState::WasPlaying;
	}	
}

void URookAudioController::AfterFinishedPlaying( TWeakObjectPtr<AActor> Parent, FAudioSourceModel AudioSourceModel ) {
	if ( Parent.IsValid() && ( AudioSourceModel.PlaybackOption == EPlayback::Random || AudioSourceModel.PlaybackOption == EPlayback::Sequence ) ) {
		Play( Parent );
	}
	OpenALSoft::Instance().RemoveAudioSource( AudioSourceModel.AudioSourceID );
	AudioSources.Remove( AudioSourceModel.AudioSourceID );
	AudioSources.Compact();	
}

void URookAudioController::UpdateLocation( TWeakObjectPtr<AActor> Parent, FAudioSourceModel AudioSourceModel ){
	if ( AudioSourceModel.AudioSourcePosition != Parent->GetActorLocation() ) {
		AudioSourceModel.AudioSourcePosition = Parent->GetActorLocation();
		OpenALSoft::Instance().UpdateAudioSourcePosition( AudioSourceModel.AudioSourceID, Parent->GetActorLocation() );

		if ( AudioSourceModel.bUseVelocity )
			OpenALSoft::Instance().UpdateAudioSourceVelocity( AudioSourceModel.AudioSourceID, Parent->GetVelocity() );
	
		if ( AudioSourceModel.bUseRaytrace ) {
			RaytraceToListener( Parent, AudioSourceModel.AudioSourceID );
		} else {
			float DistanceToListener = ( Parent->GetActorLocation() - ActiveListenerController->GetListenerLocation() ).Size();

			if ( AudioSourceModel.VolumeOverDistanceCurve.IsValid() ) {
				float AudioGain = AudioSourceModel.VolumeOverDistanceCurve->GetFloatValue( DistanceToListener );
				OpenALSoft::Instance().ChangeAudioSourceGain( AudioSourceModel.AudioSourceID, AudioGain );
				if ( bUseDebugSpheres ) {
#if WITH_EDITOR
					DrawDebugSphere(
						RookUtils::Instance().GetWorld(),
						Parent->GetActorLocation(),
						AudioGain,
						24,
						//blue
						FColor( 0, 25, 255 ),
						false,
						1.0f
						);
#endif
				}
			}

			if ( AudioSourceModel.LowpassOverDistanceCurve.IsValid() ) {
				float LowpassGain = AudioSourceModel.LowpassOverDistanceCurve->GetVectorValue( DistanceToListener ).X;
				float LowpassHFGain = AudioSourceModel.LowpassOverDistanceCurve->GetVectorValue( DistanceToListener ).Y;
				OpenALSoft::Instance().ChangeLowpassFilter( AudioSourceModel.AudioSourceID, LowpassGain, LowpassHFGain );
#if WITH_EDITOR
				DrawDebugSphere(
					RookUtils::Instance().GetWorld(),
					Parent->GetActorLocation(),
					LowpassGain,
					24,
					//darker yellow
					FColor( 255, 210, 0 ),
					false,
					1.0f
					);
				DrawDebugSphere(
					RookUtils::Instance().GetWorld(),
					Parent->GetActorLocation(),
					LowpassGain,
					24,
					//lighter yellow
					FColor( 255, 255, 0 ),
					false,
					1.0f
					);
#endif
			}

			if ( AudioSourceModel.BandpassOverDistanceCurve.IsValid() ) {
				float BandpassHFGain = AudioSourceModel.BandpassOverDistanceCurve->GetVectorValue( DistanceToListener ).X;
				float BandpassLFGain = AudioSourceModel.BandpassOverDistanceCurve->GetVectorValue( DistanceToListener ).Y;
				OpenALSoft::Instance().ChangeBandpassFilter( AudioSourceModel.AudioSourceID, BandpassHFGain, BandpassLFGain );
#if WITH_EDITOR
				DrawDebugSphere(
					RookUtils::Instance().GetWorld(),
					Parent->GetActorLocation(),
					BandpassHFGain,
					24,
					//darker green
					FColor( 0 , 255, 0 ),
					false,
					1.0f
					);
				DrawDebugSphere(
					RookUtils::Instance().GetWorld(),
					Parent->GetActorLocation(),
					BandpassLFGain,
					24,
					//lighter green
					FColor( 0, 255, 100 ),
					false,
					1.0f
					);
#endif
			}				
		}			
	}	
}

void URookAudioController::RaytraceToListener( TWeakObjectPtr<AActor> Parent, uint32 AudioSourceID ) {
	FVector StartPosition = Parent->GetActorLocation();
	FVector EndPosition = ActiveListenerController->GetListenerLocation();
	FHitResult Hit;
	UWorld* TemporaryWorld = RookUtils::Instance().GetWorld();
	AActor* PreRayedActor = nullptr;
	FCollisionQueryParams TraceParams( TEXT("RayTrace"), true, Parent.Get() );
	TraceParams.bReturnPhysicalMaterial = true;
	
	float STC = 0.0f;
	float SAC = 0.0f;

	TemporaryWorld->LineTraceSingleByChannel( Hit, StartPosition, EndPosition, ECC_Visibility, TraceParams );

	while ( Hit.GetActor() != nullptr && Hit.GetActor() != PreRayedActor && Hit.GetActor() != Parent ) {
		PreRayedActor = Hit.GetActor();
		TraceParams.AddIgnoredActor( PreRayedActor );
		TemporaryWorld->LineTraceSingleByChannel( Hit, Hit.Location, EndPosition, ECC_Visibility, TraceParams );
		if ( Hit.PhysMaterial != nullptr ) {
			if ( RookUtils::Instance().STC.Contains( Hit.PhysMaterial->GetName() ) ) {
				STC += RookUtils::Instance().STC[Hit.PhysMaterial->GetName()] / 1.3;
				SAC += RookUtils::Instance().SAC[Hit.PhysMaterial->GetName()];
			}
		}
	}
	
	float AudioAlterGain = 100.0f - ( STC - 25.0f ) * 2.857142f - SAC * 100 - Parent->GetVelocity().Size() / 200.0f;
	if ( AudioAlterGain < 0.0f ) {
		AudioAlterGain = 0.0f;
	} else if ( AudioAlterGain > 100.0f ) {
		AudioAlterGain = 100.0f;
	}

	float TargetFilterGain = 1.0f - STC / 100;
	if ( TargetFilterGain < 0.0f ) {
		TargetFilterGain = 0.0f;
	}

	OpenALSoft::Instance().ChangeAudioSourceGain( AudioSourceID, AudioAlterGain );
	OpenALSoft::Instance().ChangeLowpassFilter( AudioSourceID, TargetFilterGain, TargetFilterGain );

	PreRayedActor = nullptr;
	TemporaryWorld = nullptr;
}

URookAudioController::~URookAudioController() {
	if ( AudioSources.Num() > 0 )	{
		for ( TPair< uint32, FAudioSourceModel >& Kvp : AudioSources ) {
			OpenALSoft::Instance().RemoveAudioSource( Kvp.Key );
		}
		AudioSources.Empty();
	}	
	ActiveListenerController = nullptr;
	DataLoader = nullptr;
}