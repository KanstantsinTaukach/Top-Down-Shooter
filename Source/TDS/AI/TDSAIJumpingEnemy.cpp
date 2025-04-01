// Top Down Shooter. All Rights Reserved

#include "TDSAIJumpingEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "../Animations/TDSAIAttackAnimNotify.h"

void ATDSAIJumpingEnemy::JumpAttack()
{
	if (!CanAttack || !JumpAttackAnimation || !GetMesh() || !GetMesh()->GetAnimInstance()) return;

	CanAttack = false;

	float AnimDuration = GetMesh()->GetAnimInstance()->Montage_Play(JumpAttackAnimation);

	FVector LaunchVelocity = GetActorForwardVector() * JumpForwardForce + FVector::UpVector * JumpHeight;
	LaunchCharacter(LaunchVelocity, true, true);

	GetWorld()->GetTimerManager().SetTimer(JumpAttackTimerHandle, this, &ATDSAIJumpingEnemy::EndJumpAttack, AnimDuration, false);
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
#if ENABLE_DRAW_DEBUG
		EDrawDebugTrace::ForDuration,
#else
		EDrawDebugTrace::None,
#endif
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
	if (JumpAttackTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(JumpAttackTimerHandle);
	}
}