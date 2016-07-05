#pragma once
#include "Components/SceneComponent.h"
#include "RookEnums.h"
#include "RookStructs.h"
#include "RookListenerController.generated.h"

UCLASS( ShowCategories = ( Mobility ), ClassGroup = Audio, meta = ( BlueprintSpawnableComponent ), Blueprintable )
class ROOK_API URookListenerController : public USceneComponent {
	GENERATED_BODY()
public:
	URookListenerController();
	virtual ~URookListenerController();

	virtual void					TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	virtual void					InitializeComponent() override;
	virtual void					UninitializeComponent() override;
	void							SetListenerActiveState( bool bActive );	
	void							ChangeListenerType( EListenerType NewListenerType );
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rook Listener")
	TWeakObjectPtr<class AActor>					ActorToFollow = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rook Listener")
	TWeakObjectPtr<class APlayerCameraManager>		CameraToFollow = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rook Listener")
	FSocketAttachment								SocketAttachment;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rook Listener")
	bool											bFreezRotation = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rook Listener")
	FFreezRotation									FreezRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rook Listener")
	bool											bUseVelocity = false;
private:
	EListenerType					ListenerType = EListenerType::FollowCamera;
	TWeakObjectPtr<class AActor>	SocketActor = nullptr;
private:
	void							UpdateListenerPosition( TWeakObjectPtr<class AActor> Actor );
	bool							IsFollowerValid() const;
	void							ValidatedSocketActor();
};