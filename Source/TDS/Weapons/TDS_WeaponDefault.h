// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../FunctionLibrary/Types.h"
#include "TDS_WeaponDefault.generated.h"

class UArrowComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponReloadStartSignature, UAnimMontage*, AnimMontage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponReloadEndSignature);

UCLASS()
class TDS_API ATDS_WeaponDefault : public AActor
{
	GENERATED_BODY()
	
public:	
	ATDS_WeaponDefault();

	FOnWeaponReloadStartSignature OnWeaponReloadStart;
	FOnWeaponReloadEndSignature OnWeaponReloadEnd;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FireLogic")
	bool WeaponFiring = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FireLogic")
	bool WeaponReloading = false;

	UPROPERTY()
	FWeaponInfo WeaponSettings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponInfo")
	FAdditionalWeaponInfo WeaponInfo;

	//flags
	bool BlockFire = false;
	//dispersion
	bool ShouldReduceDispersion = false;
	float CurrentDispersion = 0.0f;
	float CurrentDispersionMax = 1.0f;
	float CurrentDispersionMin = 0.1f;
	float CurrentDispersionRecoil = 1.0f;
	float CurrentDispersionReduction = 1.0f;

	FVector ShootEndLocation = FVector(0);

	UFUNCTION(BlueprintCallable)
	void SetWeaponStateFire(bool bIsFire);

	UFUNCTION(BlueprintCallable)
	int32 GetWeaponRound();

	void Fire();

	void UpdateStateWeapon(EMovementState InMovementState);

	void InitReload();

	float GetReloadPercent() const { return ReloadTimer / WeaponSettings.ReloadTime; };

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
	float SizeVectorToChangeShootDirectionLogic = 100.0f;

	void FireTick(float DeltaTime);
	void ReloadTick(float DeltaTime);
	void DispersionTick(float DeltaTime);

	void WeaponInit();

	virtual void BeginPlay() override;

private:
	float FireTimer = 0.0f; //Timers flags
	float ReloadTimer = 0.0f;

	bool CheckWeaponCanFire();

	void ChangeDispersionByShot();

	void FinishReload();

	FVector GetFireEndLocation() const;
	FVector ApplyDispersionToShoot(FVector DirectionShoot) const;
	float GetCurrentDispersion() const;
};
