// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "TDSAICharacterBase.h"
#include "TDSAIJumpingEnemy.generated.h"

UCLASS()
class TDS_API ATDSAIJumpingEnemy : public ATDSAICharacterBase
{
	GENERATED_BODY()

public:
	ATDSAIJumpingEnemy(const FObjectInitializer& ObjInit);

	UFUNCTION(BlueprintCallable, Category = "AI|JumpAttack")
	void JumpAttack();

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
	float JumpAttackCooldown = 7.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpAttack", meta = (ClampMin = 1.0f))
	float FallingGravityMultiplier = 2.5;

	UFUNCTION()
	void PerformJumpAttackHitCheck();

	UFUNCTION()
	void EndJumpAttack();

	UFUNCTION()
	void ResetJumpAttackCooldown();

	virtual void InitAnimation() override;

	void NotifyJumpAttackHitConfirmed(USkeletalMeshComponent* MeshComponent);

	virtual void Tick(float DeltaTime) override;

	virtual ACharacter* GetTarget() const;

	FVector CalculateLaunchVelocity(const ACharacter* Target) const;

private:
	UPROPERTY()
	UCharacterMovementComponent* CachedMovementComponent = nullptr;

	bool IsJumpAttacking = false;
	bool IsJumpAttackOnCooldown = false;

	float InitialGravityScale = 1.0f;

	FTimerHandle JumpAttackTimerHandle;
	FTimerHandle JumpAttackCooldownTimerHandle;
};
