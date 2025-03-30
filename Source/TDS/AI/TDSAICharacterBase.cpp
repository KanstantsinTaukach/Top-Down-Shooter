// Top Down Shooter. All Rights Reserved

#include "TDSAICharacterBase.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Materials/MaterialInterface.h"
#include "../BuffSystem/TDSStateEffect.h"
#include "../Components/TDSHealthComponent.h"
#include "../Components/TDSLootDropComponent.h"
#include "../Weapons/TDS_ProjectileDefault.h"
#include "Components/CapsuleComponent.h"
#include "../FunctionLibrary/AnimUtils.h"
#include "TDSAIController.h"
#include "../Animations/TDSAIAttackAnimNotify.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(TDSAICharacterBaseLog, All, All);

ATDSAICharacterBase::ATDSAICharacterBase(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ATDSAIController::StaticClass();

	bUseControllerRotationYaw = false;
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 180.0f, 0.0f);
	}

	PrimaryActorTick.bCanEverTick = false;

	HealthComponent = CreateDefaultSubobject<UTDSHealthComponent>("HealthComponent");
	LootDropComponent = CreateDefaultSubobject<UTDSLootDropComponent>("LootDropComponent");

	IsDead = false;
	IsConfused = false;
	IsDamaged = false;
	CanAttack = true;

	GetCharacterMovement()->MaxAcceleration = 500.0f;

	//ChangeMovementState(EAIMovementState::Run_State);

	InitAttackParams();

	ECollisionChannel MeleeAttackChannel = ECC_GameTraceChannel4;

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionResponseToChannel(MeleeAttackChannel, ECR_Ignore);
	}

	if (USkeletalMeshComponent* SkeletalMesh = GetMesh())
	{
		SkeletalMesh->SetCollisionResponseToChannel(MeleeAttackChannel, ECR_Ignore);
	}
}

void ATDSAICharacterBase::BeginPlay()
{
	Super::BeginPlay();

	CheckAnimationArrays();

	InitAnimation();

	check(LootDropComponent);

	check(HealthComponent);
	HealthComponent->OnDeath.AddDynamic(this, &ATDSAICharacterBase::OnAICharacterDeath);

	OnTakeRadialDamage.AddDynamic(this, &ATDSAICharacterBase::OnTakeRadialDamageHandle);
}

EPhysicalSurface ATDSAICharacterBase::GetSurfaceType()
{
	EPhysicalSurface Result = ITDSInterfaceGameActor::GetSurfaceType();

	if (HealthComponent)
	{
		if (GetMesh())
		{
			UMaterialInterface* MyMaterial = GetMesh()->GetMaterial(0);
			if (MyMaterial)
			{
				Result = MyMaterial->GetPhysicalMaterial()->SurfaceType;
			}
		}
	}

	return Result;
}

TArray<UTDSStateEffect*> ATDSAICharacterBase::GetAllCurrentEffects()
{
	return StateEffects;
}

void ATDSAICharacterBase::RemoveEffect(UTDSStateEffect* EffectToRemove)
{
	StateEffects.Remove(EffectToRemove);
}

void ATDSAICharacterBase::AddEffect(UTDSStateEffect* EffectToAdd)
{
	StateEffects.Add(EffectToAdd);
}

float ATDSAICharacterBase::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!HealthComponent || IsDead)
	{
		return 0.0f;
	}
		
	HealthComponent->RemoveFromCurrentHealth(Damage);

	if(!IsConfused && !IsDamaged && !IsDead)
	{
		float RandomConfusionChance = FMath::FRandRange(0.0f, 1.0f);
		if (RandomConfusionChance < ConfusionChance)
		{
			EnableConfusion();
		}
		else
		{
			float RandomHitChance = FMath::FRandRange(0.0f, 1.0f);
			if (RandomHitChance < HitChance)
			{
				EnableHit();
			}
		}
	}

	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

void ATDSAICharacterBase::OnTakeRadialDamageHandle(AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector Origin, FHitResult HitInfo, AController* InstigatedBy, AActor* DamageCauser)
{
	ATDS_ProjectileDefault* TargetProjectile = Cast<ATDS_ProjectileDefault>(DamageCauser);
	if (TargetProjectile)
	{
		UTypes::AddEffectBySurfaceType(TargetProjectile->GetProjectileSettings().Effect, GetSurfaceType(), HitInfo);
	}
}

