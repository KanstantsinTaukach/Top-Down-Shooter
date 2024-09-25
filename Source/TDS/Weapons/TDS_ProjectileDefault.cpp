// Top Down Shooter. All Rights Reserved


#include "TDS_ProjectileDefault.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

ATDS_ProjectileDefault::ATDS_ProjectileDefault()
{
	PrimaryActorTick.bCanEverTick = true;

	BulletCollisionSphere = CreateDefaultSubobject<USphereComponent>("CollisionSphere");

	BulletCollisionSphere->SetSphereRadius(5.0f);

	BulletCollisionSphere->OnComponentHit.AddDynamic(this, &ATDS_ProjectileDefault::BulletCollisionSphereHit);
	BulletCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ATDS_ProjectileDefault::BulletCollisionSphereBeginOverlap);
	BulletCollisionSphere->OnComponentEndOverlap.AddDynamic(this, &ATDS_ProjectileDefault::BulletCollisionSphereEndOverlap);

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
	BulletProjectileMovement->InitialSpeed = 100.0f;
	BulletProjectileMovement->MaxSpeed = 1000.0f;

	BulletProjectileMovement->bRotationFollowsVelocity = true;
	BulletProjectileMovement->bShouldBounce = true;
}

void ATDS_ProjectileDefault::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATDS_ProjectileDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATDS_ProjectileDefault::BulletCollisionSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}

void ATDS_ProjectileDefault::BulletCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void ATDS_ProjectileDefault::BulletCollisionSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}