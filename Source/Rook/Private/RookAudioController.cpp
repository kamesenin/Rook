/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#include "RookPrivatePCH.h"
#include "Runtime/Engine/Classes/Sound/SoundWave.h"
#include "Runtime/Engine/Classes/Curves/CurveFloat.h"
#include "OpenALSoft.h"
#include "RookAudioController.h"
#include "RookListenerController.h"
#include "RookUtils.h"
#include "RookAudioDataLoader.h"
#include "RookAudioDataLoadingTask.h"

URookAudioController::URookAudioController() {
	ApplicationVolumeMultiplier = FApp::GetVolumeMultiplier();
	OpenALSoft::Instance().SetVolumeMulitiplier( ApplicationVolumeMultiplier );

	TSharedPtr<IModuleInterface> RookModule = FModuleManager::Get().GetModule( "Rook" );
	if ( RookModule.IsValid() ) {
		 RookInterface = StaticCastSharedPtr<IRook>( RookModule );	
		 EndPlayHnadle = RookInterface->OnEndPlay.AddUObject( this, &URookAudioController::OnEndPlay );
	}
}

bool URookAudioController::IsTickable() const {
	if ( RookInterface.IsValid() )
		return RookInterface->bIsRookEnabled;
	return true;
}

TStatId URookAudioController::GetStatId() const {
	return this->GetStatID();
}

void URookAudioController::Tick( float DeltaTime ) {
	if ( AudioSourceModel.AudioType == EAudioType::is3D ) {
		if ( FailedToPlayAudioModel.Num() > 0 ) {
			bool bHasFail = false;
			for ( TPair< uint32, bool >& Kvp : FailedToPlayAudioModel ) {
				if ( Kvp.Value ) {
					b3DFailedToPlay = !OpenALSoft::Instance().Play( AudioSources[Kvp.Key] );
					FailedToPlayAudioModel[Kvp.Key] = b3DFailedToPlay;
					
					if( !b3DFailedToPlay )
						AudioSources[Kvp.Key].AudioState = EAudioState::Playing;

					if ( b3DFailedToPlay && !bHasFail ) {
						bHasFail = true;
					}						
				}				
			}
			if ( !bHasFail )
				FailedToPlayAudioModel.Empty();
		}			

		if ( AudioSources.Num() > 0 ) {
			for ( TPair< uint32, FAudioSourceModel >& Kvp : AudioSources ) {
				if ( Kvp.Value.AudioType == EAudioType::is3D ) {
					CheckIfSourceFinishedPlaying( Kvp.Key );
				}

				TWeakObjectPtr<AActor> TemporaryParent;
				for ( TObjectIterator<AActor> Itr; Itr; ++Itr ) {
					if ( Itr->GetUniqueID() == Kvp.Value.ParentID ) {
						TemporaryParent = Cast<AActor>(*Itr);
						break;
					}
				}

				switch ( Kvp.Value.AudioState ) {
					case EAudioState::WasPlaying:
						if ( TemporaryParent.IsValid() )
							AfterFinishedPlaying( TemporaryParent, Kvp.Key );
						break;
					case EAudioState::Playing:
						if ( TemporaryParent.IsValid() && Kvp.Value.AudioType == EAudioType::is3D )
							UpdateLocation( TemporaryParent, Kvp.Key );
					break;
				}			
			}
		}	
	}
}

void URookAudioController::Play( const TWeakObjectPtr<class AActor> Parent ) {
	if ( !RookInterface->bIsRookEnabled )
		return;
	CheckAudioType();
	if ( AudioSourceModel.AudioType == EAudioType::is3D ) {
		if ( HasPlayLimitReached() )
			return;
		GetAudioMaxiumDistance();
		CheckIfBufferHasAudioData();
		TemporaryAviableAudioSources.Empty();
		if ( HasActiveListenerController() && HasAudioModelCurrentSurface() && RookUtils::Instance().InSpehereRadius( Parent->GetActorLocation(), ActiveListenerController->GetListenerLocation(), MaxiumDistanceToListener ) ) {
			if ( !ShouldUnPause( Parent->GetUniqueID() ) ) {
				SetUpNewMonoAudio( Parent );
			} else {
				Pause();
			}		
		}
	} else {
		SetUpMultichannelSource( Parent );
	}
}

