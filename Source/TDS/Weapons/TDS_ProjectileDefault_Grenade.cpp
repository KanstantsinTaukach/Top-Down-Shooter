// Top Down Shooter. All Rights Reserved

#include "TDS_ProjectileDefault_Grenade.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "../Interaction/TDSInterfaceGameActor.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"
#include "Components/AudioComponent.h"

int32 DebugExplodeShow = 0;
FAutoConsoleVariableRef CVARExplodeShow(TEXT("DebugExplode"), DebugExplodeShow, TEXT("Draw Debug for Explode"), ECVF_Cheat);

void ATDS_ProjectileDefault_Grenade::BeginPlay()
{
	ShouldSpawnStateEffect = false;

	Super::BeginPlay();
}

void ATDS_ProjectileDefault_Grenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasDelayBeforeExplosion)
	{
		TimerExplode(DeltaTime);
	}

	if (IsLuringEnemies)
	{
		NoiseUpdateTimer += DeltaTime;
		if (NoiseUpdateTimer >= NoiseUpdateInterval)
		{
			UpdateLuringNoise();
			NoiseUpdateTimer = 0.0f;
		}
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
	if (!GetWorld()) return;

	if (DebugExplodeShow)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), ProjectileSettings.ProjectileMaxRadiusDamage, 18, FColor::Green, false, 10.0f);
		DrawDebugSphere(GetWorld(), GetActorLocation(), ProjectileSettings.ProjectileMinRadiusDamage, 18, FColor::Red, false, 10.0f);
	}

	IsLuringEnemies = false;

	if (FlightSoundComponent)
	{
		FlightSoundComponent->Stop();
		FlightSoundComponent->ConditionalBeginDestroy();
		FlightSoundComponent = nullptr;
		IsSoundPlaying = false;
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

	TArray<AActor*> IgnoredActors;

	UGameplayStatics::ApplyRadialDamageWithFalloff
	(
		GetWorld(), 
		ProjectileSettings.ExplodeMaxDamage, 
		ProjectileSettings.ExplodeMaxDamage * 0.2f, 
		GetActorLocation(), 
		ProjectileSettings.ProjectileMinRadiusDamage, 
		ProjectileSettings.ProjectileMaxRadiusDamage, 
		5, 
		NULL, 
		IgnoredActors, 
		this, 
		this->GetInstigatorController()
	);

	TArray<AActor*> AffectedActors;
	GetActorsInRange(GetWorld(), GetActorLocation(), ProjectileSettings.ProjectileMaxRadiusDamage, AffectedActors);

	this->Destroy();
}

void ATDS_ProjectileDefault_Grenade::GetActorsInRange(UWorld* World, const FVector& Origin, float Radius, TArray<AActor*>& OutActors)
{
	if (!World) return;

	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), AllActors);

	for (AActor* Actor : AllActors)
	{
		if (Actor && FVector::DistSquared(Actor->GetActorLocation(), Origin) <= FMath::Square(Radius) && Actor != this)
		{
			FHitResult Hit;
			Hit.Actor = Actor;
			Hit.ImpactPoint = Actor->GetActorLocation();
			Hit.ImpactNormal = (Hit.ImpactPoint - GetActorLocation()).GetSafeNormal();

			OutActors.Add(Actor);

			UAISense_Damage::ReportDamageEvent(GetWorld(), Actor, GetInstigator(), ProjectileSettings.ExplodeMaxDamage, GetOwner()->GetActorLocation(), Hit.ImpactPoint);
		}
	}
}

void ATDS_ProjectileDefault_Grenade::UpdateLuringNoise()
{
	if (!GetWorld() || !ProjectileSettings.WarningSound) return;

	if (FlightSoundComponent && IsSoundPlaying)
	{
		FlightSoundComponent->SetWorldLocation(GetActorLocation());
		return;
	}
	
	FlightSoundComponent = UGameplayStatics::SpawnSoundAttached(ProjectileSettings.WarningSound, GetRootComponent(), NAME_None, FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, false, 1.0f, 1.0f, 0.0f, nullptr, nullptr, false);
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 5.0f, this, 1000.0f, TEXT("LureTarget"));

	if (FlightSoundComponent)
	{
		IsSoundPlaying = true;
		FlightSoundComponent->OnAudioFinished.AddDynamic(this, &ATDS_ProjectileDefault_Grenade::OnSoundFinished);
	}
}

void ATDS_ProjectileDefault_Grenade::OnSoundFinished()
{
	if (FlightSoundComponent)
	{
		FlightSoundComponent->ConditionalBeginDestroy();
		FlightSoundComponent = nullptr;
	}
	IsSoundPlaying = false;
}