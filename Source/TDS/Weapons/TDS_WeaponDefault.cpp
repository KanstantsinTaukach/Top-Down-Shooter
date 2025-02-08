// Top Down Shooter. All Rights Reserved

#include "TDS_WeaponDefault.h"
#include "TDS_ProjectileDefault.h"
#include "DrawDebugHelpers.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/StaticMeshActor.h"
#include "../FunctionLibrary/AnimUtils.h"
#include "Particles/ParticleSystemComponent.h"
#include "../Components/TDSInventoryComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"

DEFINE_LOG_CATEGORY_STATIC(TDSWeaponDefaultLog, All, All);

ATDS_WeaponDefault::ATDS_WeaponDefault()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("Scene");
	RootComponent = SceneComponent;

	SkeletalMeshWeapon = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMesh");
	SkeletalMeshWeapon->SetGenerateOverlapEvents(false);
	SkeletalMeshWeapon->SetCollisionProfileName("NoCollision");
	SkeletalMeshWeapon->SetupAttachment(RootComponent);

	StaticMeshWeapon = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMeshWeapon->SetGenerateOverlapEvents(false);
	StaticMeshWeapon->SetCollisionProfileName("NoCollision");
	StaticMeshWeapon->SetupAttachment(RootComponent);

	ShootLocation = CreateDefaultSubobject<UArrowComponent>("ShootLocation");
	ShootLocation->SetupAttachment(GetRootComponent());

	TraceSpeed = 0.2f;
}

void ATDS_WeaponDefault::BeginPlay()
{
	Super::BeginPlay();

	check(SceneComponent);
	check(SkeletalMeshWeapon);
	check(StaticMeshWeapon);
	check(ShootLocation);

	WeaponInit();
}

void ATDS_WeaponDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FireTick(DeltaTime);
	ReloadTick(DeltaTime);
	DispersionTick(DeltaTime);
	ClipDropTick(DeltaTime);
	ShellDropTick(DeltaTime);
}

void ATDS_WeaponDefault::FireTick(float DeltaTime)
{
	if (WeaponFiring && GetWeaponRound() > 0 && !WeaponReloading)
	{
		if (FireTimer < 0.0f)
		{
			Fire();
		}
		else
		{
			FireTimer -= DeltaTime;
		}
	}
}

void ATDS_WeaponDefault::ReloadTick(float DeltaTime)
{
	if (WeaponReloading)
	{
		if (ReloadTimer < 0.0f)
		{
			FinishReload();
		}
		else
		{
			ReloadTimer -= DeltaTime;
		}
	}
}

void ATDS_WeaponDefault::InitReload()
{
	if (!GetWorld())
	{
		return;
	}

	if (!WeaponReloading)
	{
		WeaponReloading = true;

		ReloadTimer = WeaponSettings.ReloadTime;

		if (WeaponSettings.SoundReloadWeapon)
		{
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponSettings.SoundReloadWeapon, ShootLocation->GetComponentLocation());
		}

		auto AnimCharacterToPlay = AnimUtils::FindAnimToPlay(WeaponSettings.AnimWeaponInfo.AnimCharReloadAim, WeaponSettings.AnimWeaponInfo.AnimCharReload, WeaponAiming);
		OnWeaponReloadStart.Broadcast(AnimCharacterToPlay);

		auto AnimWeaponToPlay = AnimUtils::FindAnimToPlay(WeaponSettings.AnimWeaponInfo.AnimWeaponReloadAim, WeaponSettings.AnimWeaponInfo.AnimWeaponReload, WeaponAiming);
		if (AnimWeaponToPlay && SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
		{
			SkeletalMeshWeapon->GetAnimInstance()->Montage_Play(AnimWeaponToPlay);
		}

		if (WeaponSettings.ClipDropMesh.DropMesh)
		{
			DropClipFlag = true;
			DropClipTimer = WeaponSettings.ClipDropMesh.DropMeshTime;
		}
	}
}

