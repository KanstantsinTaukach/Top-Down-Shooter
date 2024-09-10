// Top Down Shooter. All Rights Reserved

#include "TDSStaminaComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogStaminaComponent, All, All);

UTDSStaminaComponent::UTDSStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTDSStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	check(MaxStamina > 0.0f);
		
	SetStamina(MaxStamina);
}

void UTDSStaminaComponent::ChangeSpamina(bool IsSprinting)
{
	if (IsSprinting)
	{
		GetWorld()->GetTimerManager().SetTimer(StaminaTimerHandle, this, &UTDSStaminaComponent::DrainStamina, StaminaUpdateTime, true);
	}
	else 
	{
		GetWorld()->GetTimerManager().SetTimer(StaminaTimerHandle, this, &UTDSStaminaComponent::RegenerateStamina, StaminaUpdateTime, true);
	}
}

void UTDSStaminaComponent::DrainStamina()
{
	if (GetWorld())
	{	
		if (Stamina > 0.0f)
		{			
			SetStamina(Stamina - StaminaDrainRate);
		} 
		else if (IsStaminaEmpty())
		{
			GetWorld()->GetTimerManager().ClearTimer(StaminaTimerHandle);
			OnStaminaEmpty.Broadcast();
		}
	}
}
void UTDSStaminaComponent::RegenerateStamina()
{
	if (GetWorld())
	{
		if (Stamina < MaxStamina)
		{
			SetStamina(Stamina + StaminaRegenRate);
		}
		else if (FMath::IsNearlyEqual(Stamina, MaxStamina))
		{
			GetWorld()->GetTimerManager().ClearTimer(StaminaTimerHandle);
		}
	}
}

void UTDSStaminaComponent::SetStamina(float NewStamina)
{
	Stamina = FMath::Clamp(NewStamina, 0.0f, MaxStamina);
	OnStaminaChanged.Broadcast(Stamina);
}
