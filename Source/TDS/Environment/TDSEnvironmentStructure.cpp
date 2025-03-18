// Top Down Shooter. All Rights Reserved

#include "TDSEnvironmentStructure.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Materials/MaterialInterface.h"
#include "../BuffSystem/TDSStateEffect.h"
#include "../Components/TDSHealthComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "../Weapons/TDS_ProjectileDefault.h"

DEFINE_LOG_CATEGORY_STATIC(TDSEnvironmentStructure, All, All);

ATDSEnvironmentStructure::ATDSEnvironmentStructure()
{
	PrimaryActorTick.bCanEverTick = false;

	HealthComponent = CreateDefaultSubobject<UTDSHealthComponent>("HealthComponent");
}

void ATDSEnvironmentStructure::BeginPlay()
{
	Super::BeginPlay();

	check(HealthComponent);
	HealthComponent->OnDeath.AddDynamic(this, &ATDSEnvironmentStructure::OnObjectDestruction);

	OnTakeRadialDamage.AddDynamic(this, &ATDSEnvironmentStructure::OnTakeRadialDamageHandle);
}

EPhysicalSurface ATDSEnvironmentStructure::GetSurfaceType()
{
	EPhysicalSurface Result = ITDSInterfaceGameActor::GetSurfaceType();

	UStaticMeshComponent* MyMesh = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (MyMesh)
	{
		UMaterialInterface* MyMaterial = MyMesh->GetMaterial(0);
		if (MyMaterial)
		{
			Result = MyMaterial->GetPhysicalMaterial()->SurfaceType;
		}
	}
	return Result;
}

TArray<UTDSStateEffect*> ATDSEnvironmentStructure::GetAllCurrentEffects()
{
	return StateEffects;
}

void ATDSEnvironmentStructure::RemoveEffect(UTDSStateEffect* EffectToRemove)
{
	StateEffects.Remove(EffectToRemove);
}

void ATDSEnvironmentStructure::AddEffect(UTDSStateEffect* EffectToAdd)
{
	StateEffects.Add(EffectToAdd);
}

float ATDSEnvironmentStructure::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (HealthComponent)
	{
		HealthComponent->RemoveFromCurrentHealth(Damage);
	}
	return ActualDamage;
}

void ATDSEnvironmentStructure::OnTakeRadialDamageHandle(AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector Origin, FHitResult HitInfo, AController* InstigatedBy, AActor* DamageCauser)
{
	ATDS_ProjectileDefault* TargetProjectile = Cast<ATDS_ProjectileDefault>(DamageCauser);
	if (TargetProjectile)
	{
		UTypes::AddEffectBySurfaceType(TargetProjectile->GetProjectileSettings().Effect, GetSurfaceType(), HitInfo);
	}
}

void ATDSEnvironmentStructure::OnObjectDestruction()
{
	if (!GetWorld()) return;

	if (DestructionVFX)
	{
		NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DestructionVFX, GetActorLocation());
	}

	if (DestructionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DestructionSound, GetActorLocation());
	}

	Destroy();
}
