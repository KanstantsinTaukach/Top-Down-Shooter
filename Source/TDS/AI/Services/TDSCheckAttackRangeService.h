// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "TDSCheckAttackRangeService.generated.h"

UCLASS()
class TDS_API UTDSCheckAttackRangeService : public UBTService
{
	GENERATED_BODY()
	
public:
	UTDSCheckAttackRangeService();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FBlackboardKeySelector AttackRangeKey;

	UPROPERTY(EditAnywhere, Category = "AI", meta = (ClampMin = 0.0f))
	float DistanceToAttack = 1000.0f;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
