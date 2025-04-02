// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TDSAIController.generated.h"

class UTDSAIPerceptionComponent;

UCLASS()
class TDS_API ATDSAIController : public AAIController
{
	GENERATED_BODY()

public:
	ATDSAIController();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTDSAIPerceptionComponent* TDSAIPerceptionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FName FocusOnKeyName = "TargetActor";

	virtual void OnPossess(APawn *InPawn) override;

	virtual void Tick(float Deltatime) override;

private:
	AActor* GetFocusOnActor() const;
};
