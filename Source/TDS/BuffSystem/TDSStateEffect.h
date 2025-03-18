// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TDSStateEffect.generated.h"

class UParticleSystemComponent;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS(Blueprintable, BlueprintType)
class TDS_API UTDSStateEffect : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effects")
	TArray<TEnumAsByte<EPhysicalSurface>> InteractEffects;

	virtual bool InitObject(AActor* Actor, const FHitResult& Hit);
	virtual void DestroyObject();

	void DestroyEffectFX();

	bool GetIsStackable() const { return IsStackable; };

protected:
	AActor* TargetActor = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effects")
	bool IsStackable = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ExecuteEffectFX")
	UParticleSystem* EffectFX = nullptr;

	UParticleSystemComponent* ParticleEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* NiagaraEffectFX;

	UNiagaraComponent* NiagaraEffect;

private:
	void InitEffectFX(const FHitResult& Hit);
};



UCLASS()
class TDS_API UTDSStateEffect_ExecuteOnce : public UTDSStateEffect
{
	GENERATED_BODY()

public:
	bool InitObject(AActor* Actor, const FHitResult& Hit) override;
	void DestroyObject() override;

	virtual void ExecuteOnce();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ExecuteOnceEffects")
	float PowerOfOnceEffect = 10.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ExecuteOnceEffects")
	float DestroyEffectFXtime = 1.0f;

private:
	FTimerHandle DestroyEffectFXTimerHandle;
};



UCLASS()
class TDS_API UTDSStateEffect_ExecuteTimer : public UTDSStateEffect
{
	GENERATED_BODY()

public:
	bool InitObject(AActor* Actor, const FHitResult& Hit) override;
	void DestroyObject() override;

	virtual void ExecuteOnTimer();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ExecuteTimerEffects")
	float PowerOfTimerEffect = 20.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ExecuteTimerEffects")
	float EffectUpdateTime = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ExecuteTimerEffects")
	float EffectRate = 2.0f;

private:
	FTimerHandle ExecuteEffectlTimerHandle;

	float CurrentPowerOfTimerEffect = 20.0f;
};