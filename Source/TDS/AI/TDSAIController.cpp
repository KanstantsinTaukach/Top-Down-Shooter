// Top Down Shooter. All Rights Reserved

#include "TDSAIController.h"
#include "TDSAICharacterBase.h"
#include "BehaviorTree/BlackboardComponent.h"

void ATDSAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	const auto AICharacter = Cast<ATDSAICharacterBase>(InPawn);
	if (AICharacter)
	{
		RunBehaviorTree(AICharacter->BehaviorTreeAsset);
	}
}
