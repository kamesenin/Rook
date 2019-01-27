#pragma once
#include "GameFramework/Actor.h"
#include "RookEnums.h"
#include "RookEAXVolumeBox.generated.h"

UCLASS()
class ROOK_API ARookEAXVolumeBox : public AActor 
{
	GENERATED_BODY()
public:

	ARookEAXVolumeBox(const class FObjectInitializer& ObjectInitializer);
	virtual ~ARookEAXVolumeBox();
	virtual void			BeginPlay();
	virtual void			EndPlay( const EEndPlayReason::Type EndPlayReason );

public:
	/** Box component for volume */
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Rook EAX" )
	class UBoxComponent*	EAXVolumeBox;
	/** Current EAX on volume */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Rook EAX" )
	EEAX					VolumeEAX;
private:
	/** Function for when Actor is overlaping volume */
	UFUNCTION()
	void					OnOverlap( class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult );
	/** Function for when Actor is ends overlaping volume */
	UFUNCTION()
	void					EndOfOverlap( class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );
private:
	/** Helper shared pointer to Rook interface */
	class IRook*			RookInterface;
};