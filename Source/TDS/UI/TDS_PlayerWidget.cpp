// Top Down Shooter. All Rights Reserved


#include "TDS_PlayerWidget.h"
#include "Components/ProgressBar.h"

void UTDS_PlayerWidget::SetStamina(float CurrentStamina, float MaxStamina)
{
	if (StaminaBar)
	{
		StaminaBar->SetPercent(CurrentStamina / MaxStamina);
	}
}
