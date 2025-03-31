// Top Down Shooter. All Rights Reserved

#include "TDSAIJumpingEnemy.h"
#include "Kismet/GameplayStatics.h"

void ATDSAIJumpingEnemy::JumpAttack()
{
	if (!CanAttack || !JumpAttackAnimation || !GetMesh() || !GetMesh()->GetAnimInstance()) return;

	CanAttack = false;

	float AnimDuration = GetMesh()->GetAnimInstance()->Montage_Play(JumpAttackAnimation);

	FVector JumpImpulse = GetActorForwardVector() * JumpForwardForce + FVector::UpVector * JumpHeight;
	LaunchCharacter(JumpImpulse, false, true);

	GetWorld()->GetTimerManager().SetTimer(JumpAttackTimerHandle, this, &ATDSAIJumpingEnemy::PerformJumpAttackHitCheck, AnimDuration * 0.5, false);

	GetWorld()->GetTimerManager().SetTimer(JumpAttackTimerHandle, this, &ATDSAIJumpingEnemy::EndJumpAttack, AnimDuration, false);
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
	GetWorld()->GetTimerManager().ClearTimer(JumpAttackTimerHandle);
}
