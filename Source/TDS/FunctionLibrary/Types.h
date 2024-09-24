// Top Down Shooter. All Rights Reserved

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types.generated.h"

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Aim_State = 0 UMETA(DisplayName = "Aim State"),
	Walk_State UMETA(DisplayName = "Walk State"),
	Run_State UMETA(DisplayName = "Run State"),
	Sprint_State UMETA(DisplayName = "Sprint State")
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
struct FWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSettings")
	float WeaponDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSettings")
	float RateOfFire = 0.1f;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSettings")
	//FProjectileInfo ProjectileSettings;

};

//USTRUCT(BlueprintType)
//struct FProjectileInfo
//{
//	GENERATED_BODY()
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
//	TSubclassOf<class AProjectlieDefault> Projectile = nullptr;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSettings")
//	float ProjectileDamage = 20.0f;
//
//};


UCLASS()
class TDS_API UTypes : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};