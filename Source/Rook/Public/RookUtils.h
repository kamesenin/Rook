#pragma once
#include "RookEnums.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "OpenALSoft/include/AL/efx-presets.h"
#include "OpenALSoft/include/AL/efx-creative.h"

class RookUtils {
public:
	static RookUtils&	Instance();
	~RookUtils() {};
	bool				InSpehereRadius( FVector SourceLocation, FVector TargetLocation, float Radius ) const;
	class UWorld*		GetWorld( TEnumAsByte<EWorldType::Type> WorldType = EWorldType::Game ) const;
	uint32				GetUniqueID();
	float				CalculateSTC( float MeshDepth, float MeshSize );
	float				AvarageSAC( float MeshDepth );
public:
	TMap<EEAX, EFXEAXREVERBPROPERTIES>  EAXReverb;
	TMap<FString, float>				SAC;
	TMap<FString, uint8>				STC;
private:
	void				SetUpEAXReverbMap();
	void				SetUpAcusticsMap();
	static RookUtils	Utils;
	RookUtils();
	RookUtils( const RookUtils& );
	RookUtils& operator= ( const RookUtils& );
private:
	uint32				CurrentUniqeID = 0;
};