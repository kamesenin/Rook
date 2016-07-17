#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "RookAudioControllerBPL.generated.h"

UCLASS()
class ROOK_API URookAudioControllerBPL : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Rook|Audio")
	static bool				Play( TSubclassOf<class URookAudioController> AudioBlueprint, AActor* Parent );
	UFUNCTION(BlueprintCallable, Category = "Rook|Audio")
	static bool				Stop( TSubclassOf<class URookAudioController> AudioBlueprint );
	UFUNCTION(BlueprintCallable, Category = "Rook|Audio")
	static bool				Pause( TSubclassOf<class URookAudioController> AudioBlueprint );
	UFUNCTION(BlueprintCallable, Category = "Rook|Audio")
	static bool				ChangeSurfaceType( TSubclassOf<class URookAudioController> AudioBlueprint, TEnumAsByte<EPhysicalSurface> NewSurface );
	UFUNCTION(BlueprintCallable, Category = "Rook|Audio")
	static bool				ChangeEAXType( TSubclassOf<class URookAudioController> AudioBlueprint, EEAX NewEAX );
	UFUNCTION(BlueprintCallable, Category = "Rook|Audio")
	static bool				DebugLines( TSubclassOf<class URookAudioController> AudioBlueprint, EEnableState UseDebuLines );
};