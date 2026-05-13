// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../FunctionLibrary/Types.h"
#include "TDS_WeaponDefault.generated.h"

class UArrowComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponFireSignature, UAnimMontage*, AnimMontage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponReloadStartSignature, UAnimMontage*, AnimMontage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponReloadEndSignature, bool, bIsSuccess, int32, AmmoLeft);

UCLASS()
class TDS_API ATDS_WeaponDefault : public AActor
{
	GENERATED_BODY()
	
public:	
	ATDS_WeaponDefault();
	
	virtual void Tick(float DeltaTime) override;

	FOnWeaponReloadStartSignature OnWeaponReloadStart;
	FOnWeaponReloadEndSignature OnWeaponReloadEnd;
	FOnWeaponFireSignature OnWeaponFire;

	// Dispersion
	UPROPERTY(Replicated)
	bool ShouldReduceDispersion = false;
	float CurrentDispersion = 0.0f;
	float CurrentDispersionMax = 1.0f;
	float CurrentDispersionMin = 0.1f;
	float CurrentDispersionRecoil = 1.0f;
	float CurrentDispersionReduction = 1.0f;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetWeaponStateFire_OnServer(bool bIsFire);

	UFUNCTION(BlueprintCallable)
	int32 GetWeaponRound();

	void Fire();

	UFUNCTION(Server, Reliable)
	void UpdateStateWeapon_OnServer(EMovementState InMovementState);

	void InitReload();
	void CancelReload();

	float GetReloadPercent() const { return ReloadTimer / WeaponSettings.ReloadTime; };

	bool GetReloadState() const { return WeaponReloading; }

	int32 GetNumberProjectileByShot() const;
	
	FProjectileInfo GetProjectile();

	bool CheckWeaponCanReload();

	const FWeaponInfo& GetWeaponInfo() const { return WeaponSettings; };
	void SetWeaponInfo(FWeaponInfo NewWeaponInfo) { WeaponSettings = NewWeaponInfo; };

	const FAdditionalWeaponInfo& GetAdditionWeaponInfo() const { return AdditionalWeaponInfo; };
	void SetAdditionWeaponInfo(FAdditionalWeaponInfo NewAdditionWeaponInfo) { AdditionalWeaponInfo = NewAdditionWeaponInfo; };
	
	UFUNCTION(Server, Unreliable)
	void UpdateShootEndLocationByCharacter_OnServer(FVector NewShootEndLocation, bool NewShouldReduceDispersion);

	UFUNCTION(NetMulticast, Reliable)
	void PlayWeaponAnimation_Multicast(UAnimMontage* WeaponAnimation);

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

	UPROPERTY()
	FWeaponInfo WeaponSettings;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "WeaponInfo")
	FAdditionalWeaponInfo AdditionalWeaponInfo;

	UPROPERTY(Replicated)
	FVector ShootEndLocation = FVector(0);

	UFUNCTION(NetMulticast, Unreliable)
	void InitDropMesh_Multicast(const FDropMeshInfo& DropMeshInfo);

	UFUNCTION(NetMulticast, Reliable)
	void PlayReloadSound_Multicast(USoundBase* HitSound, FVector SoundLocation);

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

	// Variables used to control and update the movement of a projectile trail effect
	FTimerHandle FXTimerHandle;
	UParticleSystemComponent* ActiveTrailFX = nullptr;
	FVector ActiveTrailDirection;
	FVector ActiveTrailEnd;
	float ActiveTrailTravelTime = 0.0f;

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

	UFUNCTION(NetMulticast, Unreliable)
	void SpawnMuzzleEffects_Multicast() const;
	
	void SpawnTrailEffect();
	void UpdateTrailEffect();
	void SpawnImpactEffects(const FHitResult& Hit);

	void HandleProjectileHit(FProjectileInfo ProjectileInfo);
	void HandleHitScan();

	int32 GetAmmoForReload();
};