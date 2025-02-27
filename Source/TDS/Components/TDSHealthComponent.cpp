// Top Down Shooter. All Rights Reserved


#include "TDSHealthComponent.h"

UTDSHealthComponent::UTDSHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UTDSHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	check(MaxHealth > 0.0f);

	SetCurrentHealth(MaxHealth);
}

void UTDSHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTDSHealthComponent::AddToCurrentHealth(float HealthValue)
{
	Health += HealthValue;

	if (Health > MaxHealth)
	{
		Health = MaxHealth;
	}

	OnHealthChanged.Broadcast(Health, HealthValue);
}

void UTDSHealthComponent::RemoveFromCurrentHealth(float DamageValue)
{
	DamageValue *= DamageFactor;
	Health -= DamageValue;

	if (Health <= 0.0f)
	{
		OnDeath.Broadcast();
	}

	OnTakeDamage.Broadcast(Health, DamageValue);
}