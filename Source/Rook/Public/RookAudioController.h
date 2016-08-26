/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#pragma once
#include "Runtime/Engine/Classes/Engine/EngineBaseTypes.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "RookEnums.h"
#include "RookStructs.h"
#include "RookAudioController.generated.h"

UCLASS( ShowCategories = ( Mobility ), ClassGroup = Audio, Blueprintable )
class ROOK_API URookAudioController : public UObject, public FTickableGameObject {
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FRookDelegate, FName, TagName );
public:
	URookAudioController();
	virtual ~URookAudioController();

	virtual bool			IsTickable() const override;
	virtual TStatId			GetStatId() const override;
	virtual void			Tick( float DeltaTime ) override;
	/** Function for playing audio
	@param Parent - weak pointer to parent actor - its needed for positioning 
	*/
	void					Play( const TWeakObjectPtr<class AActor> Parent, FName Tag = "" );
	/** Pauses/Unpauses audio */
	void					Pause();
	/** Stops audio */
	void					Stop();
	/** 
	Changes current physical surface so proper audio source can be selected to play. Physical surface is very often use for ex. footsteps
	Additionally it reset LastRandomIndex and LastSequenceIndex.
	*/
	void					ChangeSurface( const TEnumAsByte<EPhysicalSurface> NewSurface );
	/** Changes EAX reverb for every audio source which is playing or will be played */
	void					ChangeEAX( const EEAX NewEAX );
	/** Changes desibels values for currently playing multichannel audio - range should be from 0.0f to -100.0f */
	void					ChangeDecibelsOnActiveMultichannel( const float dB );
public:
	/** Data model for audio source(s) */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	FAudioSourceModel								AudioSourceModel;
	/** Helper boolean for showing debug sphere. Blue is for audio gain, yellow for lowpass, green for bandpass */
	bool											bUseDebugSpheres = false;
	/** Delegate on playback finish */
	UPROPERTY( EditAnywhere, BlueprintAssignable, Category = "Rook Audio" )
	FRookDelegate									FinishPlaying;
	/** Delegate when loop is done */
	UPROPERTY( EditAnywhere, BlueprintAssignable, Category = "Rook Audio" )
	FRookDelegate									FinishLoop;
	/** Delegate when sequence is done */
	UPROPERTY( EditAnywhere, BlueprintAssignable, Category = "Rook Audio" )
	FRookDelegate									FinishSequence;
private:
	/** 
	Check if current audio controller has active audio listener controller 
	@return true if it has one
	*/
	bool											HasActiveListenerController();
	/** Function grabes current active audio listener controller */
	void											GetActiveListenerController();
	/** Check maximum distance based on float curve for audio gain - last key from curve */
	void											GetAudioMaxiumDistance();
	/** 
	Function adds data to AudioSourceModel like: parent position, new unique id, parent id, current gain, random pitch (if used) and EAX (if used). 
	Sends this model to OpenAL Soft to create real audio source (OpenAL one) with given data.
	@param Parent - weak object pointer to parent actor. Needed for parent id and position.
	*/
	void											SetUpNewMonoAudio( const TWeakObjectPtr<class AActor> Parent );
	/**
	While calling Play, function checks if audio source was paused.
	@param ParentID - helps determine if audio source which we want to play has the same parent
	@return true if source was paused
	*/
	bool											ShouldUnPause( const uint32 ParentID );
	/** Function iterates through Mono audio sources and checks if they are in buffers TMap. If not it starts loading process. */
	void											CheckIfBufferHasAudioData();
	/** Check if weak pointer to data loader class is valid, if not looks or creates new one. */
	void											CheckDataLoader();
	/**
	Based on audio playback enum return proper audio asset.
	@return weak pointer to audio asset
	*/
	TWeakObjectPtr<class USoundWave>				GetMonoAudioSource();
	/**
	Function selects random asset from mono array. It prevents to repeat the same random choice as previously
	@return weak pointer to audio asset
	*/
	TWeakObjectPtr<class USoundWave>				GetRandomAudioSource();
	/**
	Function selects gets next audio asset from array in sequence
	@return weak pointer to audio asset
	*/
	TWeakObjectPtr<class USoundWave>				GetSequenceAudioSource();
	/**
	Helper function for checking if mono audio source from model finished playing, if so changes it state.
	@param AudioModelID - helps to get and change current state, getting mono audio source and audio source id
	*/
	void											CheckIfSourceFinishedPlaying( const uint32 AudioModelID, const float DeltaTime );
	/** 
	Function removes entry in AudioSource TMap, removes audio source in OpenAl. Additionally if audio Playback is Random or Sequence it will play next audio source
	@param Parent - is used when we want to play next track in Random or Sequence
	@param AudioModelID - helps to set up new audio source and remove old one
	*/
	void											AfterFinishedPlaying( const TWeakObjectPtr<class AActor> Parent, const uint32 AudioModelID );
	/**
	Update location of audio source based on parent position, velocity, audio gain, forward and up vector. Additionally in editor it can show debug spheres
	@param Parent - weak pointer to parent actor
	@param AudioModelID - helps to send information to OpenAL Soft
	*/
	void											UpdateLocation( const TWeakObjectPtr<class AActor> Parent, const uint32 AudioModelID );
	/**
	Check if current physical surface is the same as in MonoAudioModel.
	@return if so it return true
	*/
	bool											HasAudioModelCurrentSurface();
	/** 
	Simple raytrace to set up gain and lowpass dynamically. It raytrace from parent source to active listener controller
	@param Parent - weak pointer to parent actor
	@param AudioSourceID - id of audio source from which raytrace begins
	*/
	void											RaytraceToListener( const TWeakObjectPtr<class AActor> Parent, const uint32 AudioSourceID );
	/** Function checks current Audio Bus and sets Audio Type Rook enum. It simplyfiles check without going through audio buses */
	void											CheckAudioType();
	/** 
	Function check if play limit has been reached - if not it will continue setting up audio source 
	@return true if limit has been reached
	*/
	bool											HasPlayLimitReached();
	/** 
	Sets up new multichannel audio source via Unreal Audio Component
	@param Parent - helps to set Outer of Audio Component
	*/
	void											SetUpMultichannelSource( const TWeakObjectPtr<class AActor> Parent );
	/**
	Get multichannel audio asset based on current audio Playback type
	@return audio asset
	*/
	TWeakObjectPtr<class USoundWave>				GetMultichannelAudioSource();
	/**
	Function which is executed when multichannel audio finishes playing. Unreal Audio Component will be destroy and if audio Playback type is Loop, Random or Sequence ther will be Play call
	@param UnrealAudioComponent
	*/
	void											MultichannelFinishedPlaying( UAudioComponent* UnrealAudioComponent );
	/** Helper function - is triggred when OnEndPlay has been delegated */
	UFUNCTION()
	void											OnEndPlay();
	/** Checks if mono audio will be fading */
	void											CheckMonoFading();
	/**
	Perform mono audio fading 
	@param AudioModelID - helps with fading
	*/
	void											PerformMonoFading( const uint32 AudioModelID );
	/** 
	Checks if multichannel should fade
	@param UnrealAudioComponent - helps to add to helper tmap and getting pitch
	*/
	void											CheckMultichannelFading( const TWeakObjectPtr<class UAudioComponent> UnrealAudioComponent );
	/**
	Performs multichannel fade
	@param DeltaTime - helps count time
	*/
	void											PerformMultichannelFading( const float DeltaTime );
	/** Helper function for cleaning unused audio models */
	void											CheckAndRemoveUnusedModels();