void ATDS_WeaponDefault::CancelReload()
{
	WeaponReloading = false;
	if (SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
	{
		SkeletalMeshWeapon->GetAnimInstance()->StopAllMontages(0.15f);
	}

	OnWeaponReloadEnd.Broadcast(false, 0);
	DropClipFlag = false;
}

void ATDS_WeaponDefault::FinishReload()
{
	WeaponReloading = false;

	int32 AvailableAmmo = GetAmmoForReload();

	int32 AmmoNeedToReload = WeaponSettings.MaxRound - AdditionalWeaponInfo.Round;

	int32 AmmoNeedToTakeFromInventory = 0;

	if (AmmoNeedToReload > AvailableAmmo)
	{
		AdditionalWeaponInfo.Round += AvailableAmmo;
		AmmoNeedToTakeFromInventory = AvailableAmmo;
	}
	else
	{
		AdditionalWeaponInfo.Round += AmmoNeedToReload;
		AmmoNeedToTakeFromInventory = AmmoNeedToReload;
	}

	OnWeaponReloadEnd.Broadcast(true, -AmmoNeedToTakeFromInventory);
}

void ATDS_WeaponDefault::DispersionTick(float DeltaTime)
{
	if (!WeaponReloading)
	{
		if (!WeaponFiring)
		{
			if (ShouldReduceDispersion)
			{
				CurrentDispersion = CurrentDispersion - CurrentDispersionReduction;
			}
			else
			{
				CurrentDispersion = CurrentDispersion + CurrentDispersionReduction;
			}
		}

		if (CurrentDispersion < CurrentDispersionMin)
		{
			CurrentDispersion = CurrentDispersionMin;
		}
		else if (CurrentDispersion > CurrentDispersionMax)
		{
			CurrentDispersion = CurrentDispersionMax;
		}
	}
	if (ShowDebug)
	{
		UE_LOG(TDSWeaponDefaultLog, Display, TEXT("ATDS_WeaponDefault::DispersionTick - Dispersion: MAX = %f, MIN = %f, Current = %f"), CurrentDispersionMax, CurrentDispersionMin, CurrentDispersion);
	}
}

void ATDS_WeaponDefault::ClipDropTick(float DeltaTime)
{
	if (DropClipFlag)
	{
		if (DropClipTimer <= 0.0f)
		{
			DropClipFlag = false;
			InitDropMesh(WeaponSettings.ClipDropMesh);
		}
		else
		{
			DropClipTimer -= DeltaTime;
		}
	}
}

void ATDS_WeaponDefault::ShellDropTick(float DeltaTime)
{
	if (DropShellFlag)
	{
		if (DropShellTimer <= 0.0f)
		{
			DropShellFlag = false;
			InitDropMesh(WeaponSettings.ShellBullets);
		}
		else
		{
			DropShellTimer -= DeltaTime;
		}
	}
}

void ATDS_WeaponDefault::WeaponInit()
{
	if (SkeletalMeshWeapon && !SkeletalMeshWeapon->SkeletalMesh)
	{
		SkeletalMeshWeapon->DestroyComponent(true);
	}

	if (StaticMeshWeapon && !StaticMeshWeapon->GetStaticMesh())
	{
		StaticMeshWeapon->DestroyComponent(true);
	}

	UpdateStateWeapon(EMovementState::Run_State);
}

void ATDS_WeaponDefault::SetWeaponStateFire(bool bIsFire)
{
	if (CheckWeaponCanFire())
	{
		WeaponFiring = bIsFire;
	}
	else
	{
		WeaponFiring = false;
		FireTimer = 0.01f;
	}
}

bool ATDS_WeaponDefault::CheckWeaponCanFire()
{
	return !BlockFire;
}

bool ATDS_WeaponDefault::CheckWeaponCanReload()
{
	bool Result = true;

	if (GetOwner())
	{
		auto InventoryComp = Cast<UTDSInventoryComponent>(GetOwner()->GetComponentByClass(UTDSInventoryComponent::StaticClass()));
		if (InventoryComp)
		{
			int32 AvailableAmmoForWeapon = 0;
			if (!InventoryComp->CheckAmmoForWeapon(WeaponSettings.WeaponType, AvailableAmmoForWeapon))
			{
				Result = false;
			}
		}
	}

	return Result;
}

int32 ATDS_WeaponDefault::GetAmmoForReload()
{
	int32 AvailableAmmoForWeapon = WeaponSettings.MaxRound;

	if (GetOwner())
	{
		auto InventoryComp = Cast<UTDSInventoryComponent>(GetOwner()->GetComponentByClass(UTDSInventoryComponent::StaticClass()));
		if (InventoryComp)
		{
			if (InventoryComp->CheckAmmoForWeapon(WeaponSettings.WeaponType, AvailableAmmoForWeapon))
			{
				return AvailableAmmoForWeapon;
			}
		}
	}

	return AvailableAmmoForWeapon;
}

FProjectileInfo ATDS_WeaponDefault::GetProjectile()
{
	return WeaponSettings.ProjectileSettings;
}

void ATDS_WeaponDefault::Fire()
{
	if (!GetWorld() || !ShootLocation)
	{
		return;
	}

	auto AnimCharacterToPlay = AnimUtils::FindAnimToPlay(WeaponSettings.AnimWeaponInfo.AnimCharFireAim, WeaponSettings.AnimWeaponInfo.AnimCharFire, WeaponAiming);

	if (WeaponSettings.AnimWeaponInfo.AnimWeaponFire && SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
	{
		SkeletalMeshWeapon->GetAnimInstance()->Montage_Play(WeaponSettings.AnimWeaponInfo.AnimWeaponFire);
	}	

	if (WeaponSettings.ShellBullets.DropMesh)
	{
		if (WeaponSettings.ShellBullets.DropMeshTime <= 0.0f)
		{
			InitDropMesh(WeaponSettings.ShellBullets);
		}
		else
		{
			DropShellFlag = true;
			DropShellTimer = WeaponSettings.ShellBullets.DropMeshTime;
		}
	}

	FireTimer = WeaponSettings.RateOfFire;
	--AdditionalWeaponInfo.Round;
	ChangeDispersionByShot();
	
	OnWeaponFire.Broadcast(AnimCharacterToPlay);	

	SpawnMuzzleEffects();

	FProjectileInfo ProjectileInfo = GetProjectile();

	if (ProjectileInfo.Projectile)
	{
		HandleProjectileHit(ProjectileInfo);
	}
	else
	{
		GetWorldTimerManager().SetTimerForNextTick([this]()
			{
				FTimerHandle DelayTimerHandle;
				GetWorldTimerManager().SetTimer(DelayTimerHandle, FTimerDelegate::CreateLambda([this]()
					{
						SpawnTrailEffect();
						HandleHitScan();
					}), TraceSpeed, false);
			});
	}

	if (GetWeaponRound() <= 0 && !WeaponReloading)
	{
		if (CheckWeaponCanReload())
		{
			InitReload();
		}
	}
}

void ATDS_WeaponDefault::UpdateStateWeapon(EMovementState InMovementState)
{
	BlockFire = false;
	switch (InMovementState)
	{
	case EMovementState::Aim_State:
		WeaponAiming = true;
		CurrentDispersionMax = WeaponSettings.DispersionWeapon.Aim_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSettings.DispersionWeapon.Aim_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSettings.DispersionWeapon.Aim_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSettings.DispersionWeapon.Aim_StateDispersionReduction;
		break;
	case EMovementState::Walk_State:
		WeaponAiming = false;
		CurrentDispersionMax = WeaponSettings.DispersionWeapon.Walk_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSettings.DispersionWeapon.Walk_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSettings.DispersionWeapon.Walk_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSettings.DispersionWeapon.Walk_StateDispersionReduction;
		break;
	case EMovementState::Run_State:
		WeaponAiming = false;
		CurrentDispersionMax = WeaponSettings.DispersionWeapon.Run_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSettings.DispersionWeapon.Run_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSettings.DispersionWeapon.Run_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSettings.DispersionWeapon.Run_StateDispersionReduction;
		break;
	case EMovementState::Sprint_State:
		WeaponAiming = false;
		BlockFire = true;
		SetWeaponStateFire(false);
		break;
	default:
		break;
	}
}

void ATDS_WeaponDefault::ChangeDispersionByShot()
{
	CurrentDispersion += CurrentDispersionRecoil;
}

int32 ATDS_WeaponDefault::GetWeaponRound()
{
	return AdditionalWeaponInfo.Round;
}

FVector ATDS_WeaponDefault::GetFireEndLocation() const
{
	bool bShootDirection = false;
	FVector EndLocation = FVector(0.0f);

	FVector tmpV = (ShootLocation->GetComponentLocation() - ShootEndLocation);
	if (tmpV.Size() > SizeVectorToChangeShootDirectionLogic)
	{
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot((ShootLocation->GetComponentLocation() - ShootEndLocation).GetSafeNormal()) * -20000.0f;

		if (ShowDebug)
		{
			DrawDebugCone
			(
				GetWorld(), 
				ShootLocation->GetComponentLocation(), 
				-(ShootLocation->GetComponentLocation() - ShootEndLocation), 
				WeaponSettings.DistanceTrace, 
				GetCurrentDispersion() * PI / 180.0f,
				GetCurrentDispersion() * PI / 180.0f, 
				32, FColor::Emerald, 
				false, 
				0.1f, 
				(uint8)'\000',
				1.0f
			);
		}
	}
	else
	{
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot(ShootLocation->GetForwardVector()) * 20000.0f;

		if (ShowDebug)
		{
			DrawDebugCone
			(
				GetWorld(), 
				ShootLocation->GetComponentLocation(), 
				ShootLocation->GetForwardVector(), 
				WeaponSettings.DistanceTrace, 
				GetCurrentDispersion() * PI / 180.0f, 
				GetCurrentDispersion() * PI / 180.0f,
				32, 
				FColor::Emerald, 
				false,
				0.1f, 
				(uint8)'\000', 
				1.0f
			);
		}
	}

	if (ShowDebug)
	{
		//direction weapon look
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), ShootLocation->GetComponentLocation() + ShootLocation->GetForwardVector() * 500.0f, FColor::Cyan, false, 10.0f, (uint8)'\000', 0.5f);
		//direction projectile must fly
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), ShootEndLocation, FColor::Red, false, 10.0f, (uint8)'\000', 0.5f);
		//direction projectile current fly
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), EndLocation, FColor::Black, false, 10.0f, (uint8)'\000', 0.5f);
	}

	return EndLocation;
}

