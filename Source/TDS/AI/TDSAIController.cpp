// Top Down Shooter. All Rights Reserved

#include "TDSAIController.h"
#include "TDSAICharacterBase.h"
#include "../Components/TDSAIPerceptionComponent.h"

ATDSAIController::ATDSAIController()
{
	TDSAIPerceptionComponent = CreateDefaultSubobject<UTDSAIPerceptionComponent>("TDSAIPerceptionComponent");
	SetPerceptionComponent(*TDSAIPerceptionComponent);
}

void ATDSAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	const auto AICharacter = Cast<ATDSAICharacterBase>(InPawn);
	if (AICharacter)
	{
		RunBehaviorTree(AICharacter->BehaviorTreeAsset);
	}
}

void ATDSAIController::Tick(float Deltatime)
{
	Super::Tick(Deltatime);

	const auto AimActor = TDSAIPerceptionComponent->GetClosesEnemy();
	SetFocus(AimActor);
}
