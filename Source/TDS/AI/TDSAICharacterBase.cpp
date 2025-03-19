// Top Down Shooter. All Rights Reserved

#include "TDSAICharacterBase.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Materials/MaterialInterface.h"
#include "../BuffSystem/TDSStateEffect.h"
#include "../Components/TDSHealthComponent.h"
#include "../Components/TDSLootDropComponent.h"
#include "../Weapons/TDS_ProjectileDefault.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TDSAIController.h"

ATDSAICharacterBase::ATDSAICharacterBase(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ATDSAIController::StaticClass();

	PrimaryActorTick.bCanEverTick = false;

	IsDead = false;

	HealthComponent = CreateDefaultSubobject<UTDSHealthComponent>("HealthComponent");
	LootDropComponent = CreateDefaultSubobject<UTDSLootDropComponent>("LootDropComponent");

	GetCharacterMovement()->MaxAcceleration = 300.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;

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