#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "RookListenerControllerBPL.generated.h"

UCLASS()
class ROOK_API URookListenerControllerBPL : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION( BlueprintCallable, Category = "Rook|Listener" )
	static bool				SetActiveListener( TSubclassOf<class URookListenerController> ListenerBlueprint );
	UFUNCTION( BlueprintCallable, Category = "Rook|Listener" )
	static bool				SetListenerType( TSubclassOf<class URookListenerController> ListenerBlueprint, EListenerType NewListenerType );
	UFUNCTION( BlueprintCallable, Category = "Rook|Listener" )
	static bool				SetActorOnListener( TSubclassOf<class URookListenerController> ListenerBlueprint, class AActor* NewActor );
	UFUNCTION( BlueprintCallable, Category = "Rook|Listener" )
	static bool				SetSocketOnListener( TSubclassOf<class URookListenerController> ListenerBlueprint, FSocketAttachment NewSocketAttachment );
	UFUNCTION( BlueprintCallable, Category = "Rook|Listener" )
	static bool				SetCameraOnListener( TSubclassOf<class URookListenerController> ListenerBlueprint, class APlayerCameraManager* NewCamera );
	UFUNCTION( BlueprintCallable, Category = "Rook|Listener" )
	static bool				SetFreezRotationOnListener( TSubclassOf<class URookListenerController> ListenerBlueprint, bool bFreezRotation, FFreezRotation NewFreezRotation );
	UFUNCTION( BlueprintCallable, Category = "Rook|Listener" )
	static bool				ShouldListenerUseVelocity( TSubclassOf<class URookListenerController> ListenerBlueprint, bool bShouldUseVelocity );
};