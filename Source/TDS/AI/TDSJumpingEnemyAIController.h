// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TDSJumpingEnemyAIController.generated.h"

UCLASS()
class TDS_API ATDSJumpingEnemyAIController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void BeginPlay() override;
};
