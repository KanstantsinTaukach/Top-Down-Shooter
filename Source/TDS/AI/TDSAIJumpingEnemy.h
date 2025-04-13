// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "TDSAICharacterBase.h"
#include "TDSAIJumpingEnemy.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class TDS_API ATDSAIJumpingEnemy : public ATDSAICharacterBase
{
	GENERATED_BODY()

public:
	ATDSAIJumpingEnemy(const FObjectInitializer& ObjInit);

	UFUNCTION(BlueprintCallable, Category = "AI|JumpAttack")
	bool JumpAttack(AActor* TargetCharacter);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpAttack")
	UAnimMontage* JumpAttackAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpAttack", meta = (ClampMin = 0.0f))
	float JumpAttackDamage = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpAttack", meta = (ClampMin = 0.0f))
	float JumpAttackSpread = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpAttack", meta = (ClampMin = 0.0f))
	float JumpAttackRadius = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpAttack", meta = (ClampMin = 0.0f))
	float JumpHeight = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpAttack", meta = (ClampMin = 0.0f))
	float JumpDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpAttack", meta = (ClampMin = 1.0f))
	float FallingGravityMultiplier = 2.5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effects|JumpAttack")
	UNiagaraSystem* JumpAttackEffectFX;

	UNiagaraComponent* NiagaraJumpAttackEffect;

	UFUNCTION()
	void PerformJumpAttackHitCheck();

	UFUNCTION()
	void EndJumpAttack();

	virtual void InitAnimation() override;

	void NotifyJumpAttackHitConfirmed(USkeletalMeshComponent* MeshComponent);

	virtual void Tick(float DeltaTime) override;

	FVector CalculateLaunchVelocity(const AActor* Target) const;

private:
	bool IsJumpAttacking = false;

	float InitialGravityScale = 1.0f;

	FTimerHandle JumpAttackTimerHandle;
};
