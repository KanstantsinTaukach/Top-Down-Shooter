// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TSD_GameplayWidget.generated.h"

class UProgressBar;

UCLASS()
class TDS_API UTSD_GameplayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	float GetStaminaPercent() const;

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaBar;
};