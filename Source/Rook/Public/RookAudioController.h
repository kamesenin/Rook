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

	void					Play( TWeakObjectPtr<class AActor> Parent );
	void					Pause();
	void					Stop();
	void					ChangeSurface( TEnumAsByte<EPhysicalSurface> NewSurface );
	void					ChangeEAX( EEAX NewEAX );
public:
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Audio" )
	FAudioSourceModel								AudioSourceModel;
	bool											bUseDebugSpheres = false;
private:
	bool											HasActiveListenerController();
	void											GetActiveListenerController();
	void											GetAudioMaxiumDistance();
	void											SetUpNewMonoAudio( TWeakObjectPtr<class AActor> Parent );
	bool											ShouldUnPause( uint32 ParentID );
	void											CheckIfBufferHasAudioData();
	void											CheckDataLoader();
	TWeakObjectPtr<class USoundWave>				GetMonoAudioSource( uint32 AudioSourceID );
	TWeakObjectPtr<class USoundWave>				GetRandomMonoAudioSource( uint32 AudioSourceID );
	TWeakObjectPtr<class USoundWave>				GetSequenceMonoAudioSource( uint32 AudioSourceID );
	void											CheckIfSourceFinishedPlaying( FAudioSourceModel AudioSourceModel );
	void											AfterFinishedPlaying( TWeakObjectPtr<class AActor> Parent, FAudioSourceModel AudioSourceModel );
	void											UpdateLocation( TWeakObjectPtr<class AActor> Parent, FAudioSourceModel AudioSourceModel );
	bool											HasAudioModelCurrentSurface();
	void											RaytraceToListener( TWeakObjectPtr<class AActor> Parent, uint32 AudioSourceID );
private:
	UPROPERTY()
	TMap<uint32, FAudioSourceModel>					AudioSources;
	UPROPERTY()
	TWeakObjectPtr<class URookListenerController>	ActiveListenerController = nullptr;
	float											MaxiumDistanceToListener = 0.0f;	
	UPROPERTY()
	TWeakObjectPtr<class URookAudioDataLoader>		DataLoader = nullptr;
	uint16											LastRandomIndex = 0;
	uint16											LastSequenceIndex = 0;	
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface>					CurrentAudioControllerSurafce = EPhysicalSurface::SurfaceType_Default;
	UPROPERTY()
	EEAX											CurrentAudioControllerEAX = EEAX::None;
};