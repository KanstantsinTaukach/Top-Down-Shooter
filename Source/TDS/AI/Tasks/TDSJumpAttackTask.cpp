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

		const auto TargetObject = BlackboardComponent->GetValueAsObject("EnemyActor");
		if (!TargetObject) return EBTNodeResult::Failed;

		ACharacter* TargetCharacter = Cast<ACharacter>(TargetObject);
		if (!TargetCharacter) return EBTNodeResult::Failed;

		const auto MyPawn = Cast<ATDSAIJumpingEnemy>(MyController->GetPawn());
		if (!MyPawn) return EBTNodeResult::Failed;

		MyPawn->JumpAttack(Cast<ACharacter>(TargetCharacter));
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
