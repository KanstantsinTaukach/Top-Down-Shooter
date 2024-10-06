// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../FunctionLibrary/Types.h"
#include "TDS_WeaponDefault.generated.h"

class UArrowComponent;

UCLASS()
class TDS_API ATDS_WeaponDefault : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FireLogic")
	bool WeaponFiring = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FireLogic")
	bool WeaponReloading = false;

	UPROPERTY()
	FWeaponInfo WeaponSettings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponInfo")
	FAdditionalWeaponInfo WeaponInfo;

	UFUNCTION(BlueprintCallable)
	void SetWeaponStateFire(bool bIsFire);

	UFUNCTION(BlueprintCallable)
	int32 GetWeaponRound();

	void Fire();

	void UpdateStateWeapon(EMovementState InMovementState);

	void InitReload();

	FProjectileInfo GetProjectile();

	ATDS_WeaponDefault();	

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

	void FireTick(float DeltaTime);
	void ReloadTick(float DeltaTime);

	void WeaponInit();

	virtual void BeginPlay() override;

private:
	float FireTimer = 0.0f; //Timers flags
	float ReloadTimer = 0.0f;

	bool CheckWeaponCanFire();

	void ChangeDispersion();

	void FinishReload();
};
