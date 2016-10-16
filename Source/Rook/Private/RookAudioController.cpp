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
		 EndPlayHandle = RookInterface->OnEndPlay.AddUObject( this, &URookAudioController::OnEndPlay );
		 EAXOverlapHandle = RookInterface->OnEAXOverlap.AddUObject( this, &URookAudioController::EAXOverlap );
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
					b3DFailedToPlay = !OpenALSoft::Instance().Play( AudioModels[Kvp.Key] );
					FailedToPlayAudioModel[Kvp.Key] = b3DFailedToPlay;
					
					if( !b3DFailedToPlay )
						AudioModels[Kvp.Key].AudioState = EAudioState::Playing;

					if ( b3DFailedToPlay && !bHasFail ) {
						bHasFail = true;
					}						
				}				
			}
			if ( !bHasFail )
				FailedToPlayAudioModel.Empty();
		}			

		if ( OutOfRangeParents.Num() > 0 ) {
			GetAudioMaxiumDistance();
			TArray<TWeakObjectPtr<AActor>> ParentsToRemove;
			if ( HasActiveListenerController() ) {
				for ( TWeakObjectPtr<AActor> TemporaryParent : OutOfRangeParents ) {
					if ( RookUtils::Instance().InSpehereRadius( TemporaryParent->GetActorLocation(), ActiveListenerController->GetListenerLocation(), MaxiumDistanceToListener ) ) {
						Play( TemporaryParent );
						ParentsToRemove.Add( TemporaryParent );
					}
				}

				if ( ParentsToRemove.Num() ) {
					for ( TWeakObjectPtr<AActor> TemporaryParent : ParentsToRemove ) {
						OutOfRangeParents.Remove( TemporaryParent );
					}
					ParentsToRemove.Empty();
				}
			}			
		}

		if ( AudioModels.Num() > 0 ) {
			for ( TPair< uint32, FAudioSourceModel >& Kvp : AudioModels ) {
				if ( Kvp.Value.AudioType == EAudioType::is3D ) {
					CheckIfSourceFinishedPlaying( Kvp.Key, DeltaTime );
				}

				TWeakObjectPtr<AActor> TemporaryParent;
				for ( TObjectIterator<AActor> Itr; Itr; ++Itr ) {
					if ( Itr->GetUniqueID() == Kvp.Value.ParentID ) {
						TemporaryParent = Cast<AActor>( *Itr );
						break;
					}
				}

				PerformMonoFading( Kvp.Key );

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
			CheckAndRemoveUnusedModels();
		}	
	} else {
		PerformMultichannelFading( DeltaTime );
	}
}

void URookAudioController::CheckAndRemoveUnusedModels() {
	if ( ModelsToRemove.Num() == 0 )
		return;

	for ( uint32 AudioModelID : ModelsToRemove ) {
		AudioModels.Remove( AudioModelID );
	}
	ModelsToRemove.Empty();
	AudioModels.Compact();
}

void URookAudioController::Play( const TWeakObjectPtr<class AActor> Parent, FName Tag ) {
	if ( !RookInterface->bIsRookEnabled )
		return;
	TemporaryTag = Tag;
	CheckAudioType();
	if ( AudioSourceModel.AudioType == EAudioType::is3D ) {
		if ( HasPlayLimitReached() )
			return;
		GetAudioMaxiumDistance();
		CheckIfBufferHasAudioData();
		TemporaryAviableAudioSources.Empty();
	
		if ( HasActiveListenerController() && HasAudioModelCurrentSurface() ) {
			bool bShouldPlay = true;
			if ( !bPlayOutsideDistanceRange ) 
				bShouldPlay = RookUtils::Instance().InSpehereRadius( Parent->GetActorLocation(), ActiveListenerController->GetListenerLocation(), MaxiumDistanceToListener );
			
			if ( bShouldPlay ) {
				if ( !ShouldUnPause( Parent->GetUniqueID() ) ) {
					SetUpNewMonoAudio( Parent );
				} else {
					Pause();
				}
			} else {
				OutOfRangeParents.AddUnique( Parent );
			}
		}
	} else {
		SetUpMultichannelSource( Parent );
	}
	TemporaryTag = "";
}

