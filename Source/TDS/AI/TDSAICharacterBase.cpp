// Top Down Shooter. All Rights Reserved

#include "TDSAICharacterBase.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Materials/MaterialInterface.h"
#include "../BuffSystem/TDSStateEffect.h"
#include "../Components/TDSHealthComponent.h"
#include "../Weapons/TDS_ProjectileDefault.h"
#include "Components/CapsuleComponent.h"

ATDSAICharacterBase::ATDSAICharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	HealthComponent = CreateDefaultSubobject<UTDSHealthComponent>("HealthComponent");
}

void ATDSAICharacterBase::BeginPlay()
{
	Super::BeginPlay();

	IsDead = false;
	
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
	if (IsDead) return 0.0f;

	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (HealthComponent && !IsDead)
	{
		HealthComponent->RemoveFromCurrentHealth(Damage);
	}
	return ActualDamage;
}

void ATDSAICharacterBase::OnTakeRadialDamageHandle(AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector Origin, FHitResult HitInfo, AController* InstigatedBy, AActor* DamageCauser)
{
	if (IsDead) return;

	ATDS_ProjectileDefault* TargetProjectile = Cast<ATDS_ProjectileDefault>(DamageCauser);
	if (TargetProjectile)
	{
		UTypes::AddEffectBySurfaceType(TargetProjectile->GetProjectileSettings().Effect, GetSurfaceType(), HitInfo);
	}
}

void ATDSAICharacterBase::OnAICharacterDeath()
{
	IsDead = true;

	float AnimationTime = 0.0f;
	int32 RandomNumber = FMath::RandHelper(DeathAnimations.Num());
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
	SetCanMove(false);

	SetLifeSpan(10);

	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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