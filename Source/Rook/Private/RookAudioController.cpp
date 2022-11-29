/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#include "RookAudioController.h"
#include "RookPrivatePCH.h"
#include "Runtime/Engine/Classes/Sound/SoundWave.h"
#include "Runtime/Engine/Classes/Curves/CurveFloat.h"
#include "OpenALSoft.h"
#include "RookListenerController.h"
#include "RookUtils.h"
#include "RookAudioDataLoader.h"
#include "RookAudioDataLoadingTask.h"

URookAudioController::URookAudioController()
	: bUseDebugSpheres(false)
	, bPlayOutsideDistanceRange(false)
	, ActiveListenerController(nullptr)
	, MaxiumDistanceToListener(0.0f)
	, DataLoader(nullptr)
	, CurrentAudioControllerSurafce(EPhysicalSurface::SurfaceType_Default)
	, CurrentAudioControllerEAX(EEAX::None)
	, BeginingDecbiles(-100.0f)
	, ApplicationVolumeMultiplier(1.0f)
	, RookInterface(nullptr)
	, b3DFailedToPlay(false)
{
	ApplicationVolumeMultiplier = FApp::GetVolumeMultiplier();
	OpenALSoft::Instance().SetVolumeMulitiplier(ApplicationVolumeMultiplier);

	RookInterface = (IRook*)FModuleManager::Get().GetModule("Rook");

	if (RookInterface)
	{
		EndPlayHandle = RookInterface->OnEndPlay.AddUObject(this, &URookAudioController::OnEndPlay);
		EAXOverlapHandle = RookInterface->OnEAXOverlap.AddUObject(this, &URookAudioController::EAXOverlap);
	}
}

bool URookAudioController::IsTickable() const 
{
	if (RookInterface)
	{
		return RookInterface->bIsRookEnabled;
	}
		
	return true;
}

TStatId URookAudioController::GetStatId() const 
{
	return GetStatID();
}

void URookAudioController::Tick(float DeltaTime) 
{
	if (AudioSourceModel.AudioType == EAudioType::is3D) 
	{
		if (FailedToPlayAudioModel.Num() > 0) 
		{
			bool bHasFail = false;
			
			for (TPair< uint32, bool >& FailedModel : FailedToPlayAudioModel) 
			{
				if (FailedModel.Value)
				{
					b3DFailedToPlay = !OpenALSoft::Instance().Play(AudioModels[FailedModel.Key]);
					FailedToPlayAudioModel[FailedModel.Key] = b3DFailedToPlay;

					if (!b3DFailedToPlay) { AudioModels[FailedModel.Key].AudioState = EAudioState::Playing; }						

					if (b3DFailedToPlay && !bHasFail) { bHasFail = true; }											
				}
			}

			if (!bHasFail) { FailedToPlayAudioModel.Empty(); }				
		}

		if (OutOfRangeParents.Num() > 0) 
		{
			GetAudioMaxiumDistance();
			TArray<TWeakObjectPtr<AActor>> ParentsToRemove;
		
			if (HasActiveListenerController()) 
			{
				for (TWeakObjectPtr<AActor> TemporaryParent : OutOfRangeParents)
				{
					if (RookUtils::Instance().InSpehereRadius(TemporaryParent->GetActorLocation(), ActiveListenerController->GetListenerLocation(), MaxiumDistanceToListener)) 
					{
						Play(TemporaryParent);
						ParentsToRemove.Add(TemporaryParent);
					}
				}

				if (ParentsToRemove.Num() > 0) 
				{
					for (TWeakObjectPtr<AActor> TemporaryParent : ParentsToRemove)
					{
						OutOfRangeParents.Remove(TemporaryParent);
					}						
					
					ParentsToRemove.Empty();
				}
			}
		}
		UE_LOG(RookLog, Log, TEXT("Model num %u"), AudioModels.Num());

		if (AudioModels.Num() > 0) 
		{
			for(auto& ModelMap : AudioModels)
			{
				if (ModelMap.Value.AudioType == EAudioType::is3D) { CheckIfSourceFinishedPlaying(ModelMap.Key, DeltaTime); }
								
				TWeakObjectPtr<AActor> TemporaryParent;

				for (TObjectIterator<AActor> Itr; Itr; ++Itr) 
				{
					if (Itr->GetUniqueID() == ModelMap.Value.ParentID)
					{
						TemporaryParent = Cast<AActor>(*Itr);
						break;
					}
				}

				PerformMonoFading(ModelMap.Key);

				switch (ModelMap.Value.AudioState)
				{
					case EAudioState::WasPlaying:
						if (TemporaryParent.IsValid()) { AfterFinishedPlaying(TemporaryParent, ModelMap.Key); }							
						break;
					case EAudioState::Playing:
						if (TemporaryParent.IsValid() && ModelMap.Value.AudioType == EAudioType::is3D) { UpdateLocation(TemporaryParent, ModelMap.Key); }
						break;
					default:
						break;
				}
			}		
		}
	}
	else 
	{
		PerformMultichannelFading(DeltaTime);
	}
}

void URookAudioController::RemoveUnusedModels( const uint32 IdToRemove ) 
{
	AudioModels.Remove(IdToRemove);
	AudioModels.Compact();
}

