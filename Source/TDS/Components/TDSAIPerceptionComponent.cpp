// Top Down Shooter. All Rights Reserved

#include "TDSAIPerceptionComponent.h"
#include "AIController.h"
#include "../FunctionLibrary/TDSUtils.h"
#include "../Components/TDSHealthComponent_Character.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"
#include "../Weapons/TDS_ProjectileDefault_Grenade.h"

AActor* UTDSAIPerceptionComponent::GetClosesEnemy() const
{
	TArray<AActor*> PerceiveActors;
	GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceiveActors);
	if (PerceiveActors.Num() == 0) return nullptr;

	const auto Controller = Cast<AAIController>(GetOwner());
	if (!Controller) return nullptr;

	const auto Pawn = Controller->GetPawn();
	if (!Pawn) return nullptr;

	float BestDistance = MAX_FLT;
	AActor* BestPawn = nullptr;

	for (const auto PerceiveActor : PerceiveActors)
	{
		const auto HealthComponent = TDSUtils::GetTDSPlayerComponent<UTDSHealthComponent_Character>(PerceiveActor);
		if (HealthComponent && HealthComponent->GetCurrentHealth() > 0.0f)
		{
			const auto CurrentDistance = (PerceiveActor->GetActorLocation() - Pawn->GetActorLocation()).Size();
			if (CurrentDistance < BestDistance)
			{
				BestDistance = CurrentDistance;
				BestPawn = PerceiveActor;
			}
		}
	}

	return BestPawn;
}

AActor* UTDSAIPerceptionComponent::GetHighestPriorityTarget() const
{
	TArray<AActor*> PerceiveActors;
	GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceiveActors);
	GetCurrentlyPerceivedActors(UAISense_Damage::StaticClass(), PerceiveActors);
	GetCurrentlyPerceivedActors(UAISense_Hearing::StaticClass(), PerceiveActors);

	if (PerceiveActors.Num() == 0) return nullptr;

	const auto Controller = Cast<AAIController>(GetOwner());
	if (!Controller) return nullptr;

	const auto Pawn = Controller->GetPawn();
	if (!Pawn) return nullptr;

	float BestScore = -1.0f;
	AActor* BestTarget = nullptr;

	for (const auto PerceiveActor : PerceiveActors)
	{
		if (!IsValid(PerceiveActor)) continue;

		bool IsGrenade = Cast<ATDS_ProjectileDefault_Grenade>(PerceiveActor) != nullptr;

		const auto HealthComponent = TDSUtils::GetTDSPlayerComponent<UTDSHealthComponent_Character>(PerceiveActor);
		bool IsAlivePlayer = HealthComponent && HealthComponent->GetCurrentHealth() > 0.0f;

		if (IsGrenade || IsAlivePlayer)
		{
			float Distance = (PerceiveActor->GetActorLocation() - Pawn->GetActorLocation()).Size();
			if (Distance <= KINDA_SMALL_NUMBER) Distance = 1.0f;

			float Score = IsGrenade ? 1.0f : (1.0f / Distance);

			if (Score > BestScore)
			{
				BestScore = Score;
				BestTarget = PerceiveActor;
			}
		}
	}

	return BestTarget;
}
