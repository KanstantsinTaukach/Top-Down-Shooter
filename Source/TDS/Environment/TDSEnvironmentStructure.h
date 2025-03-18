// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interaction/TDSInterfaceGameActor.h"
#include "TDSEnvironmentStructure.generated.h"

class UTDSStateEffect;
class UTDSHealthComponent;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class TDS_API ATDSEnvironmentStructure : public AActor, public ITDSInterfaceGameActor
{
	GENERATED_BODY()
	
public:	
	ATDSEnvironmentStructure();

	EPhysicalSurface GetSurfaceType() override;

	TArray<UTDSStateEffect*> GetAllCurrentEffects() override;

	void RemoveEffect(UTDSStateEffect* EffectToRemove) override;
	void AddEffect(UTDSStateEffect* EffectToAdd) override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	TArray<UTDSStateEffect*> StateEffects;

	UNiagaraComponent* NiagaraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTDSHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Destruction")
	USoundBase* DestructionSound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintreadWrite, Category = "Destruction")
	UNiagaraSystem* DestructionVFX;

	UFUNCTION()
	virtual void OnObjectDestruction();	

	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnTakeRadialDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo, class AController* InstigatedBy, AActor* DamageCauser);
};