void URookAudioController::Play(const TWeakObjectPtr<class AActor> Parent, FName Tag) 
{
	if (!RookInterface || (RookInterface && !RookInterface->bIsRookEnabled) || HasPlayLimitReached()) { return; }
	
	TemporaryTag = Tag;
	CheckAudioType();
	
	if (AudioSourceModel.AudioType == EAudioType::is3D) 
	{		
		GetAudioMaxiumDistance();
		CheckIfBufferHasAudioData();
		TemporaryAviableAudioSources.Empty();

		if (HasActiveListenerController() && HasAudioModelCurrentSurface()) 
		{
			bool bShouldPlay = true;
			
			if (!bPlayOutsideDistanceRange)
			{
				bShouldPlay = RookUtils::Instance().InSpehereRadius(Parent->GetActorLocation(), ActiveListenerController->GetListenerLocation(), MaxiumDistanceToListener);
			}

			if (bShouldPlay) 
			{
				if (!ShouldUnPause(Parent->GetUniqueID())) 
				{
					SetUpNewMonoAudio(Parent);
				}
				else 
				{
					Pause();
				}
			}
			else 
			{
				OutOfRangeParents.AddUnique(Parent);
			}
		}
	}
	else 
	{
		SetUpMultichannelSource(Parent);
	}
	TemporaryTag = "";
}

//TODO: refactor nesting!
void URookAudioController::Pause() 
{
	if (AudioSourceModel.AudioType == EAudioType::is3D && AudioModels.Num() > 0)
	{
		for (TPair< uint32, FAudioSourceModel >& AudioModel : AudioModels)
		{
			if (AudioModel.Value.AudioType == EAudioType::is3D)
			{
				switch (AudioModel.Value.AudioState)
				{
				case EAudioState::Playing:					
					AudioModel.Value.AudioState = EAudioState::Paused;
					OpenALSoft::Instance().Pause(AudioModel.Key);					
					break;
				case EAudioState::Paused:
					AudioModel.Value.AudioState = EAudioState::Playing;
					OpenALSoft::Instance().PlayAfterPause(AudioModel.Key);
					break;
				default:
					break;
				}
			}
		}		
	}
	else 
	{
		//Yeah, I know, however Unreal Audio Component does not have Pause
		for (TWeakObjectPtr<UAudioComponent> MultichannelComponent : MultichannelComponents) 
		{
			if (MultichannelComponent.IsValid()) { MultichannelComponent->Stop(); }				
		}
	}
}

bool URookAudioController::ShouldUnPause(const uint32 ParentID) 
{
	if (AudioModels.Num() > 0) 
	{
		for (TPair< uint32, FAudioSourceModel >& AudioModel : AudioModels)
		{
			if (AudioModel.Value.ParentID == ParentID && AudioModel.Value.AudioState == EAudioState::Paused)
			{
				return true;
			}							
		}
	}
	return false;
}

void URookAudioController::Stop() 
{
	if (AudioSourceModel.AudioType == EAudioType::is3D && AudioModels.Num() > 0)
	{
		for (TPair< uint32, FAudioSourceModel >& AudioModel : AudioModels)
		{
			if (AudioModel.Value.AudioType == EAudioType::is3D) { OpenALSoft::Instance().RemoveAudioSource(AudioModel.Key); }
		}

		AudioModels.Empty();
		AudioModels.Compact();
		FailedToPlayAudioModel.Empty();
		FailedToPlayAudioModel.Compact();		
	}
	else 
	{
		for (TWeakObjectPtr<UAudioComponent> MultichannelComponent : MultichannelComponents) 
		{
			if (MultichannelComponent.IsValid()) { MultichannelComponent->Stop(); }				
		}
	}

	RandomIndicis.Empty();
	SequenceIndicis.Empty();
}

void URookAudioController::ChangeSurface(const TEnumAsByte<EPhysicalSurface> NewSurface)
{
	if (NewSurface != CurrentAudioControllerSurafce) 
	{
		CurrentAudioControllerSurafce = NewSurface;
		RandomIndicis.Empty();
		SequenceIndicis.Empty();
	}
}

void URookAudioController::ChangeEAX(const EEAX NewEAX) 
{
	CurrentAudioControllerEAX = NewEAX;

	if (AudioSourceModel.AudioType == EAudioType::is3D && AudioModels.Num() > 0)
	{		
		for (TPair< uint32, FAudioSourceModel >& AudioModel : AudioModels)
		{
			if (AudioModel.Value.AudioType == EAudioType::is3D) { OpenALSoft::Instance().SetEAXOnAudioSource(CurrentAudioControllerEAX, AudioModel.Key); }
		}		
	}
	else 
	{
		RookUtils::Instance().SetReverbInUnreal(CurrentAudioControllerEAX);
	}
}

void URookAudioController::ChangeDecibelsOnActiveMultichannel(const float Decibels) 
{
	for (TWeakObjectPtr<UAudioComponent> MultichannelAudio : MultichannelComponents) 
	{
		if (MultichannelAudio.IsValid()) 
		{
			const float TemporaryVolume = RookUtils::Instance().DecibelsToVolume(Decibels) * ApplicationVolumeMultiplier;
			MultichannelAudio->SetVolumeMultiplier(TemporaryVolume);
			MultichannelFadeHelper[MultichannelAudio].InternalVolume = TemporaryVolume;
		}
	}
}

bool URookAudioController::HasActiveListenerController() 
{
	if (ActiveListenerController.IsValid() == false || ActiveListenerController->IsActive() == false) { GetActiveListenerController(); }
	return ActiveListenerController.IsValid();
}