void ATDSAICharacterBase::EnableConfusion()
{
	IsConfused = true;
	ChangeMovementState(EAIMovementState::CantMove_State);

	UAnimMontage* RandomAnimation = AnimUtils::GetRandomAnimation(ConfusionAnimations);
	if (RandomAnimation && GetMesh() && GetMesh()->GetAnimInstance())
	{
		float AnimationTime = RandomAnimation->GetPlayLength();
		GetMesh()->GetAnimInstance()->Montage_Play(RandomAnimation);

		GetWorld()->GetTimerManager().SetTimer(ConfusionTimerHandle, this, &ATDSAICharacterBase::DisableConfusion, AnimationTime, false);
	}
}

void ATDSAICharacterBase::DisableConfusion()
{
	GetWorld()->GetTimerManager().ClearTimer(ConfusionTimerHandle);

	IsConfused = false;
	ChangeMovementState(EAIMovementState::Run_State);
}

void ATDSAICharacterBase::EnableHit()
{
	IsDamaged = true;
	ChangeMovementState(EAIMovementState::Hit_State);

	UAnimMontage* RandomAnimation = AnimUtils::GetRandomAnimation(HitAnimations);
	if (RandomAnimation && GetMesh() && GetMesh()->GetAnimInstance())
	{
		float AnimationTime = RandomAnimation->GetPlayLength();
		GetMesh()->GetAnimInstance()->Montage_Play(RandomAnimation);

		GetWorld()->GetTimerManager().SetTimer(HitTimerHandle, this, &ATDSAICharacterBase::DisableHit, AnimationTime, false);
	}
}

void ATDSAICharacterBase::DisableHit()
{
	GetWorld()->GetTimerManager().ClearTimer(HitTimerHandle);

	IsDamaged = false;
	ChangeMovementState(EAIMovementState::Run_State);
}

void ATDSAICharacterBase::OnAICharacterDeath()
{
	if (IsDead)
	{
		return;
	}

	IsDead = true;

	UAnimMontage* RandomAnimation = AnimUtils::GetRandomAnimation(DeathAnimations);
	if (RandomAnimation && GetMesh() && GetMesh()->GetAnimInstance())
	{
		float AnimationTime = RandomAnimation->GetPlayLength();
		GetMesh()->GetAnimInstance()->Montage_Play(RandomAnimation);

		//GetWorld()->GetTimerManager().SetTimer(RagdollTimerHandle, this, &ATDSAICharacterBase::EnableRagdoll, AnimationTime, false);
	}

	if (GetController())
	{
		GetController()->UnPossess();
	}

	SetLifeSpan(10);

	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FVector DeathLocation = GetActorLocation();
	LootDropComponent->DropLoot(DeathLocation);
}

void ATDSAICharacterBase::EnableRagdoll()
{
	if (IsDead && GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetSimulatePhysics(true);
	}
}

void ATDSAICharacterBase::CheckAnimationArrays()
{
	ensureMsgf(DeathAnimations.Num() > 0, TEXT("DeathAnimations array is empty in %s!"), *GetName());
	ensureMsgf(ConfusionAnimations.Num() > 0, TEXT("ConfusionAnimations array is empty in %s!"), *GetName());
	ensureMsgf(HitAnimations.Num() > 0, TEXT("HitAnimations array is empty in %s!"), *GetName());
	ensureMsgf(LightAttackAnimations.Num() > 0, TEXT("LightAttackAnimations array is empty in %s!"), *GetName());
	ensureMsgf(HeavyAttackAnimations.Num() > 0, TEXT("HeavyAttackAnimations array is empty in %s!"), *GetName());
}

void ATDSAICharacterBase::ChangeMovementState(EAIMovementState InAIMovementState)
{
	AIMovementState = InAIMovementState;
	
	float ResSpeed = 500.0f;
	switch (AIMovementState)
	{
	case EAIMovementState::CantMove_State:	ResSpeed = AISpeedInfo.CantMoveSpeed;	break;
	case EAIMovementState::Hit_State:		ResSpeed = AISpeedInfo.HitSpeed;		break;
	case EAIMovementState::Run_State:		ResSpeed = AISpeedInfo.RunSpeed;		break;
	default: break;
	}

	GetCharacterMovement()->MaxWalkSpeed = ResSpeed;
}

void ATDSAICharacterBase::LightAttack()
{
	CurrentAttackType = EAIAttackType::Light;
	CanAttack = false;

	UAnimMontage* RandomAnimation = AnimUtils::GetRandomAnimation(LightAttackAnimations);
	if (RandomAnimation && GetMesh() && GetMesh()->GetAnimInstance())
	{
		float AnimationTime = RandomAnimation->GetPlayLength();
		GetMesh()->GetAnimInstance()->Montage_Play(RandomAnimation);

		GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &ATDSAICharacterBase::AttackCompleted, AnimationTime, false);
	}
}

