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

	UFUNCTION(BlueprintCallable, Category = "UI")
	float GetReloadPercent() const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	float GetAmmoPercent() const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool IsPlayerReloading() const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool IsPlayerFiring() const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool PlayerWantsToChangeWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool PlayerHasThisWeapon() const;

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ReloadBar;
};