void URookAudioController::GetActiveListenerController() 
{
	if (!RookInterface) { return; }		

	if (RookInterface->Listeners.Num() > 0)
	{
		for (TWeakObjectPtr<URookListenerController> ListenerController : RookInterface->Listeners)
		{
			if (ListenerController->IsActive())
			{
				ActiveListenerController = ListenerController;
				break;
			}
		}
	}
	else
	{
		TWeakObjectPtr<URookListenerController> NewAudioListener = NewObject<URookListenerController>();
		RookInterface->Listeners.Add(NewAudioListener);
		ActiveListenerController = NewAudioListener;
	}	
}

void URookAudioController::SetUpNewMonoAudio(const TWeakObjectPtr<AActor> Parent) 
{
	if (!Parent.IsValid()) { return; }		
	
	const uint32 TemporaryNewAudioSourceID = RookUtils::Instance().GetUniqueID();
	AudioSourceModel.AudioSourceID = TemporaryNewAudioSourceID;
	AudioSourceModel.ParentID = Parent->GetUniqueID();
	AudioSourceModel.CurrentAudioSourceAsset = GetMonoAudioSource();

	AudioSourceModel.Tag = TemporaryTag;

	if (AudioSourceModel.CurrentAudioSourceAsset == nullptr || (AudioSourceModel.CurrentAudioSourceAsset.IsValid() && AudioSourceModel.CurrentAudioSourceAsset->NumChannels != 1))
	{
		UE_LOG(RookLog, Warning, TEXT("While trying to Play, Mono audio source is NULL or Multichannel! Will not play."));
		AudioModels.Remove(TemporaryNewAudioSourceID);
		TemporaryAviableAudioSources.Empty();
		return;
	}
	
	const FVector ParentLocation = Parent->GetActorLocation();
	CheckMonoFading();

	AudioSourceModel.AudioSourceGain = 0.0f;
	if (AudioSourceModel.VolumeOverDistanceCurve) 
	{
		const float DistanceToListener = (ParentLocation - ActiveListenerController->GetListenerLocation()).Size();
		AudioSourceModel.AudioSourceGain = AudioSourceModel.VolumeOverDistanceCurve->GetFloatValue(DistanceToListener) * ApplicationVolumeMultiplier * AudioSourceModel.FadeFactor * 0.01f;
	}

	AudioSourceModel.AudioSourceLocation = ParentLocation;

	if (AudioSourceModel.bUseRandomPitch)
	{
		AudioSourceModel.AudioSourceRandomPitch = FMath::FRandRange(AudioSourceModel.BottomRandomPitchValue, AudioSourceModel.TopRandomPitchValue);
	}
	
	if (AudioSourceModel.AudioSourceEAX != CurrentAudioControllerEAX)
	{
		AudioSourceModel.AudioSourceEAX = CurrentAudioControllerEAX;
	}		

	TemporaryAviableAudioSources.Empty();
	AudioModels.Add(TemporaryNewAudioSourceID, AudioSourceModel);
	b3DFailedToPlay = !OpenALSoft::Instance().Play(AudioModels[TemporaryNewAudioSourceID]);

	AudioSourceModel.FadeFactor = 1.0f;
	AudioSourceModel.bFadeIn = false;
	AudioSourceModel.StartFadeOutAt = 0.0f;
	AudioSourceModel.IndividualFadeCurve = nullptr;

	if (b3DFailedToPlay) 
	{
		FailedToPlayAudioModel.Add(TemporaryNewAudioSourceID, b3DFailedToPlay);
	}
	else 
	{
		AudioModels[TemporaryNewAudioSourceID].AudioState = EAudioState::Playing;
	}
}

void URookAudioController::GetAudioMaxiumDistance() 
{
	MaxiumDistanceToListener = 0.0f;

	if (AudioSourceModel.VolumeOverDistanceCurve)
	{
		MaxiumDistanceToListener = AudioSourceModel.VolumeOverDistanceCurve->FloatCurve.GetLastKey().Time;
	}
}

void URookAudioController::CheckIfBufferHasAudioData() 
{
	CheckDataLoader();
	
	for (FMonoAudioModel MonoAudioModel : AudioSourceModel.MonoAssets) 
	{
		TArray<TWeakObjectPtr<USoundWave>> AudioToLoad;
		
		if (!OpenALSoft::Instance().Buffers.Contains(MonoAudioModel.AudioAsset->GetUniqueID())) 
		{
			OpenALSoft::Instance().Buffers.Add(MonoAudioModel.AudioAsset->GetUniqueID(), 0);
			AudioToLoad.Add(MonoAudioModel.AudioAsset);
		}
		
		if (AudioToLoad.Num() > 0) 
		{
			DataLoader->LoadingGraph.Add(TGraphTask<RookAudioDataLoadingTask>::CreateTask(nullptr, ENamedThreads::AnyThread).ConstructAndDispatchWhenReady(AudioToLoad));
			AudioToLoad.Empty();
		}
	}
}

void URookAudioController::CheckDataLoader() 
{
	if (DataLoader == nullptr || !DataLoader.IsValid()) 
	{
		TWeakObjectPtr<URookAudioDataLoader> TemporaryDataLoader(nullptr);
		
		for (TObjectIterator<URookAudioDataLoader> Itr; Itr; ++Itr) 
		{
			TemporaryDataLoader = Cast<URookAudioDataLoader>(*Itr);
		}				
		
		DataLoader = TemporaryDataLoader == nullptr ? NewObject<URookAudioDataLoader>() : TemporaryDataLoader;
		
		TemporaryDataLoader = nullptr;
	}
}

