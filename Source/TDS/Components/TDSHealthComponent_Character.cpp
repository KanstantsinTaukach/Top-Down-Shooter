// Top Down Shooter. All Rights Reserved


#include "TDSHealthComponent_Character.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(TDSHealthComponent_CharacterLog, All, All);

void UTDSHealthComponent_Character::BeginPlay()
{
	Super::BeginPlay();

	check(MaxShield > 0.0f);

	SetCurrentShield(MaxShield);
}

void UTDSHealthComponent_Character::RemoveFromCurrentHealth(float DamageValue)
{
	float CurrentDamage = DamageValue * DamageFactor;

	if (Shield > 0.0f)
	{
		RemoveFromCurrentShield(CurrentDamage);
	}
	else
	{
		Super::RemoveFromCurrentHealth(DamageValue);

		if (Health <= 0.0f)
		{
			GetWorld()->GetTimerManager().ClearTimer(ShieldUpdateTimerHandle);
		}
	}
}

void UTDSHealthComponent_Character::RemoveFromCurrentShield(float DamageValue)
{
	Shield -= DamageValue;

	if (Shield < 0.0f)
	{
		Shield = 0.0f;
	}

	OnShieldTakeDamage.Broadcast(Shield, DamageValue);

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(ShieldUpdateTimerHandle, this, &UTDSHealthComponent_Character::ShieldUpdate, ShieldUpdateTime, true, ShieldDelay);
	}
}

void UTDSHealthComponent_Character::AddToCurrentShield(float ShieldValue)
{
	Shield += ShieldValue;

	if (Shield > MaxShield)
	{
		Shield = MaxShield;
	}

	OnShieldChanged.Broadcast(Shield, ShieldValue);
}

void UTDSHealthComponent_Character::ShieldUpdate()
{
	AddToCurrentShield(ShieldRecoveryRate);

	if (GetCurrentShield() == MaxShield && GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ShieldUpdateTimerHandle);
	}
}