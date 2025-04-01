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
	UFUNCTION(BlueprintCallable)
	void JumpAttack();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpAttack")
	UAnimMontage* JumpAttackAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpAttack")
	float JumpAttackDamage = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpAttack")
	float JumpAttackSpread = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpAttack")
	float JumpAttackRadius = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpAttack")
	float JumpHeight = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JumpAttack")
	float JumpForwardForce = 300.0f;

	UFUNCTION()
	void PerformJumpAttackHitCheck();

	UFUNCTION()
	void EndJumpAttack();

	virtual void InitAnimation() override;

	void NotifyJumpAttackHitConfirmed(USkeletalMeshComponent* MeshComponent);

private:
	FTimerHandle JumpAttackTimerHandle;
};