TWeakObjectPtr<class USoundWave> URookAudioController::GetMonoAudioSource() 
{
	TWeakObjectPtr<class USoundWave> TemporaryAudioAsset(nullptr);
	
	switch (AudioSourceModel.PlaybackOption)
	{
		case EPlayback::Single:
		case EPlayback::Loop:
			if (AudioSourceModel.MonoAssets[0].FadeCurve) { AudioSourceModel.IndividualFadeCurve = AudioSourceModel.MonoAssets[0].FadeCurve; }
			TemporaryAudioAsset = TemporaryAviableAudioSources[0];
			break;
	
		case EPlayback::Random:
		case EPlayback::SingleRandom:
			TemporaryAudioAsset = GetRandomAudioSource();
			if (AudioSourceModel.MonoAssets[RandomIndicis[AudioSourceModel.ParentID]].FadeCurve)
			{
				AudioSourceModel.IndividualFadeCurve = AudioSourceModel.MonoAssets[RandomIndicis[AudioSourceModel.ParentID]].FadeCurve;
			}
			break;
		
		case EPlayback::Sequence:
		case EPlayback::SingleSequence:
			TemporaryAudioAsset = GetSequenceAudioSource();
			if (AudioSourceModel.MonoAssets[SequenceIndicis[AudioSourceModel.ParentID]].FadeCurve)
			{
				AudioSourceModel.IndividualFadeCurve = AudioSourceModel.MonoAssets[SequenceIndicis[AudioSourceModel.ParentID]].FadeCurve;
			}
			break;
		default:
			break;
	}
	return TemporaryAudioAsset;
}

TWeakObjectPtr<class USoundWave> URookAudioController::GetRandomAudioSource() 
{
	if (!RandomIndicis.Contains(AudioSourceModel.ParentID)) { RandomIndicis.Add(AudioSourceModel.ParentID, 0); }		

	if (TemporaryAviableAudioSources.Num() == 1) 
	{
		RandomIndicis[AudioSourceModel.ParentID] = 0;
		return TemporaryAviableAudioSources[0];
	}
	else 
	{
		uint16 TemporaryRandomIndex = FMath::RandRange(0, TemporaryAviableAudioSources.Num() - 1);

		while (TemporaryRandomIndex == RandomIndicis[AudioSourceModel.ParentID])
		{
			TemporaryRandomIndex = FMath::RandRange(0, TemporaryAviableAudioSources.Num() - 1);
		}
		
		RandomIndicis[AudioSourceModel.ParentID] = TemporaryRandomIndex;
		return TemporaryAviableAudioSources[RandomIndicis[AudioSourceModel.ParentID]];
	}
	return nullptr;
}

TWeakObjectPtr<class USoundWave> URookAudioController::GetSequenceAudioSource() 
{
	if (!SequenceIndicis.Contains(AudioSourceModel.ParentID)) { SequenceIndicis.Add(AudioSourceModel.ParentID, 0); }		

	if (TemporaryAviableAudioSources.Num() > 0) 
	{
		const TWeakObjectPtr<class USoundWave> TemporarySequenceSource(TemporaryAviableAudioSources[SequenceIndicis[AudioSourceModel.ParentID]]);
		++SequenceIndicis[AudioSourceModel.ParentID];
		
		if (SequenceIndicis[AudioSourceModel.ParentID] > TemporaryAviableAudioSources.Num() - 1)
		{
			SequenceIndicis[AudioSourceModel.ParentID] = 0;
		}		
		
		return TemporarySequenceSource;
	}
	return nullptr;
}

void URookAudioController::CheckMonoFading() 
{
	TWeakObjectPtr<class UCurveVector>	TemporaryVectorFadeCurve(nullptr);
	
	if (AudioSourceModel.IndividualFadeCurve.IsValid())
	{
		TemporaryVectorFadeCurve = AudioSourceModel.IndividualFadeCurve;
	}
	else if (AudioSourceModel.FadeCurve) 
	{
		TemporaryVectorFadeCurve = AudioSourceModel.FadeCurve;
	}

	if (TemporaryVectorFadeCurve.IsValid()) 
	{
		if (TemporaryVectorFadeCurve->FloatCurves[0].GetLastKey().Time > 0.0f)
		{
			AudioSourceModel.FadeFactor = 0.0f;
			AudioSourceModel.bFadeIn = true;
		}

		if (TemporaryVectorFadeCurve->FloatCurves[1].GetLastKey().Time > 0.0f) 
		{
			AudioSourceModel.StartFadeOutAt = (AudioSourceModel.CurrentAudioSourceAsset->GetDuration() * AudioSourceModel.AudioSourceRandomPitch) - TemporaryVectorFadeCurve->FloatCurves[1].GetLastKey().Time;
			
			if (AudioSourceModel.StartFadeOutAt < 0.0f) { AudioSourceModel.StartFadeOutAt = 0.0f; }				
		}
	}
}

bool URookAudioController::HasAudioModelCurrentSurface() 
{
	if (TemporaryAviableAudioSources.Num() > 0) { return true; }		

	if (AudioSourceModel.MonoAssets.Num() > 0) 
	{
		for (FMonoAudioModel MonoAudioModel : AudioSourceModel.MonoAssets) 
		{
			if (MonoAudioModel.SurfaceAffectingAudio == CurrentAudioControllerSurafce && MonoAudioModel.AudioAsset)
			{
				TemporaryAviableAudioSources.AddUnique(MonoAudioModel.AudioAsset);
			}						
		}

		return TemporaryAviableAudioSources.Num() != 0;
	}
	return false;
}

