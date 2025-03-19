// Top Down Shooter. All Rights Reserved

#include "TDSLootDropComponent.h"
#include "../Pickups/TDSBasePickup.h"

DEFINE_LOG_CATEGORY_STATIC(TDSLootDropComponentLog, All, All);

UTDSLootDropComponent::UTDSLootDropComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTDSLootDropComponent::BeginPlay()
{
	Super::BeginPlay();

	HasExecuted = false;
}

void UTDSLootDropComponent::DropLoot(const FVector& Location)
{
	if (LootChances.Num() == 0 || !GetWorld())
	{
		UE_LOG(TDSLootDropComponentLog, Warning, TEXT("No loot configured or invalid world. Check LootChances UPROPERTY."));
		return;
	}

	float RandomChance = FMath::FRandRange(0.0f, 1.0f);
	if (RandomChance <= DropChance)
	{
		float TotalChance = 0.0f;
		for (const auto& LootChance : LootChances)
		{
			TotalChance += LootChance.PickupDropChance;
		}

		if (TotalChance <= 0.0f)
		{
			UE_LOG(TDSLootDropComponentLog, Warning, TEXT("Total chance is zero or negative."));
			return;
		}

		float RandomValue = FMath::FRandRange(0.0f, TotalChance);
		float AccumulatedChance = 0.0f;

		for (const auto& LootChance : LootChances)
		{
			AccumulatedChance += LootChance.PickupDropChance;
			if (RandomValue <= AccumulatedChance && LootChance.PickupClass && !HasExecuted)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = GetOwner();
				SpawnParams.Instigator = Cast<APawn>(GetOwner());

				ATDSBasePickup* Pickup = GetWorld()->SpawnActor<ATDSBasePickup>(LootChance.PickupClass, Location, FRotator::ZeroRotator, SpawnParams);
				HasExecuted = true;
				return;
			}
		}
	}
}
