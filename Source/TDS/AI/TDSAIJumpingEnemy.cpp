// Top Down Shooter. All Rights Reserved

#include "TDSAIJumpingEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "../Animations/TDSAIAttackAnimNotify.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "TDSAIController.h"

ATDSAIJumpingEnemy::ATDSAIJumpingEnemy(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	AIControllerClass = ATDSAIController::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	InitialGravityScale = GetCharacterMovement()->GravityScale;

	GetCharacterMovement()->MaxAcceleration = 800.0f;
}

bool ATDSAIJumpingEnemy::JumpAttack(AActor* TargetActor)
{
	if (!CanAttack || !JumpAttackAnimation || !GetMesh() || !GetMesh()->GetAnimInstance() || !TargetActor) return false;

	CanAttack = false;
	IsJumpAttacking = true;

	FVector SelfLocation = GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(SelfLocation, TargetLocation);
	LookAtRotation.Pitch = GetActorRotation().Pitch;
	LookAtRotation.Roll = GetActorRotation().Roll;
	SetActorRotation(LookAtRotation);

	FVector LaunchVelocity = CalculateLaunchVelocity(TargetActor);
	if (LaunchVelocity.IsNearlyZero()) return false;

	LaunchCharacter(LaunchVelocity, true, true);

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}

	float AnimDuration = GetMesh()->GetAnimInstance()->Montage_Play(JumpAttackAnimation);
	if (AnimDuration > 0.0f)
	{
		SetActorTickEnabled(true);
		GetWorld()->GetTimerManager().SetTimer(JumpAttackTimerHandle, this, &ATDSAIJumpingEnemy::EndJumpAttack, AnimDuration, false);
	}
	else
	{
		EndJumpAttack();
		SetActorTickEnabled(false);
	}

	return true;
}

FVector ATDSAIJumpingEnemy::CalculateLaunchVelocity(const AActor* Target) const
{
	if (!Target) return FVector::ZeroVector;

	FVector LaunchDirection = Target->GetActorLocation() - GetActorLocation();
	LaunchDirection.Z = 0.0f;
	float HorizontalDistance = LaunchDirection.Size();
	LaunchDirection.Normalize();

	float VerticalSpeed = FMath::Sqrt(2.0 * FMath::Abs(GetCharacterMovement()->GetGravityZ()) * JumpHeight);
	float TimeInAir = (2.0f * VerticalSpeed) / FMath::Abs(GetCharacterMovement()->GetGravityZ());

	float HorizontalSpeed = JumpDistance / FMath::Max(TimeInAir, 0.1f);
	float SpeeedToTarget = HorizontalDistance / FMath::Max(TimeInAir, 0.1f);
	HorizontalSpeed = FMath::Min(HorizontalSpeed, SpeeedToTarget);

	FVector FinalVelocity = LaunchDirection * HorizontalSpeed + FVector::UpVector * VerticalSpeed;
	return FinalVelocity;
}

void ATDSAIJumpingEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsJumpAttacking && GetMesh() && GetMesh()->GetAnimInstance())
	{
		if (GetCharacterMovement()->Velocity.Z < 0)
		{
			GetCharacterMovement()->GravityScale = InitialGravityScale * FallingGravityMultiplier;
		}
		else
		{
			GetCharacterMovement()->GravityScale = InitialGravityScale;
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
	GetCharacterMovement()->Velocity = FVector::ZeroVector;
	GetCharacterMovement()->StopMovementImmediately();

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}

	SetActorTickEnabled(false);

	if (JumpAttackTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(JumpAttackTimerHandle);
	}

	if (JumpAttackEffectFX)
	{
		NiagaraJumpAttackEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), JumpAttackEffectFX, GetActorLocation());

		if (!NiagaraJumpAttackEffect)
		{
			return;
		}
	}
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

	Super::InitAnimation();
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