void URookAudioController::CheckIfSourceFinishedPlaying(const uint32 AudioModelID, const float DeltaTime)
{

	if (AudioModels[AudioModelID].AudioState != EAudioState::Playing) { return; }
	
	if (AudioModels[AudioModelID].PlaybackOption != EPlayback::Loop) 
	{
		if (OpenALSoft::Instance().AudioSourceState(AudioModels[AudioModelID].AudioSourceID) == EAudioState::Stopped)
		{
			AudioModels[AudioModelID].AudioState = EAudioState::WasPlaying;
		}			
	}
	else 
	{
		if (AudioModels[AudioModelID].OverallTime >= AudioModels[AudioModelID].CurrentAudioSourceAsset->GetDuration()) 
		{
			AudioModels[AudioModelID].AudioState = EAudioState::WasPlaying;
			AudioModels[AudioModelID].OverallTime = 0.0f;
		}
		else 
		{
			AudioModels[AudioModelID].OverallTime += DeltaTime;
		}
	}	
}

void URookAudioController::PerformMonoFading(const uint32 AudioModelID)
{
	if (AudioModels[AudioModelID].AudioState != EAudioState::Playing) { return; }		
	
	const TWeakObjectPtr<UCurveVector> TemporaryFadeCurve = AudioModels[AudioModelID].FadeCurve != nullptr ? AudioModels[AudioModelID].FadeCurve : AudioModels[AudioModelID].IndividualFadeCurve;
	
	if (TemporaryFadeCurve.IsValid() == false) { return; }
	
	const float CurrentPlaybackTime = OpenALSoft::Instance().CurrentPositionOnAudioTrack(AudioModels[AudioModelID].AudioSourceID);

	if (AudioModels[AudioModelID].bFadeIn) 
	{
		AudioModels[AudioModelID].FadeFactor = TemporaryFadeCurve->GetVectorValue(CurrentPlaybackTime).Y * 0.01f;
			
		if (AudioModels[AudioModelID].FadeFactor >= 0.99f) 
		{
			AudioModels[AudioModelID].bFadeIn = false;
			AudioModels[AudioModelID].FadeFactor = 1.0f;
		}
	}
	else if (CurrentPlaybackTime >= AudioModels[AudioModelID].StartFadeOutAt && AudioModels[AudioModelID].StartFadeOutAt > 0.0f) 
	{
		AudioModels[AudioModelID].FadeFactor = TemporaryFadeCurve->GetVectorValue(CurrentPlaybackTime - AudioModels[AudioModelID].StartFadeOutAt).X * 0.01f;
	}	
}
void URookAudioController::PerformMultichannelFading(const float DeltaTime) 
{
	for (TPair< TWeakObjectPtr<UAudioComponent>, FMultichannelFadeModel >& MultichannelFade : MultichannelFadeHelper)
	{
		MultichannelFade.Value.OverallTime += DeltaTime;		
		bool bApplyFade = false;

		if (MultichannelFade.Value.bFadeIn)
		{
			MultichannelFade.Value.FadeFactor = MultichannelFade.Value.FadeCurve->GetVectorValue(MultichannelFade.Value.FadeTime).Y * 0.01f;
			bApplyFade = true;

			if (MultichannelFade.Value.FadeFactor >= 0.99f)
			{
				MultichannelFade.Value.bFadeIn = false;
				MultichannelFade.Value.FadeFactor = 1.0f;
				MultichannelFade.Value.FadeTime = 0.0f;
			}

		}
		else if (MultichannelFade.Value.OverallTime >= MultichannelFade.Value.StartFadeOutAt && MultichannelFade.Value.StartFadeOutAt > 0.0f)
		{
			MultichannelFade.Value.FadeFactor = MultichannelFade.Value.FadeCurve->GetVectorValue(MultichannelFade.Value.FadeTime).X * 0.01f;
			bApplyFade = true;
		}

		if (bApplyFade && MultichannelFade.Key.IsValid())
		{
			const float VolumeMultiplier = MultichannelFade.Value.InternalVolume * MultichannelFade.Value.FadeFactor;
			MultichannelFade.Key->SetVolumeMultiplier(VolumeMultiplier);
			MultichannelFade.Value.FadeTime += DeltaTime;
		}
	}
}

void URookAudioController::AfterFinishedPlaying(const TWeakObjectPtr<AActor> Parent, const uint32 AudioModelID)
{
	const EPlayback LastPlaybackOption = AudioModels[AudioModelID].PlaybackOption;
	
	if (AudioModels[AudioModelID].AudioType == EAudioType::is3D)
	{
		OpenALSoft::Instance().RemoveAudioSource(AudioModels[AudioModelID].AudioSourceID);
	}		
	
	FinishPlaying.Broadcast(AudioModels[AudioModelID].Tag);

	bool bShouldPlay = false;

	if (Parent.IsValid())
	{
		switch (LastPlaybackOption) {
			case EPlayback::Loop:
				FinishLoop.Broadcast(AudioModels[AudioModelID].Tag);
				bShouldPlay = true;
				break;

			case EPlayback::Random:
				bShouldPlay = true;
				break;

			case EPlayback::Sequence:
				FinishSequence.Broadcast(AudioModels[AudioModelID].Tag);
				bShouldPlay = true;
				break;
		}
	}
	RemoveUnusedModels(AudioModelID);

	if(bShouldPlay) { Play(Parent); }		
}

