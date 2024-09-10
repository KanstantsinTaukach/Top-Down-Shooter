// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TDS_HUD.generated.h"

class UTSD_GameplayWidget;

UCLASS()
class TDS_API ATDS_HUD : public AHUD
{
	GENERATED_BODY()

public:

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTSD_GameplayWidget> GameplayWidgetClass;

	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UTSD_GameplayWidget* GameplayWidget;
};
