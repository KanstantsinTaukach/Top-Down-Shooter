// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../FunctionLibrary/Types.h"
#include "TDS_WeaponDefault.generated.h"

class UArrowComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponFireSignature, UAnimMontage*, AnimMontage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponReloadStartSignature, UAnimMontage*, AnimMontage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponReloadEndSignature, bool, bIsSuccess);

UCLASS()
class TDS_API ATDS_WeaponDefault : public AActor
{
	GENERATED_BODY()
	
public:	
	ATDS_WeaponDefault();

	FOnWeaponReloadStartSignature OnWeaponReloadStart;
	FOnWeaponReloadEndSignature OnWeaponReloadEnd;
	FOnWeaponFireSignature OnWeaponFire;

	UPROPERTY()
	FWeaponInfo WeaponSettings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponInfo")
	FAdditionalWeaponInfo AdditionalWeaponInfo;
	
	FVector ShootEndLocation = FVector(0);

	// Dispersion
	bool ShouldReduceDispersion = false;
	float CurrentDispersion = 0.0f;
	float CurrentDispersionMax = 1.0f;
	float CurrentDispersionMin = 0.1f;
	float CurrentDispersionRecoil = 1.0f;
	float CurrentDispersionReduction = 1.0f;

	UFUNCTION(BlueprintCallable)
	void SetWeaponStateFire(bool bIsFire);

	UFUNCTION(BlueprintCallable)
	int32 GetWeaponRound();

	void Fire();

	void UpdateStateWeapon(EMovementState InMovementState);

	void InitReload();
	void CancelReload();

	float GetReloadPercent() const { return ReloadTimer / WeaponSettings.ReloadTime; };

	float GetProjectilesQuantityPercent() const { return AdditionalWeaponInfo.Round / WeaponSettings.MaxRound; };

	bool GetReloadState() const { return WeaponReloading; }

	bool GetFireState() const { return WeaponFiring; }

	int32 GetNumberProjectileByShot() const;
	
	FProjectileInfo GetProjectile();
	
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Components")
	USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Components")
	USkeletalMeshComponent* SkeletalMeshWeapon = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Components")
	UStaticMeshComponent* StaticMeshWeapon = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Components")
	UArrowComponent* ShootLocation = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool ShowDebug = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	float SizeVectorToChangeShootDirectionLogic = 250.0f;

	UFUNCTION()
	void InitDropMesh(UStaticMesh* DropMesh, FTransform Offset, FVector DropImpulseDir, float LifeTimeMesh, float ImpulseRandDispersion, float PowerImpulse, float CustomMass);

	void FireTick(float DeltaTime);
	void ReloadTick(float DeltaTime);
	void DispersionTick(float DeltaTime);
	void ClipDropTick(float DeltaTime);
	void ShellDropTick(float DeltaTime);

	void WeaponInit();

	virtual void BeginPlay() override;

private:
	float TraceSpeed = 0.0f;

	// Timers
	float FireTimer = 0.0f;
	float ReloadTimer = 0.0f;

	FTimerHandle FXTimerHandle;
	
	// Flags
	bool WeaponFiring = false;
	bool WeaponReloading = false;
	bool WeaponAiming = false;
	bool BlockFire = false;

	// Timer Drop Magazine on reload
	bool DropClipFlag = false;
	float DropClipTimer = -1.0f;

	// Shell flag
	bool DropShellFlag = false;
	float DropShellTimer = -1.0f;

	bool CheckWeaponCanFire();

	void ChangeDispersionByShot();

	void FinishReload();

	FVector GetFireEndLocation() const;
	FVector ApplyDispersionToShoot(FVector DirectionShoot) const;
	float GetCurrentDispersion() const;

	void SpawnMuzzleEffects() const;
	void SpawnTrailEffect();
	void SpawnImpactEffects(const FHitResult& Hit);

	void HandleProjectileHit(FProjectileInfo ProjectileInfo);
	void HandleHitScan();
};