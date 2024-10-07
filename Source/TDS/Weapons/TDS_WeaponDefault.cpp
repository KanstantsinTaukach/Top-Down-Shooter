// Top Down Shooter. All Rights Reserved

#include "TDS_WeaponDefault.h"
#include "TDS_ProjectileDefault.h"
#include "Components/ArrowComponent.h"

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
	}
}

bool ATDS_WeaponDefault::CheckWeaponCanFire()
{
	return true;
}

FProjectileInfo ATDS_WeaponDefault::GetProjectile()
{
	return WeaponSettings.ProjectileSettings;
}

void ATDS_WeaponDefault::Fire()
{
	FireTimer = WeaponSettings.RateOfFire;
	--WeaponInfo.Round;

	if (ShootLocation)
	{
		FVector SpawnLocation = ShootLocation->GetComponentLocation();
		FRotator SpawnRotation = ShootLocation->GetComponentRotation();
		FProjectileInfo ProjectileInfo;
		ProjectileInfo = GetProjectile();

		if (ProjectileInfo.Projectile)
		{
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

void ATDS_WeaponDefault::UpdateStateWeapon(EMovementState InMovementState)
{
	// todo Dispersion
	ChangeDispersion();
}

void ATDS_WeaponDefault::ChangeDispersion()
{

}

int32 ATDS_WeaponDefault::GetWeaponRound()
{
	return WeaponInfo.Round;
}

void ATDS_WeaponDefault::InitReload()
{
	WeaponReloading = true;

	ReloadTimer = WeaponSettings.ReloadTime;

	//todo add AnimReload
}

void ATDS_WeaponDefault::FinishReload()
{
	WeaponReloading = false;
	WeaponInfo.Round = WeaponSettings.MaxRound;
}