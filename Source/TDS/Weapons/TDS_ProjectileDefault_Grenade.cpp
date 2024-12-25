// Top Down Shooter. All Rights Reserved

#include "TDS_ProjectileDefault_Grenade.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

int32 DebugExplodeShow = 0;
FAutoConsoleVariableRef CVARExplodeShow(TEXT("DebugExplode"), DebugExplodeShow, TEXT("Draw Debug for Explode"), ECVF_Cheat);

void ATDS_ProjectileDefault_Grenade::BeginPlay()
{
	Super::BeginPlay();
}

void ATDS_ProjectileDefault_Grenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasDelayBeforeExplosion)
	{
		TimerExplode(DeltaTime);
	}
}

void ATDS_ProjectileDefault_Grenade::TimerExplode(float DeltaTime)
{
	if (TimerEnabled)
	{
		if (TimerToExplode > TimeToExplode)
		{
			Explode();
		}
		else
		{
			TimerToExplode += DeltaTime;
		}
	}	
}

void ATDS_ProjectileDefault_Grenade::BulletCollisionSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{	
	if (!HasDelayBeforeExplosion)
	{
		if (OtherActor && Hit.bBlockingHit)
		{
			Explode();
		}
	}
	else
	{
		Super::BulletCollisionSphereHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	}
}

void ATDS_ProjectileDefault_Grenade::ImpactProjectile()
{
	if (HasDelayBeforeExplosion)
	{
		TimerEnabled = true;
	}
}

void ATDS_ProjectileDefault_Grenade::Explode()
{
	if (DebugExplodeShow)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), ProjectileSettings.ProjectileMaxRadiusDamage, 18, FColor::Green, false, 10.0f);
		DrawDebugSphere(GetWorld(), GetActorLocation(), ProjectileSettings.ProjectileMinRadiusDamage, 18, FColor::Red, false, 10.0f);
	}

	TimerEnabled = false;
	if (ProjectileSettings.ExplodeFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileSettings.ExplodeFX, GetActorLocation(),  GetActorRotation(), FVector(1.0f));
	}

	if (ProjectileSettings.ExplodeSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ProjectileSettings.ExplodeSound, GetActorLocation());
	}

	if (ProjectileSettings.ExplodeDecal)
	{
		UGameplayStatics::SpawnDecalAtLocation(GetWorld(), ProjectileSettings.ExplodeDecal, FVector(50.0f, 50.0f, 50.0f), GetActorLocation(), FRotator(-90.0f, 0.0f, 0.0f), 10.0f);
	}

	TArray<AActor*> IgnoredActor;

	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), ProjectileSettings.ExplodeMaxDamage, ProjectileSettings.ExplodeMaxDamage * 0.2f, GetActorLocation(), 1000.0f, 2000.0f, 5, NULL, IgnoredActor, nullptr, nullptr);

	this->Destroy();
}