FVector ATDS_WeaponDefault::ApplyDispersionToShoot(FVector DirectionShoot) const
{
	return FMath::VRandCone(DirectionShoot, GetCurrentDispersion() * PI / 180.0f);
}

float ATDS_WeaponDefault::GetCurrentDispersion() const
{
	float Result = CurrentDispersion;
	return Result;
}

int32 ATDS_WeaponDefault::GetNumberProjectileByShot() const
{
	return WeaponSettings.NumberProjectilesByShot;
}

void ATDS_WeaponDefault::InitDropMesh(const FDropMeshInfo & DropMeshInfo)
{
	if (!GetWorld() || !DropMeshInfo.DropMesh)
	{
		return;
	}

	float CorrectionX = DropMeshInfo.DropMeshOffset.GetLocation().X;
	float CorrectionY = DropMeshInfo.DropMeshOffset.GetLocation().Y;
	float CorrectionZ = DropMeshInfo.DropMeshOffset.GetLocation().Z;

	FVector LocalDir = this->GetActorForwardVector() * CorrectionX + this->GetActorRightVector() * CorrectionY + this->GetActorUpVector() * CorrectionZ;

	FTransform Transform;
	Transform.SetLocation(GetActorLocation() + LocalDir);
	Transform.SetScale3D(DropMeshInfo.DropMeshOffset.GetScale3D());
	Transform.SetRotation((GetActorRotation() + DropMeshInfo.DropMeshOffset.Rotator()).Quaternion());

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	Params.Owner = this;

	AStaticMeshActor* DropMeshActor = nullptr;
	DropMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(Transform.GetLocation(), Transform.GetRotation().Rotator(), Params);
	DropMeshActor->SetActorTickEnabled(false);

	if (!DropMeshActor)
	{
		UE_LOG(TDSWeaponDefaultLog, Display, TEXT("ATDS_WeaponDefault::InitDropMesh: Failed to spawn NewActor"));
	}

	UStaticMeshComponent* MeshComponent = DropMeshActor->GetStaticMeshComponent();

	if (MeshComponent)
	{
		MeshComponent->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		MeshComponent->Mobility = EComponentMobility::Movable;
		MeshComponent->SetSimulatePhysics(true);
		MeshComponent->SetStaticMesh(DropMeshInfo.DropMesh);

		MeshComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
		MeshComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
		MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
		MeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Block);
		MeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECollisionResponse::ECR_Block);
		MeshComponent->SetCollisionResponseToChannel(ECC_PhysicsBody, ECollisionResponse::ECR_Block);

		if (DropMeshInfo.CustomMass > 0.0f)
		{
			MeshComponent->SetMassOverrideInKg(NAME_None, DropMeshInfo.CustomMass);
		}

		FVector ImpulseDirection = DropMeshInfo.DropMeshImpulseDir.GetSafeNormal();
		FVector RandomDispersion = FVector(
			FMath::RandRange(-DropMeshInfo.ImpulseRandomDispersion, DropMeshInfo.ImpulseRandomDispersion),
			FMath::RandRange(-DropMeshInfo.ImpulseRandomDispersion, DropMeshInfo.ImpulseRandomDispersion),
			FMath::RandRange(-DropMeshInfo.ImpulseRandomDispersion, DropMeshInfo.ImpulseRandomDispersion)
		);
		FVector Impulse = (ImpulseDirection + RandomDispersion).GetSafeNormal() * DropMeshInfo.PowerImpulse;

		MeshComponent->AddImpulse(Impulse);

		DropMeshActor->SetLifeSpan(DropMeshInfo.DropMeshLifeTime);
	}
}

