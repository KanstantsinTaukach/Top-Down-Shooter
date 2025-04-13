// Top Down Shooter. All Rights Reserved

#include "TDSJumpAttackTask.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../TDSAIJumpingEnemy.h"

UTDSJumpAttackTask::UTDSJumpAttackTask()
{
	NodeName = "Jump Attack";
}

EBTNodeResult::Type UTDSJumpAttackTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto MyController = OwnerComp.GetAIOwner();
	if (ensure(MyController))
	{
		const auto BlackboardComponent = MyController->GetBlackboardComponent();
		if (!BlackboardComponent) return EBTNodeResult::Failed;

		const auto TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject("EnemyActor"));
		if (!TargetActor) return EBTNodeResult::Failed;

		const auto MyPawn = Cast<ATDSAIJumpingEnemy>(MyController->GetPawn());
		if (!MyPawn) return EBTNodeResult::Failed;

		return MyPawn->JumpAttack(TargetActor) ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}
