/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RookListenerControllerBPL.generated.h"

UCLASS()
class ROOK_API URookListenerControllerBPL : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	/**
	Set given listener to be active. Rest of listeners will be disactive
	@param ListenerBlueprint - listener which will be active
	@return true if ListenerBlueprint is valid
	*/
	UFUNCTION( BlueprintCallable, Category = "Rook|Listener" )
	static bool				SetActiveListener( TSubclassOf<class URookListenerController> ListenerBlueprint );
	/**
	Set new listener type on give listener
	@param ListenerBlueprint - listener on which change will be set
	@param NewListenerType - Rook Enum for listener type
	@return true if ListenerBlueprint is valid
	*/
	UFUNCTION( BlueprintCallable, Category = "Rook|Listener" )
	static bool				SetListenerType( TSubclassOf<class URookListenerController> ListenerBlueprint, EListenerType NewListenerType );
	/**
	Set actor to follow by listener
	@param ListenerBlueprint - listener on which change will be set
	@param NewActor - actor which listener will follow
	@return true if ListenerBlueprint is valid
	*/
	UFUNCTION( BlueprintCallable, Category = "Rook|Listener" )
	static bool				SetActorOnListener( TSubclassOf<class URookListenerController> ListenerBlueprint, class AActor* NewActor );
	/**
	Set socket to follow by listener
	@param ListenerBlueprint - listener on which change will be set
	@param NewSocketAttachment - socket which listener will follow
	@return true if ListenerBlueprint is valid
	*/
	UFUNCTION( BlueprintCallable, Category = "Rook|Listener" )
	static bool				SetSocketOnListener( TSubclassOf<class URookListenerController> ListenerBlueprint, FSocketAttachment NewSocketAttachment );
	/**
	Set camera to follow by listener
	@param ListenerBlueprint - listener on which change will be set
	@param NewCamera - camera which listener will follow
	@return true if ListenerBlueprint is valid
	*/
	UFUNCTION( BlueprintCallable, Category = "Rook|Listener" )
	static bool				SetCameraOnListener( TSubclassOf<class URookListenerController> ListenerBlueprint, class APlayerCameraManager* NewCamera );
	/**
	Set new freez roataion on listener
	@param ListenerBlueprint - listener on which change will be set
	@param bFreezRotation - should listener use freez rotation
	@param NewFreezRotation - struct for setting roation freez on axies
	@return true if ListenerBlueprint is valid
	*/
	UFUNCTION( BlueprintCallable, Category = "Rook|Listener" )
	static bool				SetFreezRotationOnListener( TSubclassOf<class URookListenerController> ListenerBlueprint, bool bFreezRotation, FFreezRotation NewFreezRotation );
	/**
	Set if listener should be affected by velocity
	@param ListenerBlueprint - listener on which change will be set
	@param bShouldUseVelocity - should listener use velocity
	@return true if ListenerBlueprint is valid
	*/
	UFUNCTION( BlueprintCallable, Category = "Rook|Listener" )
	static bool				ShouldListenerUseVelocity( TSubclassOf<class URookListenerController> ListenerBlueprint, bool bShouldUseVelocity );
};