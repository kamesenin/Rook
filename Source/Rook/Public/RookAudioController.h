#pragma once
#include "Runtime/Engine/Classes/Engine/EngineBaseTypes.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "RookEnums.h"
#include "RookStructs.h"
#include "RookAudioController.generated.h"

UCLASS( ShowCategories = ( Mobility ), ClassGroup = Audio, Blueprintable )
class ROOK_API URookAudioController : public UObject, public FTickableGameObject {
	GENERATED_BODY()
public:
	URookAudioController();
	virtual ~URookAudioController();

	virtual bool			IsTickable() const override;
	virtual TStatId			GetStatId() const override;
	virtual void			Tick( float DeltaTime ) override;
	/** Function for playing audio
	@param Parent - weak pointer to parent actor - its needed for positioning 
	*/
	void					Play( const TWeakObjectPtr<class AActor> Parent );
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
public:
	/** Data model for audio source(s) */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	FAudioSourceModel								AudioSourceModel;
	/** Helper boolean for showing debug sphere. Blue is for audio gain, yellow for lowpass, green for bandpass */
	bool											bUseDebugSpheres = false;
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
	@param AudioSourceID - helps to check if audio source is in AudioSources array and what kind of Playback enum it has
	@return weak pointer to audio asset
	*/
	TWeakObjectPtr<class USoundWave>				GetMonoAudioSource( const uint32 AudioSourceID );
	/**
	Function selects random asset from mono array. It prevents to repeat the same random choice as previously
	@return weak pointer to audio asset
	*/
	TWeakObjectPtr<class USoundWave>				GetRandomMonoAudioSource();
	/**
	Function selects gets next audio asset from array in sequence
	@return weak pointer to audio asset
	*/
	TWeakObjectPtr<class USoundWave>				GetSequenceMonoAudioSource();
	/**
	Helper function for checking if mono audio source from model finished playing, if so changes it state.
	@param AudioSourceModel - helps to get and change current state, getting mono audio source and audio source id
	*/
	void											CheckIfSourceFinishedPlaying( FAudioSourceModel AudioSourceModel );
	/** 
	Function removes entry in AudioSource TMap, removes audio source in OpenAl. Additionally if audio Playback is Random or Sequence it will play next audio source
	@param Parent - is used when we want to play next track in Random or Sequence
	@param AudioSourceModel - helps to set up new audio source and remove old one
	*/
	void											AfterFinishedPlaying( const TWeakObjectPtr<class AActor> Parent, const FAudioSourceModel AudioSourceModel );
	/**
	Update location of audio source based on parent position, velocity, audio gain, forward and up vector. Additionally in editor it can show debug spheres
	@param Parent - weak pointer to parent actor
	@param AudioSourceModel - helps to send information to OpenAL Soft
	*/
	void											UpdateLocation( const TWeakObjectPtr<class AActor> Parent, FAudioSourceModel AudioSourceModel );
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
private:
	/** TMap of current audio sources. Key is unique id of audio source, value is data model of it */
	UPROPERTY()
	TMap<uint32, FAudioSourceModel>					AudioSources;
	/** Weak pointer to current active listener controller */
	UPROPERTY()
	TWeakObjectPtr<class URookListenerController>	ActiveListenerController = nullptr;
	/** Maxiumu distance from source to listener based on volume float curve */
	float											MaxiumDistanceToListener = 0.0f;	
	/** Weak pointer to data loader. Helps while we need to load new asset data */
	UPROPERTY()
	TWeakObjectPtr<class URookAudioDataLoader>		DataLoader = nullptr;
	/** Helper to track last random audio source */
	uint16											LastRandomIndex = 0;
	/** Helper to track last sequence audio source */
	uint16											LastSequenceIndex = 0;	
	/** Current physical surface. Its Unreal enum */
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface>					CurrentAudioControllerSurafce = EPhysicalSurface::SurfaceType_Default;
	/** Current EAX enum */
	UPROPERTY()
	EEAX											CurrentAudioControllerEAX = EEAX::None;
	/** Helper temporary array of audio assets. Used while setting new audio source */
	UPROPERTY()
	TArray<TWeakObjectPtr<class USoundWave>>		TemporaryAviableAudioSources;
};