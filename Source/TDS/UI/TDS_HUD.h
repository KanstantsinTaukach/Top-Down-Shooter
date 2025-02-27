// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TDS_HUD.generated.h"

class UTDS_GameplayWidget;

UCLASS()
class TDS_API ATDS_HUD : public AHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	UTDS_GameplayWidget* GetGameplayWidget() const { return GameplayWidget; };

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTDS_GameplayWidget> GameplayWidgetClass;

	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UTDS_GameplayWidget* GameplayWidget;
};