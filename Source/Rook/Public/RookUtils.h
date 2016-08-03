/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#pragma once
#include "RookEnums.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "OpenALSoft/include/AL/efx-presets.h"
#include "OpenALSoft/include/AL/efx-creative.h"

class RookUtils {
public:
	static RookUtils&	Instance();
	~RookUtils() {};
	/**
	Helper function. Checks if given terget location is in sphere radius of source locaction. Cheaper than performing FVector DistanceTo
	@param SourceLocatiob - from where check should start
	@param TargetLocation - location wich can be in sphere - usual an Actor location
	@param Radius
	@return true if TargetLocation is in shpere radius
	*/
	bool				InSpehereRadius(FVector SourceLocation, FVector TargetLocation, float Radius) const;
	/**
	Helper function. Returns current world with give world type - by default its Game, however it can be PIE, Editor, Preview and Inactive
	@param WorldType - Game, PIE, Editor, Preview and Inactive
	@return UWorld if found
	*/
	class UWorld*		GetWorld( TEnumAsByte<EWorldType::Type> WorldType = EWorldType::Game ) const;
	/**
	Helper function. Returns new Rook internal unique ID - usefule while we don't want to create UObject
	@return Unique ID
	*/
	uint32				GetUniqueID();
	/**
	Function calculate material Sound Transmission Class
	@param MeshDepth 
	@param MeshSize
	@return STC factor
	*/
	float				CalculateSTC( float MeshDepth, float MeshSize );
	/**
	Function return avarage material Sound Absorption Coefficient
	@param MeshDepth
	@return SAC factor
	*/
	float				AvarageSAC( float MeshDepth );
	/**
	Function sets EAX reverb effect in on Unreal Audio Componets - if off cource they are having bReverb flag set to true
	@param EEAX - Rook EAX enum
	*/
	void				SetReverbInUnreal( EEAX NewReverb );
	/**
	Function converts decibels to volume - range from 0.0f to 100.0f
	@param dB - decibels in range of 0.0f to -100.0f
	@return volume value
	*/
	float				DecibelsToVolume( float dB );
	/**
	Function converts volume to decibles - range from 0.0f to -100.0f
	@param Volume - in range 0.0f to 100.0f
	@return decibels
	*/
	float				VolumeToDecibels( float Volume );
	/** Cleans data on Utils */
	void				CleanData();
	/** Function will trigger log of current audio pool use in  OpenAL Soft */
	void				LogCurrentAudioPool();
public:
	/** TMap of OpenAL Soft EAX Reverb properties - key is Rook enum, value is EAX Reverb properites*/
	TMap<EEAX, EFXEAXREVERBPROPERTIES>  EAXReverb;
	/** TMap of Sound Absorption Coefficient. Key is physicial material name (for ex. PHM_Glass) and value is its SAC value */
	TMap<FString, float>				SAC;
	/** TMap of Sound Transmission Class. Key is physicial material name (for ex. PHM_Glass) and value is its STC value */
	TMap<FString, uint8>				STC;
private:
	/** Function polulate TMap of EAX Reverb */
	void				SetUpEAXReverbMap();
	/** Function populate TMap of SAC and STC */
	void				SetUpAcusticsMap();
	/** Function removes previouse Reverb for Unreal Audio Components */
	void				RemoveUnrealReverb();	
	static RookUtils	Utils;
	RookUtils();
	RookUtils( const RookUtils& );
	RookUtils& operator= ( const RookUtils& );
private:
	/** Holdes current Rook Uniqe ID */
	uint32				CurrentUniqeID = 0;
	/** Holdes previouse Unreal EAX Reverb - helps to remove it in Unreal */
	uint8				PreviouseUnrealReverb;
	/** TWeakObjectPtr to Unreal Reverb Effect so we don't need to create it over and over */
	UPROPERTY()	
	TWeakObjectPtr<class UReverbEffect> UnrealReverb = nullptr;
};