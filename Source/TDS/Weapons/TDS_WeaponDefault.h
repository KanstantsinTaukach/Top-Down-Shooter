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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireLogic")
	FWeaponInfo WeaponSettings;

	void WeaponInit();

	virtual void BeginPlay() override;
};
