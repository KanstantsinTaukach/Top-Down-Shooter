// Top Down Shooter. All Rights Reserved

#include "TDSAIJumpingEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "../Animations/TDSAIAttackAnimNotify.h"

ATDSAIJumpingEnemy::ATDSAIJumpingEnemy(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ATDSAIJumpingEnemy::JumpAttack()
{
	if (!CanAttack || !JumpAttackAnimation || !GetMesh() || !GetMesh()->GetAnimInstance()) return;

	CanAttack = false;
	IsJumpAttacking = true;

	InitialGravityScale = GetCharacterMovement()->GravityScale;

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
			FVector Force = GetActorForwardVector() * JumpForwardForce * CurveValue + FVector::UpVector * JumpHeight * CurveValue;

			GetCharacterMovement()->AddImpulse(Force * DeltaTime, true);
		}

		float CurrentGravity = FMath::Lerp(InitialGravityScale, InitialGravityScale * 2.5f, GetCharacterMovement()->Velocity.Z < 0 ? 1 : 0);
		GetCharacterMovement()->GravityScale = CurrentGravity;
	}
}

void ATDSAIJumpingEnemy::InitAnimation()
{
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

void ATDSAIJumpingEnemy::EndJumpAttack()
{
	CanAttack = true;
	IsJumpAttacking = false;

	SetActorTickEnabled(false);

	GetCharacterMovement()->GravityScale = InitialGravityScale;

	if (JumpAttackTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(JumpAttackTimerHandle);
	}
}