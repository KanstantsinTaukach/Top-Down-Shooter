// Top Down Shooter. All Rights Reserved


#include "TDS_ProjectileDefault.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "../FunctionLibrary/TDSUtils.h"

DEFINE_LOG_CATEGORY_STATIC(TDSProjectileLog, All, All);

ATDS_ProjectileDefault::ATDS_ProjectileDefault()
{
	PrimaryActorTick.bCanEverTick = true;

	BulletCollisionSphere = CreateDefaultSubobject<USphereComponent>("CollisionSphere");

	BulletCollisionSphere->SetSphereRadius(5.0f);

	//Hit Event return physMaterial
	BulletCollisionSphere->bReturnMaterialOnMove = true;

	//Collision not affect navigation (P keybord on editor)
	BulletCollisionSphere->SetCanEverAffectNavigation(false);

	RootComponent = BulletCollisionSphere;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMesh");
	BulletMesh->SetupAttachment(RootComponent);
	BulletMesh->SetCanEverAffectNavigation(false);
	
	BulletFX = CreateDefaultSubobject<UParticleSystemComponent>("BulletFX");
	BulletFX->SetupAttachment(RootComponent);
	
	BulletProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("BulletProjectileMovement");
	BulletProjectileMovement->UpdatedComponent = RootComponent;
	BulletProjectileMovement->InitialSpeed = ProjectileSettings.ProjectileInitSpeed;
	BulletProjectileMovement->MaxSpeed = ProjectileSettings.ProjectileInitSpeed;

	UE_LOG(TDSProjectileLog, Display, TEXT("ATDS_ProjectileDefault:ATDS_ProjectileDefault() - initialSpeed of projectile is: %f"), BulletProjectileMovement->InitialSpeed);

	BulletProjectileMovement->bRotationFollowsVelocity = true;
	BulletProjectileMovement->bShouldBounce = true;
}

void  ATDS_ProjectileDefault::InitProjectile(FProjectileInfo InitParam)
{
	BulletProjectileMovement->InitialSpeed = InitParam.ProjectileInitSpeed;
	BulletProjectileMovement->MaxSpeed = InitParam.ProjectileInitSpeed;
	this->SetLifeSpan(InitParam.ProjectileLifeTime);

	if (InitParam.ProjectileStaticMesh)
	{
		BulletMesh->SetStaticMesh(InitParam.ProjectileStaticMesh);
		BulletMesh->SetRelativeTransform(InitParam.ProjectileStaticMeshOffset);
	}
	else
	{
		BulletMesh->DestroyComponent();
	}

	if (InitParam.ProjectileTrailFX)
	{
		BulletFX->SetTemplate(InitParam.ProjectileTrailFX);
		BulletFX->SetRelativeTransform(InitParam.ProjectileTrailFXOffset);
	}
	else
	{
		BulletFX->DestroyComponent();
	}

	ProjectileSettings = InitParam;
}

void ATDS_ProjectileDefault::BeginPlay()
{
	Super::BeginPlay();

	check(BulletCollisionSphere);
	check(BulletMesh);
	check(BulletFX);
	check(BulletProjectileMovement);

	BulletCollisionSphere->OnComponentHit.AddDynamic(this, &ATDS_ProjectileDefault::BulletCollisionSphereHit);
	BulletCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ATDS_ProjectileDefault::BulletCollisionSphereBeginOverlap);
	BulletCollisionSphere->OnComponentEndOverlap.AddDynamic(this, &ATDS_ProjectileDefault::BulletCollisionSphereEndOverlap);	
}

void ATDS_ProjectileDefault::BulletCollisionSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && Hit.PhysMaterial.IsValid())
	{
		EPhysicalSurface MySurfaceType = UGameplayStatics::GetSurfaceType(Hit);
		if (ProjectileSettings.HitDecals.Contains(MySurfaceType))
		{
			UMaterialInterface* MyMaterial = ProjectileSettings.HitDecals[MySurfaceType];
			if (MyMaterial && OtherComp)
			{
				UGameplayStatics::SpawnDecalAttached(MyMaterial, FVector(10.0f), OtherComp, NAME_None, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition, 10.0f);
			}
		}
		if (ProjectileSettings.HitFXs.Contains(MySurfaceType))
		{
			UParticleSystem* MyParticle = ProjectileSettings.HitFXs[MySurfaceType];
			if (MyParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MyParticle, FTransform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint, FVector(1.0f)));
			}
		}
		if (ProjectileSettings.HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ProjectileSettings.HitSound, Hit.ImpactPoint);
		}
	}	

	UGameplayStatics::ApplyDamage(OtherActor, ProjectileSettings.ProjectileDamage, GetInstigatorController(), this, NULL); 
	
	ImpactProjectile();
}

void ATDS_ProjectileDefault::BulletCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void ATDS_ProjectileDefault::BulletCollisionSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ATDS_ProjectileDefault::ImpactProjectile()
{
	UE_LOG(TDSProjectileLog, Display, TEXT("ATDS_ProjectileDefault::ImpactProjectile() - initialSpeed of projectile is: %f"), BulletProjectileMovement->InitialSpeed);
	this->Destroy();
}

void ATDS_ProjectileDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}