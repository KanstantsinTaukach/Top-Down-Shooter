// Top Down Shooter. All Rights Reserved


#include "TDSAmmoPickup.h"
#include "../Components/TDSInventoryComponent.h"

DEFINE_LOG_CATEGORY_STATIC(ATDSAmmoPickupLog, All, All);

bool ATDSAmmoPickup::GivePickupTo(APawn* PlayerPawn)
{
	if (!PlayerPawn)
	{
		return false;
	}

	const auto InventoryComponent = Cast<UTDSInventoryComponent>(PlayerPawn->GetComponentByClass(UTDSInventoryComponent::StaticClass()));

	if (InventoryComponent->CheckCanTakeAmmo(WeaponType))
	{
		InventoryComponent->AmmoSlotChangeValue(WeaponType, BulletsAmount);
	}

	UE_LOG(ATDSAmmoPickupLog, Display, TEXT("ATDSAmmoPickup::NotifyActorBeginOverlap - Pickup was taken"));

	return true;
}
