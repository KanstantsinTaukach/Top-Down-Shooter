// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "TDSBasePickup.h"
#include "../FunctionLibrary/Types.h"
#include "TDSWeaponPickup.generated.h"

UCLASS()
class TDS_API ATDSWeaponPickup : public ATDSBasePickup
{
	GENERATED_BODY()

public:
	FWeaponSlot GetWeaponslot() const { return WeaponSlot; };

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	FWeaponSlot WeaponSlot;

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	virtual void BeginPlay();

private:
	virtual bool GivePickupTo(APawn* PlayerPawn) override;	
};