void ATDS_WeaponDefault::HandleProjectileHit(FProjectileInfo ProjectileInfo)
{
	FVector SpawnLocation = ShootLocation->GetComponentLocation();
	FRotator SpawnRotation = ShootLocation->GetComponentRotation();
	FVector EndLocation = GetFireEndLocation();
	FVector Direction = EndLocation - SpawnLocation;
	Direction.Normalize();

	FMatrix MyMatrix(Direction, FVector(0, 0, 0), FVector(0, 0, 0), FVector::ZeroVector);
	SpawnRotation = MyMatrix.Rotator();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = GetInstigator();

	auto* MyProjectile = Cast<ATDS_ProjectileDefault>(GetWorld()->SpawnActor(ProjectileInfo.Projectile, &SpawnLocation, &SpawnRotation, SpawnParams));
	if (MyProjectile)
	{
		MyProjectile->InitProjectile(WeaponSettings.ProjectileSettings);
	}
}

void ATDS_WeaponDefault::SpawnMuzzleEffects() const
{
	if (!GetWorld())
	{
		return;
	}

	if (WeaponSettings.SoundFireWeapon)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponSettings.SoundFireWeapon, ShootLocation->GetComponentLocation());
	}

	if (WeaponSettings.EffectFireWeapon)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponSettings.EffectFireWeapon, ShootLocation->GetComponentTransform());
	}

	if (WeaponSettings.EffectFireWeaponBack)
	{
		FVector ForwardVec = ShootLocation->GetForwardVector();
		FVector ReservedVec = -ForwardVec;
		FRotator ReversedSpawnRotation = ReservedVec.Rotation();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponSettings.EffectFireWeaponBack, ShootLocation->GetComponentLocation() - ForwardVec * 120.f, ReversedSpawnRotation);
	}
}

