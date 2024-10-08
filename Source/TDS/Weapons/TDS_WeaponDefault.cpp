// Top Down Shooter. All Rights Reserved

#include "TDS_WeaponDefault.h"
#include "TDS_ProjectileDefault.h"
#include "DrawDebugHelpers.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"

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

	ShootLocation = CreateDefaultSubobject<UArrowComponent>("Shoot Location");
	ShootLocation->SetupAttachment(GetRootComponent());
}

void ATDS_WeaponDefault::BeginPlay()
{
	Super::BeginPlay();
	
	WeaponInit();
}

void ATDS_WeaponDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FireTick(DeltaTime);
	ReloadTick(DeltaTime);
	DispersionTick(DeltaTime);
}

void ATDS_WeaponDefault::FireTick(float DeltaTime)
{
	if (WeaponFiring)
	{
		if (FireTimer < 0.0f)
		{
			if (GetWeaponRound() > 0 && !WeaponReloading)
			{				
				Fire();
				UE_LOG(TDSWeaponDefaultLog, Display, TEXT("ATDS_WeaponDefault::FireTick - %d bullets lost"), GetWeaponRound());
			}
			else
			{
				if (!WeaponReloading)
				{
					UE_LOG(TDSWeaponDefaultLog, Display, TEXT("ATDS_WeaponDefault::FireTick - %d bullets lost, RELOAD WEAPON"), GetWeaponRound());
					InitReload();
				}
			}
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

FProjectileInfo ATDS_WeaponDefault::GetProjectile()
{
	return WeaponSettings.ProjectileSettings;
}

void ATDS_WeaponDefault::Fire()
{
	FireTimer = WeaponSettings.RateOfFire;
	--WeaponInfo.Round;
	ChangeDispersionByShot();

	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponSettings.SoundFireWeapon, ShootLocation->GetComponentLocation());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponSettings.EffectFireWeapon, ShootLocation->GetComponentTransform());

	int32 NumberProjectile = GetNumberProjectileByShot();

	if (ShootLocation)
	{
		FVector SpawnLocation = ShootLocation->GetComponentLocation();
		FRotator SpawnRotation = ShootLocation->GetComponentRotation();
		FProjectileInfo ProjectileInfo;
		ProjectileInfo = GetProjectile();

		FVector EndLocation;
		for (int8 i = 0; i < NumberProjectile; ++i)
		{
			EndLocation = GetFireEndLocation();

			FVector Direction = EndLocation - SpawnLocation;
			Direction.Normalize();

			FMatrix MyMatrix(Direction, FVector(0, 0, 0), FVector(0, 0, 0), FVector::ZeroVector);
			SpawnRotation = MyMatrix.Rotator();

			if (ProjectileInfo.Projectile)
			{
				//Projectile Init ballistic fire
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = GetOwner();
				SpawnParams.Instigator = GetInstigator();

				auto* MyProjectile = Cast<ATDS_ProjectileDefault>(GetWorld()->SpawnActor(ProjectileInfo.Projectile, &SpawnLocation, &SpawnRotation, SpawnParams));
				if (MyProjectile)
				{
					//todo Init Projectle settings by id in table row(or keep in weapon table)
					MyProjectile->InitialLifeSpan = 20.0f;
					//Projectile->bulletProjectileMovement->InitialSpeed = 2500.0f;
				}
			}
			else
			{
				//ToDo Projectile null Init trace fire
			}
		}		
	}
}

void ATDS_WeaponDefault::UpdateStateWeapon(EMovementState InMovementState)
{
	// todo Dispersion
	BlockFire = false;
	switch (InMovementState)
	{
	case EMovementState::Aim_State:
		CurrentDispersionMax = WeaponSettings.DispersionWeapon.Aim_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSettings.DispersionWeapon.Aim_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSettings.DispersionWeapon.Aim_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSettings.DispersionWeapon.Aim_StateDispersionReduction;
		break;
	case EMovementState::Walk_State:
		CurrentDispersionMax = WeaponSettings.DispersionWeapon.Walk_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSettings.DispersionWeapon.Walk_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSettings.DispersionWeapon.Walk_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSettings.DispersionWeapon.Walk_StateDispersionReduction;
		break;
	case EMovementState::Run_State:
		CurrentDispersionMax = WeaponSettings.DispersionWeapon.Run_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSettings.DispersionWeapon.Run_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSettings.DispersionWeapon.Run_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSettings.DispersionWeapon.Run_StateDispersionReduction;
		break;
	case EMovementState::Sprint_State:
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
	return WeaponInfo.Round;
}

void ATDS_WeaponDefault::InitReload()
{
	WeaponReloading = true;

	ReloadTimer = WeaponSettings.ReloadTime;

	if (WeaponSettings.AnimCharacterReload)
	{
		OnWeaponReloadStart.Broadcast(WeaponSettings.AnimCharacterReload);
	}	
}

void ATDS_WeaponDefault::FinishReload()
{
	WeaponReloading = false;
	WeaponInfo.Round = WeaponSettings.MaxRound;

	OnWeaponReloadEnd.Broadcast();
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
			DrawDebugCone(GetWorld(), ShootLocation->GetComponentLocation(), -(ShootLocation->GetComponentLocation() - ShootEndLocation), WeaponSettings.DistanceTrace, GetCurrentDispersion() * PI / 180.0f, GetCurrentDispersion() * PI / 180.0f, 32, FColor::Emerald, false, 0.1f, (uint8)'\000', 1.0f);
		}
	}
	else
	{
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot(ShootLocation->GetForwardVector()) * 20000.0f;

		if (ShowDebug)
		{
			DrawDebugCone(GetWorld(), ShootLocation->GetComponentLocation(), ShootLocation->GetForwardVector(), WeaponSettings.DistanceTrace, GetCurrentDispersion() * PI / 180.0f, GetCurrentDispersion() * PI / 180.0f, 32, FColor::Emerald, false, 0.1f, (uint8)'\000', 1.0f);
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

		//DrawDebugSphere(GetWorld(), ShootLocation->GetComponentLocation() + ShootLocation->GetForwardVector()*SizeVectorToChangeShootDirectionLogic, 10.0f, 8, FColor::Red, false, 10.0f);
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