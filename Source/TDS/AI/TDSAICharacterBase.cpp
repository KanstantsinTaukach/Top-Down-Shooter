// Top Down Shooter. All Rights Reserved

#include "TDSAICharacterBase.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Materials/MaterialInterface.h"
#include "../BuffSystem/TDSStateEffect.h"
#include "../Components/TDSHealthComponent.h"
#include "../Components/TDSLootDropComponent.h"
#include "../Weapons/TDS_ProjectileDefault.h"
#include "Components/CapsuleComponent.h"
#include "TDSAIController.h"

ATDSAICharacterBase::ATDSAICharacterBase(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ATDSAIController::StaticClass();

	PrimaryActorTick.bCanEverTick = false;

	HealthComponent = CreateDefaultSubobject<UTDSHealthComponent>("HealthComponent");
	LootDropComponent = CreateDefaultSubobject<UTDSLootDropComponent>("LootDropComponent");

	IsDead = false;
	CanAttack = true;

	SetMovement(true, MaxSpeed);
	GetCharacterMovement()->MaxAcceleration = 200.0f;
}

void ATDSAICharacterBase::BeginPlay()
{
	Super::BeginPlay();

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
	if (HealthComponent && !IsDead)
	{
		HealthComponent->RemoveFromCurrentHealth(Damage);

		float RandomChance = FMath::FRandRange(0.0f, 1.0f);
		if (RandomChance < ConfusionChance)
		{
			EnableConfusion();
		}
	}

	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	return ActualDamage;
}

void ATDSAICharacterBase::OnTakeRadialDamageHandle(AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector Origin, FHitResult HitInfo, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsDead)
	{
		ATDS_ProjectileDefault* TargetProjectile = Cast<ATDS_ProjectileDefault>(DamageCauser);
		if (TargetProjectile)
		{
			UTypes::AddEffectBySurfaceType(TargetProjectile->GetProjectileSettings().Effect, GetSurfaceType(), HitInfo);
		}
	}
}

void ATDSAICharacterBase::EnableConfusion()
{
	SetMovement(false, ConfusionSpeed);

	float AnimationTime = 0.0f;
	int32 RandomNumber = 0;
	if (ConfusionAnimations.Num() > 1)
	{
		RandomNumber = FMath::RandHelper(DeathAnimations.Num());
	}	

	if (ConfusionAnimations.IsValidIndex(RandomNumber) && ConfusionAnimations[RandomNumber] && GetMesh() && GetMesh()->GetAnimInstance() && GetWorld())
	{
		AnimationTime = ConfusionAnimations[RandomNumber]->GetPlayLength();
		GetMesh()->GetAnimInstance()->Montage_Play(ConfusionAnimations[RandomNumber]);

		if (IsDead)
		{
			OnAICharacterDeath();
		}
	}

	GetWorld()->GetTimerManager().SetTimer(ConfusionTimerHandle, this, &ATDSAICharacterBase::DisableConfusion, AnimationTime, false);
}

void ATDSAICharacterBase::DisableConfusion()
{
	GetWorld()->GetTimerManager().ClearTimer(ConfusionTimerHandle);
	SetMovement(true, MaxSpeed);
}

void ATDSAICharacterBase::OnAICharacterDeath()
{
	IsDead = true;

	float AnimationTime = 0.0f;
	int32 RandomNumber = 0;
	if (DeathAnimations.Num() > 1)
	{
		RandomNumber = FMath::RandHelper(DeathAnimations.Num());
	}
		
	if (DeathAnimations.IsValidIndex(RandomNumber) && DeathAnimations[RandomNumber] && GetMesh() && GetMesh()->GetAnimInstance())
	{
		AnimationTime = DeathAnimations[RandomNumber]->GetPlayLength();
		GetMesh()->GetAnimInstance()->Montage_Play(DeathAnimations[RandomNumber]);
	}

	if (GetController())
	{
		GetController()->UnPossess();
	}

	SetCanAttack(false);
	SetMovement(false, 0.0f);

	SetLifeSpan(10);

	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FVector DeathLocation = GetActorLocation();
	LootDropComponent->DropLoot(DeathLocation);

	if (GetWorld())
	{
		//GetWorld()->GetTimerManager().SetTimer(RagdollTimerHandle, this, &ATDSAICharacterBase::EnableRagdoll, AnimationTime, false);
	}
}

void ATDSAICharacterBase::EnableRagdoll()
{
	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetSimulatePhysics(true);
	}
}