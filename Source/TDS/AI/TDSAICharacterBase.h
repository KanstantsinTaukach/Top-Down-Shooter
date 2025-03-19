// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Interaction/TDSInterfaceGameActor.h"
#include "TDSAICharacterBase.generated.h"

class UTDSHealthComponent;
class UTDSLootDropComponent;

UCLASS()
class TDS_API ATDSAICharacterBase : public ACharacter, public ITDSInterfaceGameActor
{
	GENERATED_BODY()

public:
	ATDSAICharacterBase(const FObjectInitializer& ObjInit);

	EPhysicalSurface GetSurfaceType() override;

	TArray<UTDSStateEffect*> GetAllCurrentEffects() override;

	void RemoveEffect(UTDSStateEffect* EffectToRemove) override;
	void AddEffect(UTDSStateEffect* EffectToAdd) override;

	void SetCanAttack(bool CanCharacterAttack) { CanAttack = CanCharacterAttack; };
	bool GetCanAttack() const { return CanAttack; };

	void SetCanMove(bool CanCharacterMove) { CanMove = CanCharacterMove; };
	bool GetCanMove() const { return CanMove; };

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	bool IsDead = false;
	bool CanMove = false;
	bool CanAttack = false;

	TArray<UTDSStateEffect*> StateEffects;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTDSHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTDSLootDropComponent* LootDropComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray<UAnimMontage*> DeathAnimations;

	UFUNCTION()
	virtual void OnAICharacterDeath();

	virtual void BeginPlay() override;

private:
	FTimerHandle RagdollTimerHandle;

	UFUNCTION()
	void OnTakeRadialDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo, class AController* InstigatedBy, AActor* DamageCauser);

	void EnableRagdoll();

};
