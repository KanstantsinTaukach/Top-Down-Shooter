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
		if (Shield <= 0.0f)
		{
			UE_LOG(TDSHealthComponent_CharacterLog, Warning, TEXT("UTDSHealthComponent_Character::RemoveFromCurrentHealth - Shield <= 0"));
			//FX
		}
	}
	else
	{
		Super::RemoveFromCurrentHealth(DamageValue);
	}
}

void UTDSHealthComponent_Character::RemoveFromCurrentShield(float DamageValue)
{
	Shield -= DamageValue;

	UE_LOG(TDSHealthComponent_CharacterLog, Warning, TEXT("UTDSHealthComponent_Character::RemoveFromCurrentShield - Shield value is: %f"), Shield);

	OnShieldTakeDamage.Broadcast(Shield, DamageValue);

	if (Shield < 0.0f)
	{
		Shield = 0.0f;
	}

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
}

void UTDSHealthComponent_Character::ShieldUpdate()
{
	AddToCurrentShield(ShieldRecoveryRate);

	UE_LOG(TDSHealthComponent_CharacterLog, Warning, TEXT("UTDSHealthComponent_Character::ShieldUpdate - Shield value is: %f"), Shield);

	if (GetCurrentShield() == MaxShield && GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ShieldUpdateTimerHandle);
	}
}