void ATDSAICharacterBase::HeavyAttack()
{
	CurrentAttackType = EAIAttackType::Heavy;
	CanAttack = false;
	ChangeMovementState(EAIMovementState::CantMove_State);

	UAnimMontage* RandomAnimation = AnimUtils::GetRandomAnimation(HeavyAttackAnimations);
	if (RandomAnimation && GetMesh() && GetMesh()->GetAnimInstance())
	{
		float AnimationTime = RandomAnimation->GetPlayLength();
		GetMesh()->GetAnimInstance()->Montage_Play(RandomAnimation);

		GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &ATDSAICharacterBase::AttackCompleted, AnimationTime, false);
	}
}

void ATDSAICharacterBase::AttackCompleted()
{
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);

	CanAttack = true;
	if (AIMovementState == EAIMovementState::CantMove_State)
	{
		ChangeMovementState(EAIMovementState::Run_State);
	}
}

void ATDSAICharacterBase::InitAnimation()
{
	for (auto LightAttackAnimation : LightAttackAnimations)
	{
		const auto NotifyEvents = LightAttackAnimation->Notifies;
		for (auto NotifyEvent : NotifyEvents)
		{
			auto AttackAnimNotify = Cast<UTDSAIAttackAnimNotify>(NotifyEvent.Notify);
			if (AttackAnimNotify)
			{
				AttackAnimNotify->OnNotified.AddUObject(this, &ATDSAICharacterBase::NotifyAttackHitConfirmed);
				break;
			}
		}
	}

	for (auto HeavyAttackAnimation : HeavyAttackAnimations)
	{
		const auto NotifyEvents = HeavyAttackAnimation->Notifies;
		for (auto NotifyEvent : NotifyEvents)
		{
			auto AttackAnimNotify = Cast<UTDSAIAttackAnimNotify>(NotifyEvent.Notify);
			if (AttackAnimNotify)
			{
				AttackAnimNotify->OnNotified.AddUObject(this, &ATDSAICharacterBase::NotifyAttackHitConfirmed);
				break;
			}
		}
	}
	
}

void ATDSAICharacterBase::NotifyAttackHitConfirmed(USkeletalMeshComponent* MeshComponent)
{
	if (this->GetMesh() != MeshComponent) return;

	switch (CurrentAttackType)
	{
		case EAIAttackType::Light:
			PerformAttackTrace(LightAttackParams);
			break;
		case EAIAttackType::Heavy:
			PerformAttackTrace(HeavyAttackParams);
			break;
	}

}

void ATDSAICharacterBase::PerformAttackTrace(const FAIAttackParams& Params)
{
	FVector TraceStart = GetActorLocation() + GetActorForwardVector() * 50.0f;
	FVector TraceEnd = GetActorLocation() + GetActorForwardVector() * 150.0f;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	ETraceTypeQuery MeleeAttackQuery = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel4);

	FHitResult HitResult;
	bool Hit = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		TraceStart,
		TraceEnd,
		Params.Radius,
		MeleeAttackQuery,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResult,
		true
	);

	if (Hit && HitResult.GetActor() && HitResult.GetActor()->ActorHasTag("Player"))
	{
		float FinalDamage = GetRandomizedDamage(Params.Damage, Params.Spread);

		UGameplayStatics::ApplyDamage(HitResult.GetActor(), FinalDamage, GetController(), this, NULL);
	}

	if (HitResult.PhysMaterial.IsValid())
	{
		EPhysicalSurface SurfaceType = UGameplayStatics::GetSurfaceType(HitResult);
		// add FX
		// add sound
	}
}

void ATDSAICharacterBase::InitAttackParams()
{
	LightAttackParams.Damage = 20.0f;
	LightAttackParams.Spread = 10.0f;
	LightAttackParams.Radius = 25.0f;

	HeavyAttackParams.Damage = 50.0f;
	HeavyAttackParams.Spread = 20.0f;
	HeavyAttackParams.Radius = 75.0f;
}

float ATDSAICharacterBase::GetRandomizedDamage(float BaseDamage, float Spread)
{
	if (BaseDamage <= 0.0f) return 0.0f;
	if (Spread < 0.0f) Spread = 0.0f;

	float RandomFactor = FMath::RandRange(-1.0f, 1.0f);
	RandomFactor = FMath::Sin(RandomFactor * PI / 2);

	float FinalDamage = BaseDamage + Spread * RandomFactor;

	return FMath::RoundToFloat(FMath::Max(FinalDamage, 1.0f));
}
