// Top Down Shooter. All Rights Reserved


#include "TDSHealthPickup.h"
#include "../Components/TDSHealthComponent_Character.h"

void ATDSHealthPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (const auto OverlapPawn : OverlappingPawns)
	{
		if (GivePickupTo(OverlapPawn))
		{
			ATDSBasePickup::PickupSuccess();
			break;
		}
	}
}

bool ATDSHealthPickup::GivePickupTo(APawn* PlayerPawn)
{
	if (!PlayerPawn)
	{
		return false;
	}

	const auto HealthComponent = Cast<UTDSHealthComponent_Character>(PlayerPawn->GetComponentByClass(UTDSHealthComponent_Character::StaticClass()));
	if (!HealthComponent)
	{
		return false;
	}
	
	float PlayerMaxHealth = HealthComponent->GetMaxHealth();
	float PlayerCurrentHealth = HealthComponent->GetCurrentHealt();

	if (PlayerMaxHealth == PlayerCurrentHealth)
	{
		return false;
	}

	HealthComponent->AddToCurrentHealth(HealthAmount);
	return true;
}

void ATDSHealthPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	const auto Pawn = Cast<APawn>(OtherActor);

	if (Pawn)
	{
		OverlappingPawns.Add(Pawn);
	}
}

void ATDSHealthPickup::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	const auto Pawn = Cast<APawn>(OtherActor);
	OverlappingPawns.Remove(Pawn);
}