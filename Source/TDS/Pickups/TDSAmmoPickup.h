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

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (ClampMin = "1.0", ClampMax = "30.0"))
	int32 BulletsAmount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	EWeaponType WeaponType = EWeaponType::RifleType;

private:
	virtual bool GivePickupTo(APawn* PlayerPawn) override;
	
};
