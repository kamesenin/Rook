/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#pragma once
#include "RookEnums.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "OpenALSoft/include/AL/efx-presets.h"
#include "OpenALSoft/include/AL/efx-creative.h"

class RookUtils
{
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
	inline bool InSpehereRadius( FVector SourceLocation, FVector TargetLocation, float Radius ) const 
	{
		return ( FMath::Pow( ( SourceLocation.X - TargetLocation.X ), 2.0f ) + FMath::Pow( ( SourceLocation.Y - TargetLocation.Y ), 2.0f ) + FMath::Pow( ( SourceLocation.Z - TargetLocation.Z ), 2.0f ) ) <= Radius * Radius;
	}
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
	inline uint32 GetUniqueID() 
	{
		return ++CurrentUniqeID;
	}
	/**
	Function calculate material Sound Transmission Class
	@param MeshDepth 
	@param MeshSize
	@return STC factor
	*/
	inline float CalculateSTC( float MeshDepth, float MeshSize ) 
	{
		return 13.4f + 11.4f * ( log10( MeshSize ) ) + 0.0826f * MeshDepth;
	}
	/**
	Function return avarage material Sound Absorption Coefficient
	@param MeshDepth
	@return SAC factor
	*/
	inline float AvarageSAC( float MeshDepth ) 
	{
		return 0.073f + 0.0001f * MeshDepth;
	}
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
	inline float DecibelsToVolume( float dB ) 
	{
		return FMath::Pow( 10, dB / 20.0f ) * 100.0f;
	}
	/**
	Function converts volume to decibles - range from 0.0f to -100.0f
	@param Volume - in range 0.0f to 100.0f
	@return decibels
	*/
	inline float VolumeToDecibels( float Volume ) 
	{
		return 20.0f * FMath::LogX( 10.0f, Volume );
	}
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