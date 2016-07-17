#pragma once
#include "Runtime/Engine/Classes/Engine/EngineBaseTypes.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "RookEnums.h"
#include "RookStructs.h"
#include "RookListenerController.generated.h"

UCLASS( ShowCategories = (Mobility), ClassGroup = Audio, Blueprintable )
class ROOK_API URookListenerController : public UObject, public FTickableGameObject  {
	GENERATED_BODY()
public:
	URookListenerController();
	virtual ~URookListenerController();

	virtual bool					IsTickable() const override;
	virtual TStatId					GetStatId() const override;
	virtual void					Tick( float DeltaTime ) override;
	void							SetListenerActiveState( bool bActive );	
	void							ChangeListenerType( EListenerType NewListenerType );
	FVector							GetListenerLocation() const;
	bool							IsActive() const;
public:
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	TWeakObjectPtr<class AActor>					ActorToFollow = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	TWeakObjectPtr<class APlayerCameraManager>		CameraToFollow = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	FSocketAttachment								SocketAttachment;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	bool											bFreezRotation = false;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	FFreezRotation									FreezRotation;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	bool											bUseVelocity = false;
private:
	bool							IsTicking = true;
	EListenerType					ListenerType = EListenerType::FollowCamera;
	TWeakObjectPtr<class AActor>	SocketActor = nullptr;
	bool							bActive = false;
private:
	void							CheckInitListenerType();
	void							UpdateListenerLocation( TWeakObjectPtr<class AActor> Actor );
	bool							IsFollowerValid() const;
	void							ValidatedSocketActor();
};