void URookAudioController::Pause() {
	if ( AudioSourceModel.AudioType == EAudioType::is3D ) {
		if ( AudioSources.Num() > 0 ) {
			for ( TPair< uint32, FAudioSourceModel >& Kvp : AudioSources ) {
				if( Kvp.Value.AudioType == EAudioType::is3D ) {
					if ( Kvp.Value.AudioState == EAudioState::Playing ) {
						Kvp.Value.AudioState = EAudioState::Paused;
						OpenALSoft::Instance().Pause( Kvp.Key );
					}
					else if ( Kvp.Value.AudioState == EAudioState::Playing ) {
						Kvp.Value.AudioState = EAudioState::Playing;
						OpenALSoft::Instance().PlayAfterPause( Kvp.Key );
					}
				}
			}
		}	
	} else {
		//Yeah, I know, however Unreal Audio Component does not have Pause
		for ( TWeakObjectPtr<UAudioComponent> MultichannelComponent : MultichannelComponents ) {
			if( MultichannelComponent.IsValid() )
				MultichannelComponent->Stop();
		}
	}
}

bool URookAudioController::ShouldUnPause( const uint32 ParentID ) {
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
	if ( AudioSourceModel.AudioType == EAudioType::is3D ) {
		if ( AudioSources.Num() > 0 ) {
			for ( TPair< uint32, FAudioSourceModel >& Kvp : AudioSources ) {
				if( Kvp.Value.AudioType == EAudioType::is3D ) {
					OpenALSoft::Instance().RemoveAudioSource( Kvp.Key );
				}				
			}
			AudioSources.Empty();
			AudioSources.Compact();
			FailedToPlayAudioModel.Empty();
			FailedToPlayAudioModel.Compact();

		}	
	} else {
		for ( TWeakObjectPtr<UAudioComponent> MultichannelComponent : MultichannelComponents ) {
			if( MultichannelComponent.IsValid() )
				MultichannelComponent->Stop();
		}
	}
	RandomIndicis.Empty();
	SequenceIndicis.Empty();
}

void URookAudioController::ChangeSurface( const TEnumAsByte<EPhysicalSurface> NewSurface ) {
	if ( NewSurface != CurrentAudioControllerSurafce ) {
		CurrentAudioControllerSurafce = NewSurface;
		RandomIndicis.Empty();
		SequenceIndicis.Empty();
	}	
}

void URookAudioController::ChangeEAX( const EEAX NewEAX ) {
	CurrentAudioControllerEAX = NewEAX;
	
	if ( AudioSourceModel.AudioType == EAudioType::is3D ) {
		if ( AudioSources.Num() > 0 ) {
			for ( TPair< uint32, FAudioSourceModel >& Kvp : AudioSources ) {
				if ( Kvp.Value.AudioType == EAudioType::is3D ) {
					OpenALSoft::Instance().SetEAXOnAudioSource( NewEAX, Kvp.Key );
				}
			}
		}
	} else {
		RookUtils::Instance().SetReverbInUnreal( CurrentAudioControllerEAX );
	}
}

