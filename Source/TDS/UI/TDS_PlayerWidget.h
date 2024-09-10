// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TDS_PlayerWidget.generated.h"

class UProgressBar;

UCLASS()
class TDS_API UTDS_PlayerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetStamina(float CurrentStamina, float MaxStamina);

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaBar;
};