void URookAudioController::UpdateLocation(const TWeakObjectPtr<AActor> Parent, const uint32  AudioModelID) 
{
	if (AudioModels[AudioModelID].AudioSourceLocation != Parent->GetActorLocation()) 
	{
		AudioModels[AudioModelID].AudioSourceLocation = Parent->GetActorLocation();
		OpenALSoft::Instance().UpdateAudioSourcePosition(AudioModels[AudioModelID].AudioSourceID, Parent->GetActorLocation());

		if (AudioModels[AudioModelID].bUseVelocity)
		{
			OpenALSoft::Instance().UpdateAudioSourceVelocity(AudioModels[AudioModelID].AudioSourceID, Parent->GetVelocity());
		}
	}

	if (AudioModels[AudioModelID].bUseRaytrace) 
	{
		RaytraceToListener(Parent, AudioModels[AudioModelID].AudioSourceID);
	}
	else 
	{
		const float DistanceToListener = (Parent->GetActorLocation() - ActiveListenerController->GetListenerLocation()).Size();

		if (AudioModels[AudioModelID].VolumeOverDistanceCurve) 
		{
			const float AudioGain = AudioModels[AudioModelID].VolumeOverDistanceCurve->GetFloatValue(DistanceToListener) * AudioModels[AudioModelID].FadeFactor * ApplicationVolumeMultiplier * 0.01f;
			//UE_LOG(RookLog, Warning, TEXT("gain %f , fade factor %f"), AudioGain, AudioModels[AudioModelID].FadeFactor);

			OpenALSoft::Instance().ChangeAudioSourceGain(AudioModels[AudioModelID].AudioSourceID, AudioGain);
			if (bUseDebugSpheres) 
			{
#if WITH_EDITOR
				DrawDebugSphere(
					RookUtils::Instance().GetWorld(EWorldType::PIE),
					Parent->GetActorLocation(),
					AudioGain,
					24,
					//blue
					FColor(0, 25, 255),
					false,
					1.0f
				);
#endif
			}
		}

		if (AudioModels[AudioModelID].LowpassOverDistanceCurve) 
		{
			const float LowpassGain = AudioModels[AudioModelID].LowpassOverDistanceCurve->GetVectorValue(DistanceToListener).X;
			const float LowpassHFGain = AudioModels[AudioModelID].LowpassOverDistanceCurve->GetVectorValue(DistanceToListener).Y;
			OpenALSoft::Instance().ChangeLowpassFilter(AudioModels[AudioModelID].AudioSourceID, LowpassGain, LowpassHFGain);
#if WITH_EDITOR
			DrawDebugSphere(
				RookUtils::Instance().GetWorld(EWorldType::PIE),
				Parent->GetActorLocation(),
				LowpassGain,
				24,
				//darker yellow
				FColor(255, 210, 0),
				false,
				1.0f
			);
			DrawDebugSphere(
				RookUtils::Instance().GetWorld(EWorldType::PIE),
				Parent->GetActorLocation(),
				LowpassHFGain,
				24,
				//lighter yellow
				FColor(255, 255, 0),
				false,
				1.0f
			);
#endif
		}

		if (AudioModels[AudioModelID].BandpassOverDistanceCurve) 
		{
			const float BandpassHFGain = AudioModels[AudioModelID].BandpassOverDistanceCurve->GetVectorValue(DistanceToListener).X;
			const float BandpassLFGain = AudioModels[AudioModelID].BandpassOverDistanceCurve->GetVectorValue(DistanceToListener).Y;
			OpenALSoft::Instance().ChangeBandpassFilter(AudioModels[AudioModelID].AudioSourceID, BandpassHFGain, BandpassLFGain);
#if WITH_EDITOR
			DrawDebugSphere(
				RookUtils::Instance().GetWorld(EWorldType::PIE),
				Parent->GetActorLocation(),
				BandpassHFGain,
				24,
				//darker green
				FColor(0, 255, 0),
				false,
				1.0f
			);
			DrawDebugSphere(
				RookUtils::Instance().GetWorld(EWorldType::PIE),
				Parent->GetActorLocation(),
				BandpassLFGain,
				24,
				//lighter green
				FColor(0, 255, 100),
				false,
				1.0f
			);
#endif
		}
	}
}

void URookAudioController::RaytraceToListener(const TWeakObjectPtr<AActor> Parent, const uint32 AudioSourceID) 
{
	const FVector StartPosition = Parent->GetActorLocation();
	const FVector EndPosition = ActiveListenerController->GetListenerLocation();
	FHitResult Hit;
	UWorld* TemporaryWorld = RookUtils::Instance().GetWorld();
#if WITH_EDITOR
	TemporaryWorld = RookUtils::Instance().GetWorld(EWorldType::PIE);
#endif
	AActor* PreRayedActor(nullptr);
	FCollisionQueryParams TraceParams(TEXT("RayTrace"), true, Parent.Get());
	TraceParams.bReturnPhysicalMaterial = true;

	float STC = 0.0f;
	float SAC = 0.0f;

	TemporaryWorld->LineTraceSingleByChannel(Hit, StartPosition, EndPosition, ECC_Visibility, TraceParams);

	while (Hit.GetActor() != nullptr && Hit.GetActor() != PreRayedActor && Hit.GetActor() != Parent) 
	{
		PreRayedActor = Hit.GetActor();
		TraceParams.AddIgnoredActor(PreRayedActor);
		TemporaryWorld->LineTraceSingleByChannel(Hit, Hit.Location, EndPosition, ECC_Visibility, TraceParams);

		if (Hit.PhysMaterial != nullptr) 
		{
			if (RookUtils::Instance().STC.Contains(Hit.PhysMaterial->GetName())) 
			{
				STC += RookUtils::Instance().STC[Hit.PhysMaterial->GetName()] / 1.3;
				SAC += RookUtils::Instance().SAC[Hit.PhysMaterial->GetName()];
			}
		}
	}

	float AudioAlterGain = 100.0f - (STC - 25.0f) * 2.857142f - SAC * 100 - Parent->GetVelocity().Size() / 200.0f;
	
	if (AudioAlterGain < 0.0f) 
	{
		AudioAlterGain = 0.0f;
	}
	else if (AudioAlterGain > 100.0f) 
	{
		AudioAlterGain = 100.0f;
	}

	float TargetFilterGain = 1.0f - STC / 100;
	
	if (TargetFilterGain < 0.0f) { TargetFilterGain = 0.0f; }		
	
	OpenALSoft::Instance().ChangeAudioSourceGain(AudioSourceID, AudioAlterGain);
	OpenALSoft::Instance().ChangeLowpassFilter(AudioSourceID, TargetFilterGain, TargetFilterGain);

	PreRayedActor = nullptr;
	TemporaryWorld = nullptr;
}

