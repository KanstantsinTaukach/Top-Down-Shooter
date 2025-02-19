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
	float GetHealthPercent() const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	float GetShieldPercent() const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	float GetReloadPercent() const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool IsPlayerReloading() const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool CheckPlayerCanDropWeapon() const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool CheckPlayerHasWeapon() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnTakeDamageUI();

	virtual bool Initialize() override;

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ReloadBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ShieldBar;

private:
	UFUNCTION()
	void OnHealthTakesDamage(float Health, float HealthDelta);
};
