// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "TDSHealthComponent.h"
#include "TDSHealthComponent_Character.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShieldTakeDamageSignature, float, Health, float, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShieldChangedSignature, float, Health, float, Damage);

UCLASS()
class TDS_API UTDSHealthComponent_Character : public UTDSHealthComponent
{
	GENERATED_BODY()

private:
	FTimerHandle ShieldUpdateTimerHandle;

protected:
	float Shield = 100.0f;
	float MaxShield = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shield")
	float ShieldRecoveryRate = 10.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shield")
	float ShieldUpdateTime = 2.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shield")
	float ShieldDelay = 5.0f;

	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Shield")
	FOnShieldTakeDamageSignature OnShieldTakeDamage;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Shield")
	FOnShieldChangedSignature OnShieldChanged;

	UFUNCTION(BlueprintCallable, Category = "Shield")
	float GetCurrentShield() const { return Shield; };
	UFUNCTION(BlueprintCallable, Category = "Shield")
	void SetCurrentShield(float NewShield) { Shield = NewShield; };

	virtual void RemoveFromCurrentHealth(float DamageValue) override;

	void RemoveFromCurrentShield(float DamageValue);

	virtual void AddToCurrentShield(float ShieldValue);

	void ShieldUpdate();
};
