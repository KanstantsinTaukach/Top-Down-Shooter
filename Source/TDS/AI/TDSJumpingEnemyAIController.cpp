// Top Down Shooter. All Rights Reserved

#include "TDSJumpingEnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TDSAIJumpingEnemy.h"
#include "Kismet/GameplayStatics.h"

void ATDSJumpingEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	const auto AICharacter = Cast<ATDSAIJumpingEnemy>(InPawn);
	if (AICharacter)
	{
		RunBehaviorTree(AICharacter->BehaviorTreeAsset);
	}
}

void ATDSJumpingEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	APawn* MyPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (MyPawn)
	{
		GetBlackboardComponent()->SetValueAsVector("AimLocation", MyPawn->GetActorLocation());

		GetBlackboardComponent()->SetValueAsObject("EnemyActor", MyPawn);
	}
}
