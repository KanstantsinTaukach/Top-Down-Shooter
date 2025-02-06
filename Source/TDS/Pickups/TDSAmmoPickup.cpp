// Top Down Shooter. All Rights Reserved


#include "TDSAmmoPickup.h"
#include "../Components/TDSInventoryComponent.h"

DEFINE_LOG_CATEGORY_STATIC(ATDSAmmoPickupLog, All, All);

void ATDSAmmoPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (const auto OverlapPawn : OverlappingPawns)
	{
		if (GivePickupTo(OverlapPawn))
		{
			ATDSBasePickup::PickupSuccess();
			break;
		}
	}
}

void ATDSAmmoPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	const auto Pawn = Cast<APawn>(OtherActor);
	if (Pawn)
	{
		OverlappingPawns.Add(Pawn);
	}
}

void ATDSAmmoPickup::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	const auto Pawn = Cast<APawn>(OtherActor);
	OverlappingPawns.Remove(Pawn);
}

bool ATDSAmmoPickup::GivePickupTo(APawn* PlayerPawn)
{	
	if (!PlayerPawn)
	{
		return false;
	}

	const auto InventoryComponent = Cast<UTDSInventoryComponent>(PlayerPawn->GetComponentByClass(UTDSInventoryComponent::StaticClass()));
	if (!InventoryComponent)
	{
		return false;
	}

	bool Result = InventoryComponent->CheckCanTakeAmmo(WeaponType);
	if (Result)
	{
		InventoryComponent->AmmoSlotChangeValue(WeaponType, BulletsAmount);
	}

	return Result;
}
