// Top Down Shooter. All Rights Reserved


#include "TDSHealthComponent.h"

UTDSHealthComponent::UTDSHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UTDSHealthComponent::BeginPlay()
{
	Super::BeginPlay();	
}

void UTDSHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTDSHealthComponent::ReceiveDamage(float Damage)
{
	Health -= Damage;
	OnHealthChanged.Broadcast(Health, Damage);

	if (Health <= 0.0f)
	{
		OnDeath.Broadcast();
		IsDeadEvent();
	}

}

void UTDSHealthComponent::IsDeadEvent_Implementation()
{
	//in BP
}