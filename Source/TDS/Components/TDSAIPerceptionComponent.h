// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"
#include "TDSAIPerceptionComponent.generated.h"

UCLASS()
class TDS_API UTDSAIPerceptionComponent : public UAIPerceptionComponent
{
	GENERATED_BODY()

public:
	AActor* GetClosesEnemy() const;

	AActor* GetHighestPriorityTarget() const;	
};
