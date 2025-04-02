// Top Down Shooter. All Rights Reserved

#include "TDSFindEnemyService.h"
#include "AIController.h"
#include "../FunctionLibrary/TDSUtils.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../Components/TDSAIPerceptionComponent.h"

UTDSFindEnemyService::UTDSFindEnemyService()
{
	NodeName = "Find Enemy";
}

void UTDSFindEnemyService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const auto Blackboard = OwnerComp.GetBlackboardComponent();
	if (Blackboard)
	{
		const auto Controller = OwnerComp.GetAIOwner();
		const auto PerceptionComponent = TDSUtils::GetTDSPlayerComponent<UTDSAIPerceptionComponent>(Controller);
		if (PerceptionComponent)
		{
			Blackboard->SetValueAsObject(EnemyActorKey.SelectedKeyName, PerceptionComponent->GetClosesEnemy());
		}
	}

	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}
