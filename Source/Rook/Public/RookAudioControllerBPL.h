/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RookAudioControllerBPL.generated.h"

UCLASS()
class ROOK_API URookAudioControllerBPL : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	/**
	Plays Rook Audio
	@param AudioBlueprint - Rook Audio blueprint which will be played
	@param Parent - on which actor audio will be played
	@param Tag - helper FName for tracing a delegate on Finish
	@return true if AudioBlueprint and Parent are valid
	*/
	UFUNCTION(BlueprintCallable, Category = "Rook|Audio")
	static class URookAudioController*				Play( TSubclassOf<class URookAudioController> AudioBlueprint, AActor* Parent, FName Tag = "" );
	/**
	Stops Rook Audio
	@param AudioBlueprint - Rook Audio blueprint which will be stoped
	@return true if AudioBlueprint is valid
	*/
	UFUNCTION(BlueprintCallable, Category = "Rook|Audio")
	static bool				Stop( TSubclassOf<class URookAudioController> AudioBlueprint );
	/**
	Pauses Rook Audio
	@param AudioBlueprint - Rook Audio blueprint which will be paused
	@return true if AudioBlueprint is valid
	*/
	UFUNCTION(BlueprintCallable, Category = "Rook|Audio")
	static bool				Pause( TSubclassOf<class URookAudioController> AudioBlueprint );
	/**
	Changes physical surface which will be used
	@param AudioBlueprint - Rook Audio blueprint on which change will be perform
	@NewSurface - Unreal Physical Surface - can be set in project settings 
	@return true if AudioBlueprint is valid
	*/
	UFUNCTION(BlueprintCallable, Category = "Rook|Audio")
	static bool				ChangeSurfaceType( TSubclassOf<class URookAudioController> AudioBlueprint, TEnumAsByte<EPhysicalSurface> NewSurface );
	/**
	Changes EAX reverb effect
	@param AudioBlueprint - Rook Audio blueprint on which change will be perform
	@NewEAX - EAX reverb Enum - will be used on 3D and multichannel audio
	@return true if AudioBlueprint is valid
	*/
	UFUNCTION(BlueprintCallable, Category = "Rook|Audio")
	static bool				ChangeEAXType( TSubclassOf<class URookAudioController> AudioBlueprint, EEAX NewEAX );
	/**
	Enables/Disables debug sphere on 3D audio - only in editor! 
	@param AudioBlueprint - Rook Audio blueprint on which change will be perform
	@UseDebuLines - Rook enum: Enable/Disable
	@return true if AudioBlueprint is valid
	*/
	UFUNCTION(BlueprintCallable, Category = "Rook|Audio")
	static bool				DebugLines( TSubclassOf<class URookAudioController> AudioBlueprint, EEnableState UseDebuLines );
	/**
	Changes desibels values for currently playing multichannel audio - range should be from 0.0f to -100.0f 
	@param AudioBlueprint - Rook Audio blueprint on which change will be perform
	@param dB - decibels
	@return true if AudioBlueprint is valid
	*/
	UFUNCTION(BlueprintCallable, Category = "Rook|Audio")
	static bool				ChangeDecibelsOnActiveMultichannel( TSubclassOf<class URookAudioController> AudioBlueprint,  float dB );
};