void ATDS_WeaponDefault::SpawnTrailEffect()
{
	if (!WeaponSettings.ProjectileSettings.ProjectileTrailFX || !ShootLocation)
	{
		UE_LOG(TDSWeaponDefaultLog, Warning, TEXT("ATDS_WeaponDefault::SpawnTrailEffect: Missing TrailFX of ShootLocation"));
		return;
	}

	UParticleSystemComponent* TrailFX = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponSettings.ProjectileSettings.ProjectileTrailFX, ShootLocation->GetComponentTransform());

	if (!TrailFX)
	{
		UE_LOG(TDSWeaponDefaultLog, Warning, TEXT("ATDS_WeaponDefault::Failed to spawn TrailFX"));
		return;
	}

	FVector TraceStart = ShootLocation->GetComponentLocation();
	FVector TraceEnd = GetFireEndLocation() * WeaponSettings.DistanceTrace;

	TArray<AActor*> ActorsToIgnore;
	FHitResult Hit;

	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceStart, TraceEnd, ETraceTypeQuery::TraceTypeQuery4, false, ActorsToIgnore, EDrawDebugTrace::None, Hit, true))
	{
		TraceEnd = Hit.Location;
	}

	TrailFX->SetWorldLocation(TraceStart);

	FVector Direction = (TraceEnd - TraceStart).GetSafeNormal();
	float Distance = FVector::Dist(TraceStart, TraceEnd);
	float Speed = TraceSpeed > 0.0f ? TraceSpeed : 0.5f;
	float TravelTime = Distance / TraceSpeed;

	ActiveTrailFX = TrailFX;
	ActiveTrailDirection = Direction;
	ActiveTrailEnd = TraceEnd;
	ActiveTrailTravelTime = TravelTime;
	
	GetWorld()->GetTimerManager().SetTimer(FXTimerHandle, this, &ATDS_WeaponDefault::UpdateTrailEffect, 0.01f, true);
}

