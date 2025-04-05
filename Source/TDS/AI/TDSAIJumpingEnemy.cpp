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

ATDSAIJumpingEnemy::ATDSAIJumpingEnemy(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	CachedMovementComponent = GetCharacterMovement();
	if (CachedMovementComponent)
	{
		InitialGravityScale = GetCharacterMovement()->GravityScale;
	}
}

ACharacter* ATDSAIJumpingEnemy::GetTarget() const
{
	const auto MyController = Cast<AAIController>(GetController());
	if (!MyController) return nullptr;

	const auto BlackboardComponent = MyController->GetBlackboardComponent();
	if (!BlackboardComponent) return nullptr;

	FName EnemyActorKeyName = "EnemyActor";
	UObject* TargetObject = BlackboardComponent->GetValueAsObject(EnemyActorKeyName);
	if (!TargetObject) return nullptr;

	ACharacter* TargetCharacter = Cast<ACharacter>(TargetObject);
	if (!TargetCharacter) return nullptr;

	return TargetCharacter;
}

void ATDSAIJumpingEnemy::JumpAttack()
{
	if (!CanAttack || IsJumpAttackOnCooldown || !JumpAttackAnimation || !GetMesh() || !GetMesh()->GetAnimInstance() || !CachedMovementComponent) return;

	ACharacter* Target = GetTarget();
	if (!Target) return;

	CanAttack = false;
	IsJumpAttacking = true;
	IsJumpAttackOnCooldown = true;

	GetWorld()->GetTimerManager().SetTimer(JumpAttackCooldownTimerHandle, this, &ATDSAIJumpingEnemy::ResetJumpAttackCooldown, JumpAttackCooldown, false);

	FVector SelfLocation = GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(SelfLocation, TargetLocation);
	LookAtRotation.Pitch = GetActorRotation().Pitch;
	LookAtRotation.Roll = GetActorRotation().Roll;
	SetActorRotation(LookAtRotation);

	FVector LaunchVelocity = CalculateLaunchVelocity(Target);
	if (LaunchVelocity.IsNearlyZero()) return;

	LaunchCharacter(LaunchVelocity, true, true);

	CachedMovementComponent->bOrientRotationToMovement = false;
	CachedMovementComponent->bUseControllerDesiredRotation = false;

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
}

FVector ATDSAIJumpingEnemy::CalculateLaunchVelocity(const ACharacter* Target) const
{
	if (!Target || !CachedMovementComponent) return FVector::ZeroVector;

	FVector LaunchDirection = Target->GetActorLocation() - GetActorLocation();
	LaunchDirection.Z = 0.0f;
	float HorizontalDistance = LaunchDirection.Size();
	LaunchDirection.Normalize();

	float VerticalSpeed = FMath::Sqrt(2.0 * FMath::Abs(CachedMovementComponent->GetGravityZ()) * JumpHeight);
	float TimeInAir = (2.0f * VerticalSpeed) / FMath::Abs(CachedMovementComponent->GetGravityZ());

	float HorizontalSpeed = JumpDistance / FMath::Max(TimeInAir, 0.1f);
	float SpeeedToTarget = HorizontalDistance / FMath::Max(TimeInAir, 0.1f);
	HorizontalSpeed = FMath::Min(HorizontalSpeed, SpeeedToTarget);

	FVector FinalVelocity = LaunchDirection * HorizontalSpeed + FVector::UpVector * VerticalSpeed;
	return FinalVelocity;
}

void ATDSAIJumpingEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsJumpAttacking && GetMesh() && GetMesh()->GetAnimInstance() && CachedMovementComponent)
	{
		if (CachedMovementComponent->Velocity.Z < 0)
		{
			CachedMovementComponent->GravityScale = InitialGravityScale * FallingGravityMultiplier;
		}
		else
		{
			CachedMovementComponent->GravityScale = InitialGravityScale;
		}
	}
}

void ATDSAIJumpingEnemy::EndJumpAttack()
{
	CanAttack = true;
	IsJumpAttacking = false;

	if (CachedMovementComponent)
	{
		CachedMovementComponent->bOrientRotationToMovement = true;
		CachedMovementComponent->bUseControllerDesiredRotation = true;
		CachedMovementComponent->GravityScale = InitialGravityScale;
		CachedMovementComponent->Velocity = FVector::ZeroVector;
		CachedMovementComponent->StopMovementImmediately();
	}

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