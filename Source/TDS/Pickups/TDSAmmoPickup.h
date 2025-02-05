// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "TDSBasePickup.h"
#include "../FunctionLibrary/Types.h"
#include "TDSAmmoPickup.generated.h"

UCLASS()
class TDS_API ATDSAmmoPickup : public ATDSBasePickup
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (ClampMin = "1.0", ClampMax = "30.0"))
	int32 BulletsAmount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	EWeaponType WeaponType = EWeaponType::RifleType;

	UPROPERTY()
	TArray<APawn*> OverlappingPawns;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

private:
	virtual bool GivePickupTo(APawn* PlayerPawn) override;	
};
