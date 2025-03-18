// Top Down Shooter. All Rights Reserved


#include "TDSAICharacterBase.h"
#include "../Components/TDSHealthComponent.h"

ATDSAICharacterBase::ATDSAICharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	HealthComponent = CreateDefaultSubobject<UTDSHealthComponent>("HealthComponent");
}

void ATDSAICharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	check(HealthComponent);
//	HealthComponent->OnDeath.AddDynamic(this, &ATDSAICharacterBase::OnAICharacterDeath);
}

void ATDSAICharacterBase::OnAICharacterDeath()
{
}
