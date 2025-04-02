// Top Down Shooter. All Rights Reserved

#include "TDSAIJumpingEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "../Animations/TDSAIAttackAnimNotify.h"
#include "Components/CapsuleComponent.h"

ATDSAIJumpingEnemy::ATDSAIJumpingEnemy(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	InitialGravityScale = GetCharacterMovement()->GravityScale;
}

void ATDSAIJumpingEnemy::JumpAttack()
{
	if (!CanAttack || IsJumpAttackOnCooldown || !JumpAttackAnimation || !GetMesh() || !GetMesh()->GetAnimInstance()) return;

	CanAttack = false;
	IsJumpAttacking = true;
	IsJumpAttackOnCooldown = true;

	GetWorld()->GetTimerManager().SetTimer(JumpAttackCooldownTimerHandle, this, &ATDSAIJumpingEnemy::ResetJumpAttackCooldown, JumpAttackCooldown, false);

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}

	float AnimDuration = GetMesh()->GetAnimInstance()->Montage_Play(JumpAttackAnimation);

	SetActorTickEnabled(true);

	GetWorld()->GetTimerManager().SetTimer(JumpAttackTimerHandle, this, &ATDSAIJumpingEnemy::EndJumpAttack, AnimDuration, false);
}

void ATDSAIJumpingEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsJumpAttacking && GetMesh() && GetMesh()->GetAnimInstance())
	{
		float CurveValue = GetMesh()->GetAnimInstance()->GetCurveValue(JumpPowerCurveName);

		if (CurveValue > 0.1f)
		{
			FVector Force = GetActorForwardVector() * JumpDistance * CurveValue + FVector::UpVector * JumpHeight * CurveValue;

			GetCharacterMovement()->AddImpulse(Force * DeltaTime, true);

			float NewGravityScale = (GetCharacterMovement()->Velocity.Z < 0) ? InitialGravityScale * 2.5 : InitialGravityScale;
			GetCharacterMovement()->GravityScale = NewGravityScale;
		}
	}
}

void ATDSAIJumpingEnemy::EndJumpAttack()
{
	CanAttack = true;
	IsJumpAttacking = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->GravityScale = InitialGravityScale;

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}

	SetActorTickEnabled(false);

	if (JumpAttackTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(JumpAttackTimerHandle);
	}
}

void ATDSAIJumpingEnemy::ResetJumpAttackCooldown()
{
	IsJumpAttackOnCooldown = false;
}

void ATDSAIJumpingEnemy::InitAnimation()
{
	if (!JumpAttackAnimation) return;

	const auto NotifyEvents = JumpAttackAnimation->Notifies;
	for (auto NotifyEvent : NotifyEvents)
	{
		auto AttackAnimNotify = Cast<UTDSAIAttackAnimNotify>(NotifyEvent.Notify);
		if (AttackAnimNotify)
		{
			AttackAnimNotify->OnNotified.AddUObject(this, &ATDSAIJumpingEnemy::NotifyJumpAttackHitConfirmed);
			break;
		}
	}
}

void ATDSAIJumpingEnemy::NotifyJumpAttackHitConfirmed(USkeletalMeshComponent* MeshComponent)
{
	if (this->GetMesh() != MeshComponent) return;

	PerformJumpAttackHitCheck();
}

void ATDSAIJumpingEnemy::PerformJumpAttackHitCheck()
{
	if (!GetWorld()) return;

	FVector TraceStart = GetActorLocation();
	FVector TraceEnd = TraceStart + FVector::DownVector * 50.0f;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	ETraceTypeQuery MeleeAttackQuery = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel4);

	FHitResult HitResult;
	bool Hit = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		TraceStart,
		TraceEnd,
		JumpAttackRadius,
		MeleeAttackQuery,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResult,
		true
	);

	if (Hit && HitResult.GetActor())
	{
		float FinalDamage = ATDSAICharacterBase::GetRandomizedDamage(JumpAttackDamage, JumpAttackSpread);

		UGameplayStatics::ApplyDamage(HitResult.GetActor(), FinalDamage, GetController(), this, NULL);
	}

	if (HitResult.PhysMaterial.IsValid())
	{
		EPhysicalSurface SurfaceType = UGameplayStatics::GetSurfaceType(HitResult);
		// add FX
		// add sound
	}
}