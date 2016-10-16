/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#pragma once
#include "Runtime/Engine/Classes/Engine/EngineBaseTypes.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "RookEnums.h"
#include "RookStructs.h"
#include "RookListenerController.generated.h"

UCLASS( ShowCategories = (Mobility), ClassGroup = Audio, Blueprintable )
class ROOK_API URookListenerController : public UObject, public FTickableGameObject {
	GENERATED_BODY()
public:
	URookListenerController();
	virtual ~URookListenerController();

	virtual bool					IsTickable() const override;
	virtual TStatId					GetStatId() const override;
	virtual void					Tick( float DeltaTime ) override;
	/**
	Function setting current audio controller to be active or not
	@param bActive - if true controller not only be active, but also will be ticking and vice versa
	*/
	void							SetListenerActiveState( const bool bActive );
	/**
	Function changes listener type based on enum: FollowCamera, FollowActor, FollowMeshSocket
	@param NewListenerType - Rook enum
	*/
	void							ChangeListenerType( const EListenerType NewListenerType );
	/**
	Helper function for other classes to get current listener controller location vector
	@return FVector with current location
	*/
	FVector							GetListenerLocation();
	/**
	Helper function for other classes
	@return current active state
	*/
	bool							IsActive() const;
public:
	/** Sets an Actor which Listener Controller will follow */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	TWeakObjectPtr<class AActor>					ActorToFollow = nullptr;
	/** Sets a Camera which Listener Controller will follow */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	TWeakObjectPtr<class APlayerCameraManager>		CameraToFollow = nullptr;
	/** Sets a Socket which Listener Controller will attach to. To have proper attachment not only socket has to be provident, but also character which has it. */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	FSocketAttachment								SocketAttachment;
	/** Should Listener Controller have frozen rotation. If so it will use FFreezRotation struct */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	bool											bFreezRotation = false;
	/** Struct for frozen rotation. It can freeze separate axies (X, Y, Z) with given rotation FVector */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	FFreezRotation									FreezRotation;
	/** Should Listener Controller sends its velocity. It affects output of the audio */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook Listener" )
	bool											bUseVelocity = false;
private:
	/** Helper boolean. If controller is not active it will not tick */
	bool							IsTicking = true;
	/** Listener type. As a default it is following camera, however it can by set to follow actor or specific socket on mesh */
	EListenerType					ListenerType = EListenerType::FollowCamera;
	/** Helper actor. If listener controller is set to attach to socket we are creating temporary actor, add it to socket and get its position and rotation */
	TWeakObjectPtr<class AActor>	SocketActor = nullptr;
	/** Helper boolean. Set to true when Listener Controller is registerd in Rook */
	bool							bWasRegistered = false;
	/** Shared Pointer to Rook interface - used to register Listener Controller */
	TSharedPtr<class IRook>			RookInterface = nullptr;
	/** Helper boolean. Checks if EndPlay deleagate has been added */
	bool							bHasEndPlayDeleagate = false;
private:
	/** Function will check if there is actor, camera or socket set and if so it will change ListenerType */
	void							CheckInitListenerType();
	/** Function will grab current attachment location, velocity (if used), forward and up vector and send it to OpenAL Soft */
	void							UpdateListenerLocation(const TWeakObjectPtr<class AActor> Actor);
	/**
	Function check if current actor to follow is valid
	@return true if it is
	*/
	bool							IsFollowerValid();
	/** Function validates custom SocketActor. If its invalid it will create new one and attach to socket */
	void							ValidatedSocketActor();	
	/** Registers Listener Controller in Rook */
	void							RegisterListener();
	/** Helper function for getting defualt camera if there is no actor/camera/socket to follow */
	void							GetDefaultCamera();
	/** Helper function - it's needed when Listener Controller stops audio when EndPlay */
	UFUNCTION()
	void							OnEndPlay( AActor* Actor, EEndPlayReason::Type EndPlayReason );

	FVector							LastListenerLocation;
	FVector							LastListenerForwardVector;
	FVector							LastListenerUpVector;
};