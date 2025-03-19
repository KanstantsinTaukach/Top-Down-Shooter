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
	if (RandomChance > DropChance) return;
	
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
			FVector Start = Location;
			FVector End = Start - FVector(0.0f, 0.0f, 500.0f);
			FHitResult HitResult;

			FCollisionQueryParams QueryParams;
			QueryParams.bTraceComplex = false;
			QueryParams.AddIgnoredActor(GetOwner());

			bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams);

			FVector SpawnLocation = Location;
			if (bHit)
			{
				SpawnLocation = HitResult.ImpactPoint + FVector(0.0f, 100.0f, 50.0f);
			}

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = Cast<APawn>(GetOwner());
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

			ATDSBasePickup* Pickup = GetWorld()->SpawnActor<ATDSBasePickup>(LootChance.PickupClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			HasExecuted = true;
			return;
		}
	}
}