private:
	/** TMap of current audio models in use. Key is unique id of audio source, value is data model of it */
	UPROPERTY()
	TMap<uint32, FAudioSourceModel>					AudioModels;
	/** Weak pointer to current active listener controller */
	UPROPERTY()
	TWeakObjectPtr<class URookListenerController>	ActiveListenerController = nullptr;
	/** Maxiumu distance from source to listener based on volume float curve */
	float											MaxiumDistanceToListener = 0.0f;	
	/** Weak pointer to data loader. Helps while we need to load new asset data */
	UPROPERTY()
	TWeakObjectPtr<class URookAudioDataLoader>		DataLoader = nullptr;
	/** Helper to track last random audio source. Key is parent Actor ID ,value is last index */
	TMap<uint32, uint16>							RandomIndicis;
	/** Helper to track last sequence audio source. Key is parent Actor ID ,value is last index */	
	TMap<uint32, uint16>							SequenceIndicis;
	/** Current physical surface. Its Unreal enum */
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface>					CurrentAudioControllerSurafce = EPhysicalSurface::SurfaceType_Default;
	/** Current EAX enum */
	UPROPERTY()
	EEAX											CurrentAudioControllerEAX = EEAX::None;
	/** Helper temporary array of audio assets. Used while setting new audio source */
	TArray<TWeakObjectPtr<class USoundWave>>		TemporaryAviableAudioSources;
	/** Helper value for holding desbiles of multichannel at startup */
	float											BeginingDecbiles = -100.0f;
	/** Array holding current multichannel Unreal Audio Components. */
	UPROPERTY()
	TArray<TWeakObjectPtr<class UAudioComponent>>	MultichannelComponents;
	/** Unreal Volume Multiplier. Volume on 3D souds and dB on multichannel will be multiplie by this value */
	float											ApplicationVolumeMultiplier = 1.0f;
	/** Shared Pointer to Rook interface - used to get active listeners and check enabled flag */
	TSharedPtr<class IRook>							RookInterface = nullptr;
	/** Helper boolean. If 3D audio source failed to play, in next tick controller will try it again - it's offten when audio data has not been loaded yet */
	bool											b3DFailedToPlay = false;
	/** Helper TMap for checking audio sources which didn't play - it's offten when audio data has not been loaded yet */
	UPROPERTY()
	TMap<uint32, bool>								FailedToPlayAudioModel;
	/** Helper handler - helps to remove unesed delegate */
	FDelegateHandle									EndPlayHnadle;	
	/** Helper tmap holding current Unreal Audio Components for fading */
	UPROPERTY()
	TMap<TWeakObjectPtr<class UAudioComponent>, FMultichannelFadeModel>			MultichannelFadeHelper;
	/** Helper array which holdes key for models that will be removed */
	UPROPERTY()
	TArray<uint32>									ModelsToRemove;
	/** Helper for holding a Tag */
	UPROPERTY()
	FName											TemporaryTag;
};