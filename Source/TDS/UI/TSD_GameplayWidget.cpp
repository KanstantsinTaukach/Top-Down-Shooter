// Top Down Shooter. All Rights Reserved


#include "TSD_GameplayWidget.h"
#include "Components/ProgressBar.h"
#include "../Components/TDSStaminaComponent.h"
#include "../Components/TDSInventoryComponent.h"
#include "../Components/TDSHealthComponent_Character.h"
#include "../Character/TDSCharacter.h"
#include "../Weapons/TDS_WeaponDefault.h"
#include "../FunctionLibrary/TDSUtils.h"

float UTSD_GameplayWidget::GetStaminaPercent() const
{
	const auto StaminaComponent = TDSUtils::GetTDSPlayerComponent<UTDSStaminaComponent>(GetOwningPlayerPawn());
	if (!StaminaComponent)
	{
		return 0.0f;
	}

	return StaminaComponent->GetStaminaPercent();
}

float UTSD_GameplayWidget::GetHealthPercent() const
{
	const auto HealthComponent = TDSUtils::GetTDSPlayerComponent<UTDSHealthComponent_Character>(GetOwningPlayerPawn());
	if (!HealthComponent)
	{
		return 0.0f;
	}

	return HealthComponent->GetHealthPercent();
}

float UTSD_GameplayWidget::GetShieldPercent() const
{
	const auto HealthComponent = TDSUtils::GetTDSPlayerComponent<UTDSHealthComponent_Character>(GetOwningPlayerPawn());
	if (!HealthComponent)
	{
		return 0.0f;
	}

	return HealthComponent->GetShieldPercent();
}

float UTSD_GameplayWidget::GetReloadPercent() const
{
	ATDSCharacter* PlayerCharacter = Cast<ATDSCharacter>(GetOwningPlayerPawn());
	if (!PlayerCharacter)
	{
		return 0.0f;
	}

	ATDS_WeaponDefault* Weapon = PlayerCharacter->GetCurrentWeapon();
	if (!Weapon)
	{
		return 0.0f;
	}

	return Weapon->GetReloadPercent();
}

bool UTSD_GameplayWidget::IsPlayerReloading() const
{
	ATDSCharacter* PlayerCharacter = Cast<ATDSCharacter>(GetOwningPlayerPawn());
	if (!PlayerCharacter)
	{
		return false;
	}

	ATDS_WeaponDefault* Weapon = PlayerCharacter->GetCurrentWeapon();
	if (!Weapon)
	{
		return false;
	}

	return Weapon->GetReloadState();
}

bool UTSD_GameplayWidget::CheckPlayerCanDropWeapon() const
{
	const auto InventoryComponent = TDSUtils::GetTDSPlayerComponent<UTDSInventoryComponent>(GetOwningPlayerPawn());
	if (!InventoryComponent)
	{
		return false;
	}

	return InventoryComponent->GetIsNewPickupWeaponAllowed();
}

bool UTSD_GameplayWidget::CheckPlayerHasWeapon() const
{
	const auto InventoryComponent = TDSUtils::GetTDSPlayerComponent<UTDSInventoryComponent>(GetOwningPlayerPawn());
	if (!InventoryComponent)
	{
		return false;
	}

	return InventoryComponent->IsWeaponExistsInInventory();
}

bool UTSD_GameplayWidget::Initialize()
{
	const auto HealthComponent = TDSUtils::GetTDSPlayerComponent<UTDSHealthComponent_Character>(GetOwningPlayerPawn());
	if (HealthComponent)
	{
		HealthComponent->OnTakeDamage.AddDynamic(this, &UTSD_GameplayWidget::OnHealthTakesDamage);
	}

	return Super::Initialize();
}

void UTSD_GameplayWidget::OnHealthTakesDamage(float Health, float HealthDelta)
{
	OnTakeDamageUI();
}