void URookAudioController::CheckAudioType() 
{
	switch (AudioSourceModel.AudioBus) 
	{
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

bool URookAudioController::HasPlayLimitReached()
{
	uint8 TemporaryLimit = 0;
	
	if (AudioModels.Num() > 0) 
	{
		for (TPair< uint32, FAudioSourceModel >& AudioModel : AudioModels)
		{
			if (AudioModel.Value.AudioState == EAudioState::Playing) { ++TemporaryLimit; }				
		}
	}
	return TemporaryLimit >= AudioSourceModel.PlayLimit;
}

void URookAudioController::SetUpMultichannelSource(const TWeakObjectPtr<class AActor> Parent) 
{
	if (!Parent.IsValid()) { return; }		
	
	AudioSourceModel.ParentID = Parent->GetUniqueID();
	const TWeakObjectPtr<class USoundWave> TemporaryAsset(GetMultichannelAudioSource());
	AudioSourceModel.CurrentAudioSourceAsset = TemporaryAsset;
	TemporaryAviableAudioSources.Empty();

	if (TemporaryAsset.IsValid() == false) { return; }
	
	if (TemporaryAsset->NumChannels == 1) 
	{
		UE_LOG(RookLog, Warning, TEXT("While trying to Play Multichannel audio asset %s was Mono. Will not play"), *TemporaryAsset->GetName());
		return;
	}
		
	TWeakObjectPtr<UAudioComponent> NewAudioComp = NewObject<UAudioComponent>(Parent.Get());
	NewAudioComp->OnAudioFinishedNative.AddUObject(this, &URookAudioController::MultichannelFinishedPlaying);
	NewAudioComp->SetSound(TemporaryAsset.Get());

	NewAudioComp->ComponentTags.Add(TemporaryTag);

	if (AudioSourceModel.bUseRandomPitch) 
	{
		NewAudioComp->PitchModulationMin = AudioSourceModel.BottomRandomPitchValue;
		NewAudioComp->PitchModulationMax = AudioSourceModel.TopRandomPitchValue;
	}

	if (AudioSourceModel.AudioSourceEAX != EEAX::None) 
	{
		NewAudioComp->bReverb = true;
		RookUtils::Instance().SetReverbInUnreal(AudioSourceModel.AudioSourceEAX);
	}

	CheckMultichannelFading(NewAudioComp);

	float TemporaryVolume = 1.0f;
		
	if (MultichannelFadeHelper.Contains(NewAudioComp))
	{
		TemporaryVolume = 0.0f;
		MultichannelFadeHelper[NewAudioComp].InternalVolume = RookUtils::Instance().DecibelsToVolume(BeginingDecbiles) * ApplicationVolumeMultiplier * 0.01f;
	}
	else 
	{
		TemporaryVolume = RookUtils::Instance().DecibelsToVolume(BeginingDecbiles) * ApplicationVolumeMultiplier * 0.01f;
	}

	NewAudioComp->SetVolumeMultiplier(TemporaryVolume);
	NewAudioComp->Play();
	MultichannelComponents.Add(NewAudioComp);
	AudioSourceModel.IndividualFadeCurve = nullptr;	
}

TWeakObjectPtr<class USoundWave> URookAudioController::GetMultichannelAudioSource() 
{
	for (FMultichannelAudioModel MultichannelAudioModel : AudioSourceModel.MultiChannelAssets) 
	{
		if (MultichannelAudioModel.AudioAsset) { TemporaryAviableAudioSources.AddUnique(MultichannelAudioModel.AudioAsset); }			
	}
	
	TWeakObjectPtr<class USoundWave> TemporaryMultichannelAudioAsset(nullptr);
	BeginingDecbiles = -100.0f;
	
	switch (AudioSourceModel.PlaybackOption) 
	{
		case EPlayback::Single:
		case EPlayback::Loop:
			TemporaryMultichannelAudioAsset = AudioSourceModel.MultiChannelAssets[0].AudioAsset;
			BeginingDecbiles = AudioSourceModel.MultiChannelAssets[0].Decibels;
			if (AudioSourceModel.MultiChannelAssets[0].FadeCurve)
			{
				AudioSourceModel.IndividualFadeCurve = AudioSourceModel.MultiChannelAssets[0].FadeCurve;
			}				
			break;
		case EPlayback::Random:
		case EPlayback::SingleRandom:
			TemporaryMultichannelAudioAsset = GetRandomAudioSource();
			BeginingDecbiles = AudioSourceModel.MultiChannelAssets[RandomIndicis[AudioSourceModel.ParentID]].Decibels;
			if (AudioSourceModel.MultiChannelAssets[RandomIndicis[AudioSourceModel.ParentID]].FadeCurve)
			{
				AudioSourceModel.IndividualFadeCurve = AudioSourceModel.MultiChannelAssets[RandomIndicis[AudioSourceModel.ParentID]].FadeCurve;
			}
			break;
		case EPlayback::Sequence:
		case EPlayback::SingleSequence:
			TemporaryMultichannelAudioAsset = GetSequenceAudioSource();
			BeginingDecbiles = AudioSourceModel.MultiChannelAssets[SequenceIndicis[AudioSourceModel.ParentID]].Decibels;
			if (AudioSourceModel.MultiChannelAssets[SequenceIndicis[AudioSourceModel.ParentID]].FadeCurve)
			{
				AudioSourceModel.IndividualFadeCurve = AudioSourceModel.MultiChannelAssets[SequenceIndicis[AudioSourceModel.ParentID]].FadeCurve;
			}
			break;
	}
	return TemporaryMultichannelAudioAsset;
}

void URookAudioController::CheckMultichannelFading(const TWeakObjectPtr<class UAudioComponent> UnrealAudioComponent) 
{
	TWeakObjectPtr<class UCurveVector>	TemporaryVectorFadeCurve(nullptr);
	
	if (AudioSourceModel.IndividualFadeCurve.IsValid()) 
	{
		TemporaryVectorFadeCurve = AudioSourceModel.IndividualFadeCurve;
	}
	else if (AudioSourceModel.FadeCurve) 
	{
		TemporaryVectorFadeCurve = AudioSourceModel.FadeCurve;
	}

	if (TemporaryVectorFadeCurve.IsValid()) 
	{
		FMultichannelFadeModel FadeModel;
		
		if (TemporaryVectorFadeCurve->FloatCurves[0].GetLastKey().Time > 0.0f)
		{
			FadeModel.FadeFactor = 0.0f;
			FadeModel.FadeTime = 0.0f;
			FadeModel.bFadeIn = true;
		}

		if (TemporaryVectorFadeCurve->FloatCurves[1].GetLastKey().Time > 0.0f) 
		{
			FadeModel.StartFadeOutAt = (AudioSourceModel.CurrentAudioSourceAsset->GetDuration() * UnrealAudioComponent->PitchMultiplier) - TemporaryVectorFadeCurve->FloatCurves[1].GetLastKey().Time;
			
			if (FadeModel.StartFadeOutAt < 0.0f) { FadeModel.StartFadeOutAt = 0.0f; }				
		}
		FadeModel.FadeCurve = TemporaryVectorFadeCurve;
		MultichannelFadeHelper.Add(UnrealAudioComponent, FadeModel);
	}
}

void URookAudioController::MultichannelFinishedPlaying(UAudioComponent* UnrealAudioComponent) 
{
	const TWeakObjectPtr<AActor> Parent(Cast<AActor>(UnrealAudioComponent->GetOuter()));
	MultichannelComponents.Remove(UnrealAudioComponent);
	MultichannelFadeHelper.Remove(UnrealAudioComponent);
	MultichannelFadeHelper.Compact();
	AudioSourceModel.IndividualFadeCurve = nullptr;
	FinishPlaying.Broadcast(UnrealAudioComponent->ComponentTags[0]);
	UnrealAudioComponent->ConditionalBeginDestroy();

	switch (AudioSourceModel.PlaybackOption) 
	{
		case EPlayback::Loop:
			FinishLoop.Broadcast(UnrealAudioComponent->ComponentTags[0]);
			Play(Parent);
			break;
		case EPlayback::Random:
			Play(Parent);
			break;
		case EPlayback::Sequence:
			FinishSequence.Broadcast(UnrealAudioComponent->ComponentTags[0]);
			Play(Parent);
			break;
	}
}

void URookAudioController::OnEndPlay() 
{
	Stop();
}

void URookAudioController::EAXOverlap(const uint32 ActorID, const EEAX EAX) 
{
	for (TPair< uint32, FAudioSourceModel >& AudioModel : AudioModels)
	{
		if (AudioModel.Value.ParentID == ActorID)
		{
			if (AudioModel.Value.AudioType == EAudioType::is3D)
			{
				OpenALSoft::Instance().SetEAXOnAudioSource(EAX, AudioModel.Key);
			}
			else 
			{
				RookUtils::Instance().SetReverbInUnreal(EAX);
			}
			break;
		}
	}
}

bool URookAudioController::HasSameAudioModel( const FAudioSourceModel& modelToCheck )
{
	return false;
}

URookAudioController::~URookAudioController() 
{
	if (AudioModels.Num() > 0) 
	{
		for (TPair< uint32, FAudioSourceModel >& AudioModel : AudioModels)
		{
			OpenALSoft::Instance().RemoveAudioSource(AudioModel.Key);
		}			
		
		AudioModels.Empty();
	}

	if (MultichannelComponents.Num() > 0) 
	{
		for (TWeakObjectPtr<UAudioComponent> UnrealAudioComponent : MultichannelComponents) 
		{
			if (UnrealAudioComponent.IsValid()) { UnrealAudioComponent->ConditionalBeginDestroy(); }				
		}
		MultichannelComponents.Empty();
	}
	
	MultichannelFadeHelper.Empty();

	if (RookInterface) 
	{
		RookInterface->OnEndPlay.Remove(EndPlayHandle);
		RookInterface->OnEAXOverlap.Remove(EAXOverlapHandle);
	}

	if (FailedToPlayAudioModel.Num() > 0)
		FailedToPlayAudioModel.Empty();
	
	RookInterface = nullptr;
	ActiveListenerController = nullptr;
	DataLoader = nullptr;
}