void ATDS_WeaponDefault::UpdateTrailEffect()
{
	if (!ActiveTrailFX)
	{
		return;
	}

	float DeltaTime = GetWorld()->GetDeltaSeconds();
	FVector NewLocation = ActiveTrailFX->GetComponentLocation() + ActiveTrailDirection * ActiveTrailTravelTime * DeltaTime;

	ActiveTrailFX->SetWorldLocation(NewLocation);

	if (FVector::DistSquared(NewLocation, ActiveTrailEnd) <= FMath::Square(10.0f))
	{
		ActiveTrailFX->Deactivate();
		ActiveTrailFX->DestroyComponent();

		GetWorld()->GetTimerManager().ClearTimer(FXTimerHandle);
		ActiveTrailFX = nullptr;
	}
}

void ATDS_WeaponDefault::HandleHitScan()
{
	int32 ProjectilesNumber = GetNumberProjectileByShot();

	FVector SpawnLocation = ShootLocation->GetComponentLocation();
	FVector EndLocation;

	for (int8 i = 0; i < ProjectilesNumber; ++i)
	{
		EndLocation = GetFireEndLocation();
		FHitResult Hit;
		TArray<AActor*> Actors;

		UKismetSystemLibrary::LineTraceSingle(GetWorld(), SpawnLocation, EndLocation * WeaponSettings.DistanceTrace, ETraceTypeQuery::TraceTypeQuery4, false, Actors, EDrawDebugTrace::ForDuration, Hit, true, FLinearColor::Red, FLinearColor::Green, 5.0f);

		if (ShowDebug)
		{
			DrawDebugLine(GetWorld(), SpawnLocation, EndLocation * WeaponSettings.DistanceTrace, FColor::Yellow, false, 5.0f);
		}

		GetWorldTimerManager().SetTimerForNextTick([this, Hit]()
			{
				FTimerHandle ImpactTimerHandle;
				GetWorldTimerManager().SetTimer(ImpactTimerHandle, FTimerDelegate::CreateLambda([this, Hit]()
					{
						SpawnImpactEffects(Hit);
					}), TraceSpeed, false);
			});
	}
}

void ATDS_WeaponDefault::SpawnImpactEffects(const FHitResult& Hit)
{
	if (Hit.GetActor() && Hit.PhysMaterial.IsValid())
	{
		EPhysicalSurface MySurfaceType = UGameplayStatics::GetSurfaceType(Hit);
		if (WeaponSettings.ProjectileSettings.HitDecals.Contains(MySurfaceType))
		{
			UMaterialInterface* MyMaterial = WeaponSettings.ProjectileSettings.HitDecals[MySurfaceType];
			if (MyMaterial && Hit.GetComponent())
			{
				UGameplayStatics::SpawnDecalAttached(MyMaterial, FVector(10.0f), Hit.GetComponent(), NAME_None, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition, 10.0f);
			}
		}
		if (WeaponSettings.ProjectileSettings.HitFXs.Contains(MySurfaceType))
		{
			UParticleSystem* MyParticle = WeaponSettings.ProjectileSettings.HitFXs[MySurfaceType];
			if (MyParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MyParticle, FTransform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint, FVector(1.0f)));
			}
		}
		if (WeaponSettings.ProjectileSettings.HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), WeaponSettings.ProjectileSettings.HitSound, Hit.ImpactPoint);
		}
	}

	UGameplayStatics::ApplyDamage(Hit.GetActor(), WeaponSettings.ProjectileSettings.ProjectileDamage, GetInstigatorController(), this, NULL);
}