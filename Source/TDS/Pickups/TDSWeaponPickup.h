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
	ATDSWeaponPickup();

	void InitPickup(const FDropItem& DropItemInfo);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pickup")
	USkeletalMeshComponent* PickupSkeletalMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	FWeaponSlot WeaponSlot;

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	virtual void BeginPlay() override;

private:
	virtual bool GivePickupTo(APawn* PlayerPawn) override;
};