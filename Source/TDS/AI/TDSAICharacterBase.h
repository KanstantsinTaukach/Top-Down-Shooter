// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../FunctionLibrary/Types.h"
#include "../Interaction/TDSInterfaceGameActor.h"
#include "../Interaction/TDSAIInterfaceCombat.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TDSAICharacterBase.generated.h"

class UTDSHealthComponent;
class UTDSLootDropComponent;
class UBehaviorTree;

UCLASS()
class TDS_API ATDSAICharacterBase : public ACharacter, public ITDSInterfaceGameActor, public ITDSAIInterfaceCombat
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;

	ATDSAICharacterBase(const FObjectInitializer& ObjInit);

	EPhysicalSurface GetSurfaceType() override;

	TArray<UTDSStateEffect*> GetAllCurrentEffects() override;

	void RemoveEffect(UTDSStateEffect* EffectToRemove) override;
	void AddEffect(UTDSStateEffect* EffectToAdd) override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	bool CheckCanAttack() const override { return CanAttack; };

	UFUNCTION(BlueprintCallable)
	void LightAttack() override;
	UFUNCTION(BlueprintCallable)
	void HeavyAttack() override;

protected:
	bool IsDead = false;
	bool IsConfused = false;
	bool IsDamaged = false;
	bool CanAttack = false;

	TArray<UTDSStateEffect*> StateEffects;

	EAIAttackType CurrentAttackType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTDSHealthComponent* HealthComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTDSLootDropComponent* LootDropComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray<UAnimMontage*> DeathAnimations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray<UAnimMontage*> ConfusionAnimations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray<UAnimMontage*> HitAnimations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray<UAnimMontage*> LightAttackAnimations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray<UAnimMontage*> HeavyAttackAnimations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ConfusionChance = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HitChance = 0.1f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	EAIMovementState AIMovementState = EAIMovementState::Run_State;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FAISpeed AISpeedInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	FAIAttackParams LightAttackParams;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	FAIAttackParams HeavyAttackParams;

	UFUNCTION()
	virtual void OnAICharacterDeath();

	virtual void BeginPlay() override;

	void NotifyAttackHitConfirmed(USkeletalMeshComponent* MeshComponent);

	void PerformAttackTrace(const FAIAttackParams& Params);

	float GetRandomizedDamage(float BaseDamage, float Spread);

	virtual void InitAnimation();

	void ChangeMovementState(EAIMovementState InMovementState);

private:
	FTimerHandle RagdollTimerHandle;
	FTimerHandle ConfusionTimerHandle;
	FTimerHandle HitTimerHandle;
	FTimerHandle AttackTimerHandle;

	UFUNCTION()
	void OnTakeRadialDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo, class AController* InstigatedBy, AActor* DamageCauser);

	void EnableRagdoll();

	void EnableConfusion();
	void DisableConfusion();

	void EnableHit();
	void DisableHit();

	void CheckAnimationArrays();

	void AttackCompleted();

	void InitAttackParams();
};