void URookAudioController::Pause() {
	if ( AudioSourceModel.AudioType == EAudioType::is3D ) {
		if ( AudioModels.Num() > 0 ) {
			for ( TPair< uint32, FAudioSourceModel >& Kvp : AudioModels ) {
				if( Kvp.Value.AudioType == EAudioType::is3D ) {
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
	} else {
		//Yeah, I know, however Unreal Audio Component does not have Pause
		for ( TWeakObjectPtr<UAudioComponent> MultichannelComponent : MultichannelComponents ) {
			if( MultichannelComponent.IsValid() )
				MultichannelComponent->Stop();
		}
	}
}

bool URookAudioController::ShouldUnPause( const uint32 ParentID ) {
	if ( AudioModels.Num() > 0 ) {
		for ( TPair< uint32, FAudioSourceModel >& Kvp : AudioModels ) {
			if ( Kvp.Value.ParentID == ParentID && Kvp.Value.AudioState == EAudioState::Paused ) {
				return true;
			}
		}
	}	
	return false;
}

void URookAudioController::Stop() {
	if ( AudioSourceModel.AudioType == EAudioType::is3D ) {
		if ( AudioModels.Num() > 0 ) {
			for ( TPair< uint32, FAudioSourceModel >& Kvp : AudioModels ) {
				if( Kvp.Value.AudioType == EAudioType::is3D ) {
					OpenALSoft::Instance().RemoveAudioSource( Kvp.Key );
				}				
			}
			AudioModels.Empty();
			AudioModels.Compact();
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
		if ( AudioModels.Num() > 0 ) {
			for ( TPair< uint32, FAudioSourceModel >& Kvp : AudioModels ) {
				if ( Kvp.Value.AudioType == EAudioType::is3D ) {
					OpenALSoft::Instance().SetEAXOnAudioSource( CurrentAudioControllerEAX, Kvp.Key );
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
			MultichannelFadeHelper[MultichannelAudio].InternalVolume = TemporaryVolume;
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
	if ( !Parent.IsValid() )
		return;
	const uint32 TemporaryNewAudioSourceID = RookUtils::Instance().GetUniqueID();
	AudioSourceModel.AudioSourceID = TemporaryNewAudioSourceID;
	AudioSourceModel.ParentID = Parent->GetUniqueID();
	AudioSourceModel.CurrentAudioSourceAsset = GetMonoAudioSource();

	AudioSourceModel.Tag = TemporaryTag;

	if ( AudioSourceModel.CurrentAudioSourceAsset == nullptr || AudioSourceModel.CurrentAudioSourceAsset->NumChannels != 1 ) {
		UE_LOG( RookLog, Warning, TEXT("While trying to Play, Mono audio source is NULL or Multichannel! Will not play.") );
		AudioModels.Remove( TemporaryNewAudioSourceID );
		TemporaryAviableAudioSources.Empty();
		return;
	}	
	const FVector ParentLocation = Parent->GetActorLocation();
	CheckMonoFading();

	if ( AudioSourceModel.VolumeOverDistanceCurve ) {
		const float DistanceToListener = ( ParentLocation - ActiveListenerController->GetListenerLocation() ).Size();
		AudioSourceModel.AudioSourceGain = AudioSourceModel.VolumeOverDistanceCurve->GetFloatValue( DistanceToListener ) * ApplicationVolumeMultiplier * AudioSourceModel.FadeFactor * 0.01f;
	} else {
		AudioSourceModel.AudioSourceGain = 0.0f;
	}
	
	AudioSourceModel.AudioSourceLocation = ParentLocation;

	if ( AudioSourceModel.bUseRandomPitch ) {
		AudioSourceModel.AudioSourceRandomPitch = FMath::FRandRange( AudioSourceModel.BottomRandomPitchValue, AudioSourceModel.TopRandomPitchValue );
	}

	if ( AudioSourceModel.AudioSourceEAX != CurrentAudioControllerEAX )
		AudioSourceModel.AudioSourceEAX = CurrentAudioControllerEAX;
	
	TemporaryAviableAudioSources.Empty();
	AudioModels.Add( TemporaryNewAudioSourceID, AudioSourceModel );
	b3DFailedToPlay = !OpenALSoft::Instance().Play( AudioModels[TemporaryNewAudioSourceID] );
	
	AudioSourceModel.FadeFactor = 1.0f;
	AudioSourceModel.bFadeIn = false;
	AudioSourceModel.StartFadeOutAt = 0.0f;
	AudioSourceModel.IndividualFadeCurve = nullptr;

	if ( b3DFailedToPlay ) {
		FailedToPlayAudioModel.Add( TemporaryNewAudioSourceID, b3DFailedToPlay );
	} else {
		AudioModels[TemporaryNewAudioSourceID].AudioState = EAudioState::Playing;
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

TWeakObjectPtr<class USoundWave> URookAudioController::GetMonoAudioSource() {
	TWeakObjectPtr<class USoundWave> TemporaryAudioAsset = nullptr;
	switch ( AudioSourceModel.PlaybackOption ) {
		case EPlayback::Single:			
		case EPlayback::Loop:
			if ( AudioSourceModel.MonoAssets[0].FadeCurve )
				AudioSourceModel.IndividualFadeCurve = AudioSourceModel.MonoAssets[0].FadeCurve;
			TemporaryAudioAsset = TemporaryAviableAudioSources[0];
			break;
		case EPlayback::Random:
		case EPlayback::SingleRandom:			
			TemporaryAudioAsset = GetRandomAudioSource();
			if ( AudioSourceModel.MonoAssets[RandomIndicis[AudioSourceModel.ParentID]].FadeCurve )
				AudioSourceModel.IndividualFadeCurve = AudioSourceModel.MonoAssets[RandomIndicis[AudioSourceModel.ParentID]].FadeCurve;
			break;
		case EPlayback::Sequence:
		case EPlayback::SingleSequence:
			TemporaryAudioAsset = GetSequenceAudioSource();
			if ( AudioSourceModel.MonoAssets[SequenceIndicis[AudioSourceModel.ParentID]].FadeCurve )
				AudioSourceModel.IndividualFadeCurve = AudioSourceModel.MonoAssets[SequenceIndicis[AudioSourceModel.ParentID]].FadeCurve;
			break;
	}	
	return TemporaryAudioAsset;
}

TWeakObjectPtr<class USoundWave> URookAudioController::GetRandomAudioSource() {
	if ( !RandomIndicis.Contains( AudioSourceModel.ParentID ) )
		RandomIndicis.Add( AudioSourceModel.ParentID, 0 );
	
	if ( TemporaryAviableAudioSources.Num() == 1 ) {
		RandomIndicis[AudioSourceModel.ParentID] = 0;			
		return TemporaryAviableAudioSources[0];
	} else {
		uint16 TemporaryRandomIndex = FMath::RandRange( 0, TemporaryAviableAudioSources.Num() -1 );
		while ( TemporaryRandomIndex == RandomIndicis[AudioSourceModel.ParentID] ) {
			TemporaryRandomIndex = FMath::RandRange( 0, TemporaryAviableAudioSources.Num() -1 );
		}
		RandomIndicis[AudioSourceModel.ParentID] = TemporaryRandomIndex;
		return TemporaryAviableAudioSources[RandomIndicis[AudioSourceModel.ParentID]];
	}	
	return nullptr;
}

TWeakObjectPtr<class USoundWave> URookAudioController::GetSequenceAudioSource() {
	if ( !SequenceIndicis.Contains( AudioSourceModel.ParentID ) )
		SequenceIndicis.Add( AudioSourceModel.ParentID, 0 );
	
	if ( TemporaryAviableAudioSources.Num() > 0 ) {		
		TWeakObjectPtr<class USoundWave> TemporarySequenceSource = TemporaryAviableAudioSources[SequenceIndicis[AudioSourceModel.ParentID]];
		++SequenceIndicis[AudioSourceModel.ParentID];
		if ( SequenceIndicis[AudioSourceModel.ParentID] > TemporaryAviableAudioSources.Num() - 1 )
			SequenceIndicis[AudioSourceModel.ParentID] = 0;
		return TemporarySequenceSource;
	}
	return nullptr;
}

void URookAudioController::CheckMonoFading() {
	TWeakObjectPtr<class UCurveVector>	TemporaryVectorFadeCurve = nullptr;
	if ( AudioSourceModel.IndividualFadeCurve.IsValid() ) {
		TemporaryVectorFadeCurve = AudioSourceModel.IndividualFadeCurve;
	} else if( AudioSourceModel.FadeCurve ) {
		TemporaryVectorFadeCurve = AudioSourceModel.FadeCurve;
	}	

	if ( TemporaryVectorFadeCurve.IsValid() ) {
		if ( TemporaryVectorFadeCurve->FloatCurves[0].GetLastKey().Time > 0.0f ) {
			AudioSourceModel.FadeFactor = 0.0f;
			AudioSourceModel.bFadeIn = true;
		}
		if ( TemporaryVectorFadeCurve->FloatCurves[1].GetLastKey().Time > 0.0f ) {
			AudioSourceModel.StartFadeOutAt = ( AudioSourceModel.CurrentAudioSourceAsset->GetDuration() * AudioSourceModel.AudioSourceRandomPitch ) - TemporaryVectorFadeCurve->FloatCurves[1].GetLastKey().Time;
			if ( AudioSourceModel.StartFadeOutAt < 0.0f )
				AudioSourceModel.StartFadeOutAt = 0.0f;
		}
		TemporaryVectorFadeCurve = nullptr;
	}	
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

void URookAudioController::CheckIfSourceFinishedPlaying( const uint32 AudioModelID, const float DeltaTime ) {
	if ( AudioModels[AudioModelID].AudioState == EAudioState::Playing ) {
		if ( AudioModels[AudioModelID].PlaybackOption != EPlayback::Loop ) {
			if ( OpenALSoft::Instance().AudioSourceState( AudioModels[AudioModelID].AudioSourceID ) == EAudioState::Stopped )
				AudioModels[AudioModelID].AudioState = EAudioState::WasPlaying;
		} else {
			if ( AudioModels[AudioModelID].OverallTime >= AudioModels[AudioModelID].CurrentAudioSourceAsset->GetDuration() ) {
				AudioModels[AudioModelID].AudioState = EAudioState::WasPlaying;
				AudioModels[AudioModelID].OverallTime = 0.0f;
			} else {
				AudioModels[AudioModelID].OverallTime += DeltaTime;
			}				
		}
	}	
}

void URookAudioController::PerformMonoFading( const uint32 AudioModelID ) {
	if ( AudioModels[AudioModelID].AudioState != EAudioState::Playing )
		return;
	TWeakObjectPtr<UCurveVector> TemporaryFadeCurve = nullptr;
	if ( AudioModels[AudioModelID].FadeCurve ) {
		TemporaryFadeCurve = AudioModels[AudioModelID].FadeCurve;
	} else {
		TemporaryFadeCurve = AudioModels[AudioModelID].IndividualFadeCurve;
	}

	if ( TemporaryFadeCurve.IsValid() ) {
		const float CurrentPlaybackTime = OpenALSoft::Instance().CurrentPositionOnAudioTrack( AudioModels[AudioModelID].AudioSourceID );

		if ( AudioModels[AudioModelID].bFadeIn ) {
			AudioModels[AudioModelID].FadeFactor = TemporaryFadeCurve->GetVectorValue( CurrentPlaybackTime ).Y * 0.01f;
			if ( AudioModels[AudioModelID].FadeFactor >= 0.99f ) {
				AudioModels[AudioModelID].bFadeIn = false;
				AudioModels[AudioModelID].FadeFactor = 1.0f;
			}					
		} else if ( CurrentPlaybackTime >= AudioModels[AudioModelID].StartFadeOutAt && AudioModels[AudioModelID].StartFadeOutAt > 0.0f ) {
			AudioModels[AudioModelID].FadeFactor = TemporaryFadeCurve->GetVectorValue( CurrentPlaybackTime - AudioModels[AudioModelID].StartFadeOutAt ).X * 0.01f;
		}	 
		TemporaryFadeCurve = nullptr;
	}	
}
void URookAudioController::PerformMultichannelFading( const float DeltaTime ) {
	for ( TPair< TWeakObjectPtr<UAudioComponent>, FMultichannelFadeModel >& Kvp : MultichannelFadeHelper ) {
		Kvp.Value.OverallTime += DeltaTime;
		float VolumeMultiplier = 0.0f;
		bool bApplyFade = false;
		if ( Kvp.Value.bFadeIn ) {
			Kvp.Value.FadeFactor = Kvp.Value.FadeCurve->GetVectorValue( Kvp.Value.FadeTime ).Y * 0.01f;
			bApplyFade = true;
			
			if ( Kvp.Value.FadeFactor >= 0.99f ) {
				Kvp.Value.bFadeIn = false;
				Kvp.Value.FadeFactor = 1.0f;
				Kvp.Value.FadeTime = 0.0f;				
			} 
			
		} else if ( Kvp.Value.OverallTime >= Kvp.Value.StartFadeOutAt && Kvp.Value.StartFadeOutAt > 0.0f ) {
			Kvp.Value.FadeFactor = Kvp.Value.FadeCurve->GetVectorValue( Kvp.Value.FadeTime ).X * 0.01f;			
			bApplyFade = true;
		}

		if ( bApplyFade && Kvp.Key.IsValid() ) {
			VolumeMultiplier = Kvp.Value.InternalVolume * Kvp.Value.FadeFactor;
			Kvp.Key->SetVolumeMultiplier( VolumeMultiplier );
			Kvp.Value.FadeTime += DeltaTime;
		}			
	}	
}

void URookAudioController::AfterFinishedPlaying( const TWeakObjectPtr<AActor> Parent, const uint32 AudioModelID ) {
	EPlayback LastPlaybackOption = AudioModels[AudioModelID].PlaybackOption;
	if ( AudioModels[AudioModelID].AudioType == EAudioType::is3D )
		OpenALSoft::Instance().RemoveAudioSource( AudioModels[AudioModelID].AudioSourceID );
	ModelsToRemove.Add( AudioModelID );	
	FinishPlaying.Broadcast( AudioModels[AudioModelID].Tag );
	
	if ( Parent.IsValid() ) {
		switch ( LastPlaybackOption ) {
		case EPlayback::Loop:
			FinishLoop.Broadcast( AudioModels[AudioModelID].Tag );
			Play( Parent );
			break;
		case EPlayback::Random:
			Play( Parent );
			break; 
		case EPlayback::Sequence:
			FinishSequence.Broadcast( AudioModels[AudioModelID].Tag );
			Play( Parent );
			break;
		}
	}	

	
}

void URookAudioController::UpdateLocation( const TWeakObjectPtr<AActor> Parent, const uint32  AudioModelID ){
	if ( AudioModels[AudioModelID].AudioSourceLocation != Parent->GetActorLocation() ) {
		AudioModels[AudioModelID].AudioSourceLocation = Parent->GetActorLocation();
		OpenALSoft::Instance().UpdateAudioSourcePosition( AudioModels[AudioModelID].AudioSourceID, Parent->GetActorLocation() );

		if ( AudioModels[AudioModelID].bUseVelocity )
			OpenALSoft::Instance().UpdateAudioSourceVelocity( AudioModels[AudioModelID].AudioSourceID, Parent->GetVelocity() );
	}

	if ( AudioModels[AudioModelID].bUseRaytrace ) {
		RaytraceToListener( Parent, AudioModels[AudioModelID].AudioSourceID );
	} else {
		const float DistanceToListener = ( Parent->GetActorLocation() - ActiveListenerController->GetListenerLocation() ).Size();

		if ( AudioModels[AudioModelID].VolumeOverDistanceCurve ) {
			const float AudioGain = AudioModels[AudioModelID].VolumeOverDistanceCurve->GetFloatValue( DistanceToListener ) * AudioModels[AudioModelID].FadeFactor * ApplicationVolumeMultiplier * 0.01f;
			//UE_LOG(RookLog, Warning, TEXT("gain %f , fade factor %f"), AudioGain, AudioModels[AudioModelID].FadeFactor);

			OpenALSoft::Instance().ChangeAudioSourceGain( AudioModels[AudioModelID].AudioSourceID, AudioGain );
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

		if ( AudioModels[AudioModelID].LowpassOverDistanceCurve ) {
			const float LowpassGain = AudioModels[AudioModelID].LowpassOverDistanceCurve->GetVectorValue( DistanceToListener ).X;
			const float LowpassHFGain = AudioModels[AudioModelID].LowpassOverDistanceCurve->GetVectorValue( DistanceToListener ).Y;
			OpenALSoft::Instance().ChangeLowpassFilter( AudioModels[AudioModelID].AudioSourceID, LowpassGain, LowpassHFGain );
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

		if ( AudioModels[AudioModelID].BandpassOverDistanceCurve ) {
			const float BandpassHFGain = AudioModels[AudioModelID].BandpassOverDistanceCurve->GetVectorValue( DistanceToListener ).X;
			const float BandpassLFGain = AudioModels[AudioModelID].BandpassOverDistanceCurve->GetVectorValue( DistanceToListener ).Y;
			OpenALSoft::Instance().ChangeBandpassFilter( AudioModels[AudioModelID].AudioSourceID, BandpassHFGain, BandpassLFGain );
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
	if ( AudioModels.Num() > 0 ) {
		for ( TPair< uint32, FAudioSourceModel >& Kvp : AudioModels ) {
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
	AudioSourceModel.CurrentAudioSourceAsset = TemporaryAsset;
	TemporaryAviableAudioSources.Empty();
	
	if ( TemporaryAsset.IsValid() ) {
		if ( TemporaryAsset->NumChannels == 1 ) {
			UE_LOG( RookLog, Warning, TEXT("While trying to Play Multichannel audio asset %s was Mono. Will not play"), *TemporaryAsset->GetName() );
			return;
		}
		TWeakObjectPtr<UAudioComponent> TemporaryAudioComp = NewObject<UAudioComponent>( Parent.Get() );
		TemporaryAudioComp->OnAudioFinishedNative.AddUObject( this, &URookAudioController::MultichannelFinishedPlaying );
		TemporaryAudioComp->SetSound( TemporaryAsset.Get() );
		
		TemporaryAudioComp->ComponentTags.Add( TemporaryTag );
		
		if ( AudioSourceModel.bUseRandomPitch ) {
			TemporaryAudioComp->PitchModulationMin = AudioSourceModel.BottomRandomPitchValue;
			TemporaryAudioComp->PitchModulationMax = AudioSourceModel.TopRandomPitchValue;
		}
		
		if ( AudioSourceModel.AudioSourceEAX != EEAX::None ) {
			TemporaryAudioComp->bReverb = true;
			RookUtils::Instance().SetReverbInUnreal( AudioSourceModel.AudioSourceEAX );
		}		

		CheckMultichannelFading( TemporaryAudioComp );

		float TemporaryVolume = 1.0f;
		if ( MultichannelFadeHelper.Contains( TemporaryAudioComp ) ) {
			TemporaryVolume = 0.0f;
			MultichannelFadeHelper[TemporaryAudioComp].InternalVolume = RookUtils::Instance().DecibelsToVolume( BeginingDecbiles ) * ApplicationVolumeMultiplier * 0.01f;
		} else {
			TemporaryVolume = RookUtils::Instance().DecibelsToVolume( BeginingDecbiles ) * ApplicationVolumeMultiplier * 0.01f;
		}

		TemporaryAudioComp->SetVolumeMultiplier( TemporaryVolume );
		TemporaryAudioComp->Play();
		MultichannelComponents.Add( TemporaryAudioComp );
		TemporaryAudioComp = nullptr;
		AudioSourceModel.IndividualFadeCurve = nullptr;
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
			if ( AudioSourceModel.MultiChannelAssets[0].FadeCurve )
				AudioSourceModel.IndividualFadeCurve = AudioSourceModel.MultiChannelAssets[0].FadeCurve;
			break;
		case EPlayback::Random:
		case EPlayback::SingleRandom:			
			TemporaryMultichannelAudioAsset = GetRandomAudioSource();
			BeginingDecbiles = AudioSourceModel.MultiChannelAssets[RandomIndicis[AudioSourceModel.ParentID]].Decibels;
			if ( AudioSourceModel.MultiChannelAssets[RandomIndicis[AudioSourceModel.ParentID]].FadeCurve )
				AudioSourceModel.IndividualFadeCurve = AudioSourceModel.MultiChannelAssets[RandomIndicis[AudioSourceModel.ParentID]].FadeCurve;
			break;
		case EPlayback::Sequence:
		case EPlayback::SingleSequence:			
			TemporaryMultichannelAudioAsset = GetSequenceAudioSource();
			BeginingDecbiles = AudioSourceModel.MultiChannelAssets[SequenceIndicis[AudioSourceModel.ParentID]].Decibels;
			if ( AudioSourceModel.MultiChannelAssets[SequenceIndicis[AudioSourceModel.ParentID]].FadeCurve )
				AudioSourceModel.IndividualFadeCurve = AudioSourceModel.MultiChannelAssets[SequenceIndicis[AudioSourceModel.ParentID]].FadeCurve;
			break;
	}
	return TemporaryMultichannelAudioAsset;
}

void URookAudioController::CheckMultichannelFading( const TWeakObjectPtr<class UAudioComponent> UnrealAudioComponent ) {
	TWeakObjectPtr<class UCurveVector>	TemporaryVectorFadeCurve = nullptr;
	if ( AudioSourceModel.IndividualFadeCurve.IsValid() ) {
		TemporaryVectorFadeCurve = AudioSourceModel.IndividualFadeCurve;		
	} else if ( AudioSourceModel.FadeCurve ) {
		TemporaryVectorFadeCurve = AudioSourceModel.FadeCurve;
	}

	if ( TemporaryVectorFadeCurve.IsValid() ) {
		FMultichannelFadeModel FadeModel;
		if ( TemporaryVectorFadeCurve->FloatCurves[0].GetLastKey().Time > 0.0f ) {
			FadeModel.FadeFactor = 0.0f;
			FadeModel.FadeTime = 0.0f;
			FadeModel.bFadeIn = true;
		}
			
		if ( TemporaryVectorFadeCurve->FloatCurves[1].GetLastKey().Time > 0.0f ) {
			FadeModel.StartFadeOutAt = ( AudioSourceModel.CurrentAudioSourceAsset->GetDuration() * UnrealAudioComponent->PitchMultiplier) - TemporaryVectorFadeCurve->FloatCurves[1].GetLastKey().Time;
			if ( FadeModel.StartFadeOutAt < 0.0f )
				FadeModel.StartFadeOutAt = 0.0f;
		}
		FadeModel.FadeCurve = TemporaryVectorFadeCurve;
		MultichannelFadeHelper.Add( UnrealAudioComponent, FadeModel );
		TemporaryVectorFadeCurve = nullptr;
	}
}

void URookAudioController::MultichannelFinishedPlaying( UAudioComponent* UnrealAudioComponent ) {
	const TWeakObjectPtr<AActor> Parent = Cast<AActor>( UnrealAudioComponent->GetOuter() );
	MultichannelComponents.Remove( UnrealAudioComponent );
	MultichannelFadeHelper.Remove( UnrealAudioComponent );
	MultichannelFadeHelper.Compact();
	AudioSourceModel.IndividualFadeCurve = nullptr;
	FinishPlaying.Broadcast( UnrealAudioComponent->ComponentTags[0] );
	UnrealAudioComponent->ConditionalBeginDestroy();

	switch ( AudioSourceModel.PlaybackOption ) {
		case EPlayback::Loop:
			FinishLoop.Broadcast( UnrealAudioComponent->ComponentTags[0] );
			Play(Parent);
			break;
		case EPlayback::Random:
			Play(Parent);
			break;
		case EPlayback::Sequence:
			FinishSequence.Broadcast( UnrealAudioComponent->ComponentTags[0] );
			Play( Parent );
		break;
	}
}

void URookAudioController::OnEndPlay() {
	Stop();
}

void URookAudioController::EAXOverlap( const uint32 ActorID, const EEAX EAX ) {
	for ( TPair< uint32, FAudioSourceModel >& Kvp : AudioModels ) {
		if ( Kvp.Value.ParentID == ActorID ) {
			if ( Kvp.Value.AudioType == EAudioType::is3D ) {
				OpenALSoft::Instance().SetEAXOnAudioSource( EAX, Kvp.Key );
			} else {
				RookUtils::Instance().SetReverbInUnreal( EAX );
			}
			break;
		}
	}
}

URookAudioController::~URookAudioController() {
	if ( AudioModels.Num() > 0 )	{
		for ( TPair< uint32, FAudioSourceModel >& Kvp : AudioModels ) {
			OpenALSoft::Instance().RemoveAudioSource( Kvp.Key );
		}
		AudioModels.Empty();
	}

	if ( MultichannelComponents.Num() > 0 )	{
		for ( TWeakObjectPtr<UAudioComponent> UnrealAudioComponent : MultichannelComponents ) {
			if( UnrealAudioComponent.IsValid() )
				UnrealAudioComponent->ConditionalBeginDestroy();
		}
		MultichannelComponents.Empty();		
	}
	MultichannelFadeHelper.Empty();

	if( RookInterface.IsValid() ) {
		RookInterface->OnEndPlay.Remove( EndPlayHandle );
		RookInterface->OnEAXOverlap.Remove( EAXOverlapHandle );
	}
	if ( FailedToPlayAudioModel.Num() > 0 )
		FailedToPlayAudioModel.Empty();
	ActiveListenerController = nullptr;
	DataLoader = nullptr;
}