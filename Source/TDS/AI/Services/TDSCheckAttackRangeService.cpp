// Top Down Shooter. All Rights Reserved

#include "TDSCheckAttackRangeService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UTDSCheckAttackRangeService::UTDSCheckAttackRangeService()
{
	NodeName = "Check Attack Range";
}

void UTDSCheckAttackRangeService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const auto BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackboardComponent))
	{
		const auto EnemyActor = Cast<AActor>(BlackboardComponent->GetValueAsObject("EnemyActor"));
		if (EnemyActor)
		{
			const auto AIController = OwnerComp.GetAIOwner();
			if (ensure(AIController))
			{
				const auto AIPawn = AIController->GetPawn();
				if (ensure(AIPawn))
				{
					float DistanceTo = FVector::Distance(EnemyActor->GetActorLocation(), AIPawn->GetActorLocation());

					bool HasLOS = false;
					bool IsWithinRange = DistanceTo < DistanceToAttack;
					if (IsWithinRange)
					{
						HasLOS = AIController->LineOfSightTo(EnemyActor);
					}

					BlackboardComponent->SetValueAsBool(AttackRangeKey.SelectedKeyName, IsWithinRange && HasLOS);
				}
			}
		}
	}
}