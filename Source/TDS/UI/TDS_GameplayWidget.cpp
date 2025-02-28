// Top Down Shooter. All Rights Reserved


#include "TDS_GameplayWidget.h"
#include "Components/ProgressBar.h"
#include "../Components/TDSStaminaComponent.h"
#include "../Components/TDSInventoryComponent.h"
#include "../Components/TDSHealthComponent_Character.h"
#include "../Character/TDSCharacter.h"
#include "../Weapons/TDS_WeaponDefault.h"
#include "../FunctionLibrary/TDSUtils.h"

float UTDS_GameplayWidget::GetStaminaPercent() const
{
	const auto StaminaComponent = TDSUtils::GetTDSPlayerComponent<UTDSStaminaComponent>(GetOwningPlayerPawn());
	if (!StaminaComponent)
	{
		return 0.0f;
	}

	return StaminaComponent->GetStaminaPercent();
}

float UTDS_GameplayWidget::GetHealthPercent() const
{
	const auto HealthComponent = TDSUtils::GetTDSPlayerComponent<UTDSHealthComponent_Character>(GetOwningPlayerPawn());
	if (!HealthComponent)
	{
		return 0.0f;
	}

	return HealthComponent->GetHealthPercent();
}

float UTDS_GameplayWidget::GetShieldPercent() const
{
	const auto HealthComponent = TDSUtils::GetTDSPlayerComponent<UTDSHealthComponent_Character>(GetOwningPlayerPawn());
	if (!HealthComponent)
	{
		return 0.0f;
	}

	return HealthComponent->GetShieldPercent();
}

bool UTDS_GameplayWidget::CheckPlayerCanDropWeapon() const
{
	const auto InventoryComponent = TDSUtils::GetTDSPlayerComponent<UTDSInventoryComponent>(GetOwningPlayerPawn());
	if (!InventoryComponent)
	{
		return false;
	}

	return InventoryComponent->GetIsNewPickupWeaponAllowed();
}

bool UTDS_GameplayWidget::CheckPlayerHasWeapon() const
{
	const auto InventoryComponent = TDSUtils::GetTDSPlayerComponent<UTDSInventoryComponent>(GetOwningPlayerPawn());
	if (!InventoryComponent)
	{
		return false;
	}

	return InventoryComponent->IsWeaponExistsInInventory();
}

bool UTDS_GameplayWidget::Initialize()
{
	const auto HealthComponent = TDSUtils::GetTDSPlayerComponent<UTDSHealthComponent_Character>(GetOwningPlayerPawn());
	if (HealthComponent)
	{
		HealthComponent->OnTakeDamage.AddDynamic(this, &UTDS_GameplayWidget::OnHealthTakesDamage);
	}

	return Super::Initialize();
}

void UTDS_GameplayWidget::OnHealthTakesDamage(float Health, float HealthDelta)
{
	OnTakeDamageUI();
}
