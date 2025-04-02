// Top Down Shooter. All Rights Reserved

#include "TDSAIController.h"
#include "TDSAICharacterBase.h"
#include "../Components/TDSAIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

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

	const auto AimActor = GetFocusOnActor();
	SetFocus(AimActor);
}

AActor* ATDSAIController::GetFocusOnActor() const
{
	if (!GetBlackboardComponent()) return nullptr;

	return Cast<AActor>(GetBlackboardComponent()->GetValueAsObject(FocusOnKeyName));
}
