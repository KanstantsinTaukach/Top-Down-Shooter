// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TDSHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTakeDamageSignature, float, Health, float, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, Health, float, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

USTRUCT(BlueprintType)
struct FStatsParam
{
	GENERATED_BODY()

};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TDS_API UTDSHealthComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UTDSHealthComponent();	

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Health")
	FOnTakeDamageSignature OnTakeDamage;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Health")
	FOnHealthChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Health")
	FOnDeathSignature OnDeath;

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetCurrentHealth() const { return Health; };
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetCurrentHealth(float NewHealth) { Health = FMath::Clamp(NewHealth, 0.0f, MaxHealth); };
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; };

	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual void AddToCurrentHealth(float HealthValue);
	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual void RemoveFromCurrentHealth(float DamageValue);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

	float GetHealthPercent() const { return Health / MaxHealth; };

protected:
	float Health = 100.0f;
	float MaxHealth = 100.0f;

	virtual void BeginPlay() override;
};
