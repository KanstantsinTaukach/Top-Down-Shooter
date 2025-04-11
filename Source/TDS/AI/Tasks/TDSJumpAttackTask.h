// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TDSJumpAttackTask.generated.h"

UCLASS()
class TDS_API UTDSJumpAttackTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UTDSJumpAttackTask();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;	
};
