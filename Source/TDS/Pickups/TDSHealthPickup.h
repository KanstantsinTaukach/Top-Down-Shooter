// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "TDSBasePickup.h"
#include "TDSHealthPickup.generated.h"

UCLASS()
class TDS_API ATDSHealthPickup : public ATDSBasePickup
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (ClampMin = "5.0", ClampMax = "50.0"))
	float HealthAmount = 20.0f;

	UPROPERTY()
	TArray<APawn*> OverlappingPawns;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

private:
	virtual bool GivePickupTo(APawn* PlayerPawn) override;
};
