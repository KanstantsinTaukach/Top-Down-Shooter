// Top Down Shooter. All Rights Reserved


#include "TSD_GameplayWidget.h"
#include "Components/ProgressBar.h"
#include "../Components/TDSStaminaComponent.h"

float UTSD_GameplayWidget::GetStaminaPercent() const
{
	const auto Player = GetOwningPlayerPawn();
	if (!Player)
	{
		return 0.0f;
	}

	const auto Component = Player->GetComponentByClass(UTDSStaminaComponent::StaticClass());
	const auto StaminaComponent = Cast<UTDSStaminaComponent>(Component);
	if (!StaminaComponent)
	{
		return 0.0f;
	}
	return StaminaComponent->GetStaminaPercent();
}

