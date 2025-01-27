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
