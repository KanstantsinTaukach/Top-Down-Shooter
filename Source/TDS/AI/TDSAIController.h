// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TDSAIController.generated.h"

UCLASS()
class TDS_API ATDSAIController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void OnPossess(APawn *InPawn) override;
};
