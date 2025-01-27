// Top Down Shooter. All Rights Reserved


#include "TDSWeaponPickup.h"
#include "../Components/TDSInventoryComponent.h"
#include "../Character/TDSCharacter.h"

bool ATDSWeaponPickup::GivePickupTo(APawn* PlayerPawn)
{
	if (!PlayerPawn)
	{
		return false;
	}

	const auto InventoryComponent = Cast<UTDSInventoryComponent>(PlayerPawn->GetComponentByClass(UTDSInventoryComponent::StaticClass()));
		
	if (InventoryComponent && InventoryComponent->TryGetWeaponToInventory(WeaponSlot))
	{
		return true;
	}
	else
	{
		const auto Player = Cast<ATDSCharacter>(InventoryComponent->GetOwner());
		Player->StartSwitchWeapon(this);
	}
	
	return false;
}

void ATDSWeaponPickup::NotifyActorEndOverlap(AActor* OtherActor)
{
	const auto Player = Cast<ATDSCharacter>(OtherActor);

	Player->EndSwitchWeapon();
}

void ATDSWeaponPickup::ChangePickupValue(FWeaponSlot NewWeaponSlot)
{
	WeaponSlot.NameItem = NewWeaponSlot.NameItem;
	WeaponSlot.AdditionalInfo = NewWeaponSlot.AdditionalInfo;
}