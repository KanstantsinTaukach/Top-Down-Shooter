// Top Down Shooter. All Rights Reserved

#include "TDSAIPerceptionComponent.h"
#include "AIController.h"
#include "../FunctionLibrary/TDSUtils.h"
#include "../Components/TDSHealthComponent_Character.h"
#include "Perception/AISense_Sight.h"

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
