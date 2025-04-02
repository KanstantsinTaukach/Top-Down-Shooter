// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "TDSFindEnemyService.generated.h"

UCLASS()
class TDS_API UTDSFindEnemyService : public UBTService
{
	GENERATED_BODY()

public:
	UTDSFindEnemyService();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FBlackboardKeySelector EnemyActorKey;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
