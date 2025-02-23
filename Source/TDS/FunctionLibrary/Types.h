// Top Down Shooter. All Rights Reserved

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "NiagaraFunctionLibrary.h"
#include "../BuffSystem/TDSStateEffect.h"
#include "../Interaction/TDSInterfaceGameActor.h"
#include "Types.generated.h"

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Aim_State = 0 UMETA(DisplayName = "Aim State"),
	Walk_State UMETA(DisplayName = "Walk State"),
	Run_State UMETA(DisplayName = "Run State"),
	Sprint_State UMETA(DisplayName = "Sprint State")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	RifleType = 0 UMETA(DisplayName = "Rifle"),
	ShotgunType UMETA(DisplayName = "Shotgun"),	
	GrenadeLauncherType UMETA(DisplayName = "GrenadeLauncher"),
	RocketLauncherType UMETA(DisplayName = "RocketLauncher"),
	SniperRifleType UMETA(DisplayName = "SniperRifle"),
	None UMETA(DisplayName = "None")
};

USTRUCT(BlueprintType)
struct FCharacterSpeed
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AimSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RunSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 900.0f;
};

USTRUCT(BlueprintType)
struct FProjectileInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
	TSubclassOf<class ATDS_ProjectileDefault> Projectile = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
	UStaticMesh* ProjectileStaticMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
	FTransform ProjectileStaticMeshOffset = FTransform();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
	UParticleSystem* ProjectileTrailFX = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
	FTransform ProjectileTrailFXOffset = FTransform();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
	float ProjectileDamage = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
	float ProjectileLifeTime = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
	float ProjectileInitSpeed = 2500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
	bool ExplosiveProjectile = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings", meta = (EditCondition = "!ExplosiveProjectile"))
	TMap<TEnumAsByte<EPhysicalSurface>, UMaterialInterface*> HitDecals;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings", meta = (EditCondition = "!ExplosiveProjectile"))
	USoundBase* HitSound = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings", meta = (EditCondition = "!ExplosiveProjectile"))
	TMap<TEnumAsByte<EPhysicalSurface>, UParticleSystem*> HitFXs;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExplodeSettings", meta = (EditCondition = "ExplosiveProjectile"))
	UMaterialInterface* ExplodeDecal = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExplodeSettings", meta = (EditCondition = "ExplosiveProjectile"))
	UParticleSystem* ExplodeFX = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExplodeSettings", meta = (EditCondition = "ExplosiveProjectile"))
	USoundBase* ExplodeSound = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExplodeSettings", meta = (EditCondition = "ExplosiveProjectile"))
	float ProjectileMaxRadiusDamage = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExplodeSettings", meta = (EditCondition = "ExplosiveProjectile"))
	float ProjectileMinRadiusDamage = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExplodeSettings", meta = (EditCondition = "ExplosiveProjectile"))
	float ExplodeMaxDamage = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExplodeSettings", meta = (EditCondition = "ExplosiveProjectile"))
	float ExplodeFalloffCoef = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TSubclassOf<UTDSStateEffect> Effect = nullptr;
};

USTRUCT(BlueprintType)
struct FWeaponDispersion
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
	float Aim_StateDispersionAimMax = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
	float Aim_StateDispersionAimMin = 0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
	float Aim_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
	float Aim_StateDispersionReduction = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
	float Walk_StateDispersionAimMax = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
	float Walk_StateDispersionAimMin = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
	float Walk_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
	float Walk_StateDispersionReduction = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
	float Run_StateDispersionAimMax = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
	float Run_StateDispersionAimMin = 4.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
	float Run_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
	float Run_StateDispersionReduction = 0.1f;
};

USTRUCT(blueprintType)
struct FAnimationWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimCharacter")
	UAnimMontage* AnimCharFire = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimCharacter")
	UAnimMontage* AnimCharFireAim = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimCharacter")
	UAnimMontage* AnimCharReload = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimCharacter")
	UAnimMontage* AnimCharReloadAim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimWeapon")
	UAnimMontage* AnimWeaponReload = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimWeapon")
	UAnimMontage* AnimWeaponReloadAim = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimWeapon")
	UAnimMontage* AnimWeaponFire = nullptr;
};

