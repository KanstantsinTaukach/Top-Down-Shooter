// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Interaction/TDSInterfaceGameActor.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void SetCanAttack(bool CanCharacterAttack) { CanAttack = CanCharacterAttack; };
	UFUNCTION(BlueprintCallable)
	bool GetCanAttack() const { return CanAttack; };

	UFUNCTION(BlueprintCallable)
	void SetMovement(bool CanCharacterMove, float Speed) { CanMove = CanCharacterMove; GetCharacterMovement()->MaxWalkSpeed = Speed; };
	UFUNCTION(BlueprintCallable)
	bool GetCanMove() const { return CanMove; };

protected:
	bool IsDead = false;
	bool CanAttack = true;
	bool CanMove = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxSpeed = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float HitSpeed = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float ConfusionSpeed = 0.0f;

	TArray<UTDSStateEffect*> StateEffects;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTDSHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTDSLootDropComponent* LootDropComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray<UAnimMontage*> DeathAnimations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray<UAnimMontage*> ConfusionAnimations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ConfusionChance = 0.2f;

	UFUNCTION()
	virtual void OnAICharacterDeath();

	virtual void BeginPlay() override;

private:
	FTimerHandle RagdollTimerHandle;
	FTimerHandle ConfusionTimerHandle;

	UFUNCTION()
	void OnTakeRadialDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo, class AController* InstigatedBy, AActor* DamageCauser);

	void EnableRagdoll();

	void EnableConfusion();
	void DisableConfusion();
};