void URookAudioController::ChangeDecibelsOnActiveMultichannel( const float Decibels ) {
	for ( TWeakObjectPtr<UAudioComponent> MultichannelAudio : MultichannelComponents ) {
		if ( MultichannelAudio.IsValid() ) {
			const float TemporaryVolume = RookUtils::Instance().DecibelsToVolume( Decibels ) * ApplicationVolumeMultiplier;
			MultichannelAudio->SetVolumeMultiplier( TemporaryVolume );
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
	for( TWeakObjectPtr<URookListenerController> ListenerController : RookInterface->Listeners ) {
		if ( ListenerController->IsActive() ) {
				ActiveListenerController = ListenerController;
				break;
		}			
	}
}

void URookAudioController::SetUpNewMonoAudio( const TWeakObjectPtr<AActor> Parent ) {
	const uint32 TemporaryNewAudioSourceID = RookUtils::Instance().GetUniqueID();
	const FVector ParentLocation = Parent->GetActorLocation();
	AudioSources.Add( TemporaryNewAudioSourceID, AudioSourceModel );
	AudioSources[TemporaryNewAudioSourceID].MonoAudioSourceAsset = GetMonoAudioSource( TemporaryNewAudioSourceID );

	if (AudioSources[TemporaryNewAudioSourceID].MonoAudioSourceAsset == nullptr ) {
		UE_LOG( RookLog, Warning, TEXT("While trying to Play, mono audio source is NULL! Will not play.") );
		AudioSources.Remove( TemporaryNewAudioSourceID );
		TemporaryAviableAudioSources.Empty();
		return;
	}	

	AudioSources[TemporaryNewAudioSourceID].AudioSourceID = TemporaryNewAudioSourceID;
	AudioSources[TemporaryNewAudioSourceID].ParentID = Parent->GetUniqueID();
	
	if ( AudioSources[TemporaryNewAudioSourceID].VolumeOverDistanceCurve ) {
		const float DistanceToListener = ( ParentLocation - ActiveListenerController->GetListenerLocation() ).Size();
		AudioSources[TemporaryNewAudioSourceID].AudioSourceGain = AudioSources[TemporaryNewAudioSourceID].VolumeOverDistanceCurve->GetFloatValue( DistanceToListener ) * ApplicationVolumeMultiplier;
	} else {
		AudioSources[TemporaryNewAudioSourceID].AudioSourceGain = 0.0f;
	}
	
	AudioSources[TemporaryNewAudioSourceID].AudioSourcePosition = ParentLocation;
		
	if ( AudioSources[TemporaryNewAudioSourceID].bUseRandomPitch ) {
		AudioSources[TemporaryNewAudioSourceID].AudioSourceRandomPitch = FMath::FRandRange( AudioSources[TemporaryNewAudioSourceID].BottomRandomPitchValue, AudioSources[TemporaryNewAudioSourceID].TopRandomPitchValue );
	}

	if ( AudioSources[TemporaryNewAudioSourceID].AudioSourceEAX != CurrentAudioControllerEAX )
		AudioSources[TemporaryNewAudioSourceID].AudioSourceEAX = CurrentAudioControllerEAX;
		
	TemporaryAviableAudioSources.Empty();
	b3DFailedToPlay = !OpenALSoft::Instance().Play( AudioSources[TemporaryNewAudioSourceID] );
	if ( b3DFailedToPlay ) {
		FailedToPlayAudioModel.Add( TemporaryNewAudioSourceID, b3DFailedToPlay );
	} else {
		AudioSources[TemporaryNewAudioSourceID].AudioState = EAudioState::Playing;
	}		
}

void URookAudioController::GetAudioMaxiumDistance() {
	if ( AudioSourceModel.VolumeOverDistanceCurve ) {
		MaxiumDistanceToListener = AudioSourceModel.VolumeOverDistanceCurve->FloatCurve.GetLastKey().Time;
	} else {
		//find some natural value
		MaxiumDistanceToListener = 0.0f;
	}
}

void URookAudioController::CheckIfBufferHasAudioData() {
	CheckDataLoader();
	for ( FMonoAudioModel MonoAudioModel : AudioSourceModel.MonoAssets ) {
		TArray<TWeakObjectPtr<USoundWave>> AudioToLoad;
		if ( !OpenALSoft::Instance().Buffers.Contains( MonoAudioModel.AudioAsset->GetUniqueID() ) ) {
			OpenALSoft::Instance().Buffers.Add( MonoAudioModel.AudioAsset->GetUniqueID(), 0 );
			AudioToLoad.Add( MonoAudioModel.AudioAsset );
		}
		if ( AudioToLoad.Num() > 0 ) {
			DataLoader->LoadingGraph.Add( TGraphTask<RookAudioDataLoadingTask>::CreateTask( nullptr, ENamedThreads::AnyThread ).ConstructAndDispatchWhenReady( AudioToLoad ) );
			AudioToLoad.Empty();
		}
	}	
}

void URookAudioController::CheckDataLoader() {
	if ( DataLoader == nullptr || !DataLoader.IsValid() ) {
		TWeakObjectPtr<URookAudioDataLoader> TemporaryDataLoader = nullptr;
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

TWeakObjectPtr<class USoundWave> URookAudioController::GetMonoAudioSource( const uint32 AudioModelID ) {
	if ( AudioSources.Contains( AudioModelID ) ) {
		switch ( AudioSources[AudioModelID].PlaybackOption ) {
			case EPlayback::Single:			
			case EPlayback::Loop:
				return TemporaryAviableAudioSources[0];
				break;
			case EPlayback::Random:
			case EPlayback::SingleRandom:
				return GetRandomAudioSource( AudioSources[AudioModelID].ParentID );
				break;
			case EPlayback::Sequence:
			case EPlayback::SingleSequence:
				return GetSequenceAudioSource( AudioSources[AudioModelID].ParentID );
				break;
		}		
	}
	return nullptr;
}

TWeakObjectPtr<class USoundWave> URookAudioController::GetRandomAudioSource( const uint32 ParentID ) {
	if ( !RandomIndicis.Contains( ParentID ) )
		RandomIndicis.Add( ParentID, 0 );
	
	if ( TemporaryAviableAudioSources.Num() == 1 ) {
		RandomIndicis[ParentID] = 0;
		return TemporaryAviableAudioSources[0];
	} else {
		uint16 TemporaryRandomIndex = FMath::RandRange( 0, TemporaryAviableAudioSources.Num() -1 );
		while ( TemporaryRandomIndex == RandomIndicis[ParentID] ) {
			TemporaryRandomIndex = FMath::RandRange( 0, TemporaryAviableAudioSources.Num() -1 );
		}
		RandomIndicis[ParentID] = TemporaryRandomIndex;
		return TemporaryAviableAudioSources[RandomIndicis[ParentID]];
	}	
	return nullptr;
}

TWeakObjectPtr<class USoundWave> URookAudioController::GetSequenceAudioSource( const uint32 ParentID ) {
	if ( !SequenceIndicis.Contains( ParentID ) )
		SequenceIndicis.Add( ParentID, 0 );
	
	if ( TemporaryAviableAudioSources.Num() > 0 ) {		
		TWeakObjectPtr<class USoundWave> TemporarySequenceSource = TemporaryAviableAudioSources[SequenceIndicis[ParentID]];
		++SequenceIndicis[ParentID];
		if ( SequenceIndicis[ParentID] > TemporaryAviableAudioSources.Num() - 1 )
			SequenceIndicis[ParentID] = 0;
		return TemporarySequenceSource;
	}
	return nullptr;
}

bool URookAudioController::HasAudioModelCurrentSurface() {
	if ( TemporaryAviableAudioSources.Num() > 0 )
		return true;

	if ( AudioSourceModel.MonoAssets.Num() > 0 ) {
		for ( FMonoAudioModel MonoAudioModel : AudioSourceModel.MonoAssets ) {
			if ( MonoAudioModel.SurfaceAffectingAudio == CurrentAudioControllerSurafce && MonoAudioModel.AudioAsset ) {
				TemporaryAviableAudioSources.AddUnique( MonoAudioModel.AudioAsset );
			}
				
		}	
		return TemporaryAviableAudioSources.Num() != 0;
	}	
	return false;
}

void URookAudioController::CheckIfSourceFinishedPlaying( const uint32 AudioModelID ) {
	if ( AudioSources[AudioModelID].AudioState == EAudioState::Playing ) {
		if ( OpenALSoft::Instance().AudioSourceState( AudioSources[AudioModelID].AudioSourceID ) == EAudioState::Stopped )
			AudioSources[AudioModelID].AudioState = EAudioState::WasPlaying;
	}	
}

void URookAudioController::AfterFinishedPlaying( const TWeakObjectPtr<AActor> Parent, const uint32 AudioModelID ) {
	if ( Parent.IsValid() ) {
		switch (AudioSources[AudioModelID].PlaybackOption ) {
		case EPlayback::Loop:
		case EPlayback::Random:
		case EPlayback::Sequence:
			Play( Parent );
			break;
		}
	}	

	if ( AudioSources[AudioModelID].AudioType == EAudioType::is3D )
		OpenALSoft::Instance().RemoveAudioSource( AudioSources[AudioModelID].AudioSourceID );
	
	AudioSources.Remove( AudioModelID );
	AudioSources.Compact();	
}

void URookAudioController::UpdateLocation( const TWeakObjectPtr<AActor> Parent, const uint32  AudioModelID){
	if ( AudioSources[AudioModelID].AudioSourcePosition != Parent->GetActorLocation() ) {
		AudioSources[AudioModelID].AudioSourcePosition = Parent->GetActorLocation();
		OpenALSoft::Instance().UpdateAudioSourcePosition( AudioSources[AudioModelID].AudioSourceID, Parent->GetActorLocation() );

		if ( AudioSources[AudioModelID].bUseVelocity )
			OpenALSoft::Instance().UpdateAudioSourceVelocity( AudioSources[AudioModelID].AudioSourceID, Parent->GetVelocity() );
	
		if ( AudioSources[AudioModelID].bUseRaytrace ) {
			RaytraceToListener( Parent, AudioSources[AudioModelID].AudioSourceID );
		} else {
			const float DistanceToListener = ( Parent->GetActorLocation() - ActiveListenerController->GetListenerLocation() ).Size();

			if ( AudioSources[AudioModelID].VolumeOverDistanceCurve ) {
				const float AudioGain = AudioSources[AudioModelID].VolumeOverDistanceCurve->GetFloatValue( DistanceToListener ) * ApplicationVolumeMultiplier;
				OpenALSoft::Instance().ChangeAudioSourceGain( AudioSources[AudioModelID].AudioSourceID, AudioGain );
				if ( bUseDebugSpheres ) {
#if WITH_EDITOR
					DrawDebugSphere(
						RookUtils::Instance().GetWorld( EWorldType::PIE ),
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

			if ( AudioSources[AudioModelID].LowpassOverDistanceCurve ) {
				const float LowpassGain = AudioSources[AudioModelID].LowpassOverDistanceCurve->GetVectorValue( DistanceToListener ).X;
				const float LowpassHFGain = AudioSources[AudioModelID].LowpassOverDistanceCurve->GetVectorValue( DistanceToListener ).Y;
				OpenALSoft::Instance().ChangeLowpassFilter( AudioSources[AudioModelID].AudioSourceID, LowpassGain, LowpassHFGain );
#if WITH_EDITOR
				DrawDebugSphere(
					RookUtils::Instance().GetWorld( EWorldType::PIE ),
					Parent->GetActorLocation(),
					LowpassGain,
					24,
					//darker yellow
					FColor( 255, 210, 0 ),
					false,
					1.0f
					);
				DrawDebugSphere(
					RookUtils::Instance().GetWorld( EWorldType::PIE ),
					Parent->GetActorLocation(),
					LowpassHFGain,
					24,
					//lighter yellow
					FColor( 255, 255, 0 ),
					false,
					1.0f
					);
#endif
			}

			if ( AudioSources[AudioModelID].BandpassOverDistanceCurve ) {
				const float BandpassHFGain = AudioSources[AudioModelID].BandpassOverDistanceCurve->GetVectorValue( DistanceToListener ).X;
				const float BandpassLFGain = AudioSources[AudioModelID].BandpassOverDistanceCurve->GetVectorValue( DistanceToListener ).Y;
				OpenALSoft::Instance().ChangeBandpassFilter( AudioSources[AudioModelID].AudioSourceID, BandpassHFGain, BandpassLFGain );
#if WITH_EDITOR
				DrawDebugSphere(
					RookUtils::Instance().GetWorld( EWorldType::PIE ),
					Parent->GetActorLocation(),
					BandpassHFGain,
					24,
					//darker green
					FColor( 0 , 255, 0 ),
					false,
					1.0f
					);
				DrawDebugSphere(
					RookUtils::Instance().GetWorld( EWorldType::PIE ),
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

void URookAudioController::RaytraceToListener( const TWeakObjectPtr<AActor> Parent, const uint32 AudioSourceID ) {
	FVector StartPosition = Parent->GetActorLocation();
	FVector EndPosition = ActiveListenerController->GetListenerLocation();
	FHitResult Hit;
	UWorld* TemporaryWorld = RookUtils::Instance().GetWorld();
#if WITH_EDITOR
	TemporaryWorld = RookUtils::Instance().GetWorld( EWorldType::PIE );
#endif
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

void URookAudioController::CheckAudioType() {
	switch ( AudioSourceModel.AudioBus ) {
		case EAudioBus::Ambience:
		case EAudioBus::EFX:
		case EAudioBus::Foley:
		case EAudioBus::HUD:
		case EAudioBus::Music:
		case EAudioBus::SFX:
		case EAudioBus::Voice:
			AudioSourceModel.AudioType = EAudioType::is2D;
			break;
		case EAudioBus::EFX3D:
		case EAudioBus::Foley3D:
		case EAudioBus::SFX3D:
		case EAudioBus::Voice3D:
			AudioSourceModel.AudioType = EAudioType::is3D;
			break;
	}
}

bool URookAudioController::HasPlayLimitReached() {
	uint8 TemporaryLimit = 0;
	if (AudioSources.Num() > 0) {
		for (TPair< uint32, FAudioSourceModel >& Kvp : AudioSources) {
			if ( Kvp.Value.AudioState == EAudioState::Playing ) 
				++TemporaryLimit;
		}
	}
	return TemporaryLimit >= AudioSourceModel.PlayLimit;
}

void URookAudioController::SetUpMultichannelSource( const TWeakObjectPtr<class AActor> Parent ) {
	if ( !Parent.IsValid() )
		return;
	AudioSourceModel.ParentID = Parent->GetUniqueID();
	const TWeakObjectPtr<class USoundWave> TemporaryAsset = GetMultichannelAudioSource();
	TemporaryAviableAudioSources.Empty();

	if ( TemporaryAsset.IsValid() ) {
		TWeakObjectPtr<UAudioComponent> TemporaryAudioComp = NewObject<UAudioComponent>( Parent.Get() );
		TemporaryAudioComp->OnAudioFinishedNative.AddUObject( this, &URookAudioController::MultichannelFinishedPlaying );
		TemporaryAudioComp->SetSound( TemporaryAsset.Get() );

		if ( AudioSourceModel.bUseRandomPitch ) {
			TemporaryAudioComp->PitchModulationMin = AudioSourceModel.BottomRandomPitchValue;
			TemporaryAudioComp->PitchModulationMax = AudioSourceModel.TopRandomPitchValue;
		}

		if ( AudioSourceModel.AudioSourceEAX != EEAX::None ) {
			TemporaryAudioComp->bReverb = true;
			RookUtils::Instance().SetReverbInUnreal( AudioSourceModel.AudioSourceEAX );
		}		
		const float TemporaryVolume = RookUtils::Instance().DecibelsToVolume( BeginingDecbiles ) * ApplicationVolumeMultiplier * 0.01f;
		TemporaryAudioComp->SetVolumeMultiplier( TemporaryVolume );
		TemporaryAudioComp->Play();
		MultichannelComponents.Add( TemporaryAudioComp );
		TemporaryAudioComp = nullptr;
	}	
}

TWeakObjectPtr<class USoundWave> URookAudioController::GetMultichannelAudioSource() {
	for ( FMultichannelAudioModel MultichannelAudioModel : AudioSourceModel.MultiChannelAssets ) {
		if( MultichannelAudioModel.AudioAsset )
			TemporaryAviableAudioSources.AddUnique( MultichannelAudioModel.AudioAsset );
	}
	TWeakObjectPtr<class USoundWave> TemporaryMultichannelAudioAsset = nullptr;
	BeginingDecbiles = -100.0f;
	switch ( AudioSourceModel.PlaybackOption ) {
		case EPlayback::Single:
		case EPlayback::Loop:			
			TemporaryMultichannelAudioAsset = AudioSourceModel.MultiChannelAssets[0].AudioAsset;
			BeginingDecbiles = AudioSourceModel.MultiChannelAssets[0].Decibels;
			break;
		case EPlayback::Random:
		case EPlayback::SingleRandom:			
			TemporaryMultichannelAudioAsset =  GetRandomAudioSource( AudioSourceModel.ParentID );
			BeginingDecbiles = AudioSourceModel.MultiChannelAssets[RandomIndicis[AudioSourceModel.ParentID]].Decibels;
			break;
		case EPlayback::Sequence:
		case EPlayback::SingleSequence:			
			TemporaryMultichannelAudioAsset = GetSequenceAudioSource( AudioSourceModel.ParentID );
			BeginingDecbiles = AudioSourceModel.MultiChannelAssets[SequenceIndicis[AudioSourceModel.ParentID]].Decibels;
			break;
	}
	return TemporaryMultichannelAudioAsset;
}

void URookAudioController::MultichannelFinishedPlaying( UAudioComponent* UnrealAudioComponent ) {
	const TWeakObjectPtr<AActor> Parent = Cast<AActor>( UnrealAudioComponent->GetOuter() );
	MultichannelComponents.Remove( UnrealAudioComponent );	
	UnrealAudioComponent->ConditionalBeginDestroy();
	switch ( AudioSourceModel.PlaybackOption ) {
		case EPlayback::Loop:
		case EPlayback::Random:
		case EPlayback::Sequence:
			Play( Parent );
		break;
	}
}

void URookAudioController::OnEndPlay() {
	Stop();
}

URookAudioController::~URookAudioController() {
	if ( AudioSources.Num() > 0 )	{
		for ( TPair< uint32, FAudioSourceModel >& Kvp : AudioSources ) {
			OpenALSoft::Instance().RemoveAudioSource( Kvp.Key );
		}
		AudioSources.Empty();
	}

	if ( MultichannelComponents.Num() > 0 )	{
		for ( TWeakObjectPtr<UAudioComponent> UnrealAudioComponent : MultichannelComponents ) {
			if( UnrealAudioComponent.IsValid() )
				UnrealAudioComponent->ConditionalBeginDestroy();
		}
		MultichannelComponents.Empty();
	}
	if( RookInterface.IsValid() )
		RookInterface->OnEndPlay.Remove( EndPlayHnadle );
	if ( FailedToPlayAudioModel.Num() > 0 )
		FailedToPlayAudioModel.Empty();
	ActiveListenerController = nullptr;
	DataLoader = nullptr;
}