USTRUCT(blueprintType)
struct FDropMeshInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh")
	UStaticMesh* DropMesh = nullptr;

	// 0.0f immediately drop
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh")
	float DropMeshTime = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh")
	float DropMeshLifeTime = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh")
	FTransform DropMeshOffset = FTransform();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh")
	FVector DropMeshImpulseDir = FVector(0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh")
	float PowerImpulse = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh")
	float ImpulseRandomDispersion = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh")
	float CustomMass = 0.0f;
};

USTRUCT(BlueprintType)
struct FWeaponInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Class")
	TSubclassOf<class ATDS_WeaponDefault> WeaponClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "State")
	float RateOfFire = 0.1f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "State")
	float ReloadTime = 2.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "State")
	int32 MaxRound = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "State")
	int32 NumberProjectilesByShot = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dispersion")
	FWeaponDispersion DispersionWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundBase* SoundFireWeapon = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundBase* SoundReloadWeapon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX")
	UParticleSystem* EffectFireWeapon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FX")
	UParticleSystem* EffectFireWeaponBack = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponSettings")
	FProjectileInfo ProjectileSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Trace")
	float DistanceTrace = 2000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HitEffect")
	UDecalComponent* DecalOnHit = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	FAnimationWeaponInfo AnimWeaponInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mesh")
	FDropMeshInfo ClipDropMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mesh")
	FDropMeshInfo ShellBullets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory")
	float SwitchTimeToWeapon = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory")
	UTexture2D* WeaponIcon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory")
	EWeaponType WeaponType = EWeaponType::RifleType;
};

USTRUCT(BlueprintType)
struct FAdditionalWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStats")
	int32 Round = 0;
};

USTRUCT(BlueprintType)
struct FWeaponSlot
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponSlot")
	FName NameItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponSlot")
	EWeaponType WeaponType = EWeaponType::RifleType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponSlot")
	FAdditionalWeaponInfo AdditionalInfo;
};

USTRUCT(BlueprintType)
struct FAmmoSlot
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AmmoSlot")
	EWeaponType WeaponType = EWeaponType::RifleType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AmmoSlot")
	int32 Bullets = 100;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AmmoSlot")
	int32 MaxBullets = 100;
};

USTRUCT(BlueprintType)
struct FDropItem : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DropItem")
	USkeletalMesh* WeaponDropSkeletalMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DropItem")
	FWeaponSlot WeaponDropSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintreadWrite, Category = "DropItem")
	UNiagaraSystem* WeaponDropVFX = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DropItem")
	FTransform WeaponDropMeshOffset = FTransform();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DropItem")
	USoundBase* WeaponDropSound = nullptr;
};

UCLASS()
class TDS_API UTypes : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void AddEffectBySurfaceType(TSubclassOf<UTDSStateEffect> EffectClass, EPhysicalSurface SurfaceType, const FHitResult& Hit)
	{
		if (!Hit.IsValidBlockingHit() || !EffectClass || SurfaceType == EPhysicalSurface::SurfaceType_Default)
		{
			return;
		}

		const auto TargetActor = Hit.GetActor();

		UTDSStateEffect* DefaultEffect = Cast<UTDSStateEffect>(EffectClass->GetDefaultObject());
		if (!DefaultEffect)
		{
			return;
		}

		if (!DefaultEffect->InteractEffects.Contains(SurfaceType))
		{
			return;
		}

		if (!CanAddEffect(TargetActor, EffectClass, DefaultEffect->GetIsStackable()))
		{
			return;
		}

		UTDSStateEffect* NewEffect = NewObject<UTDSStateEffect>(TargetActor, EffectClass);
		if (NewEffect)
		{
			NewEffect->InitObject(TargetActor, Hit);
		}
	};

	static bool CanAddEffect(AActor* TargetActor, TSubclassOf<UTDSStateEffect> EffectClass, bool IsStackable)
	{
		if (IsStackable)
		{
			return true;
		}

		ITDSInterfaceGameActor* MyInterface = Cast<ITDSInterfaceGameActor>(TargetActor);
		if (!MyInterface)
		{
			return false;
		}

		const TArray<UTDSStateEffect*> CurrentEffects = MyInterface->GetAllCurrentEffects();
		for (const auto& Effect : CurrentEffects)
		{
			if (Effect->GetClass() == EffectClass)
			{
				return false;
			}
		}

		return true;
	};
};