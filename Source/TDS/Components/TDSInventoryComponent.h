// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../FunctionLibrary/Types.h"
#include "TDSInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSwitchWeaponSignature, FName, WeaponIdName, FAdditionalWeaponInfo, WeaponAdditionalInfo);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TDS_API UTDSInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	TArray<FWeaponSlot> WeaponSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	TArray<FAmmoSlot> AmmoSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	int32 MaximumSlotIndex = 0;
	
	FOnSwitchWeaponSignature OnSwitchWeapon;

	UTDSInventoryComponent();

	bool SwitchWeaponToIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo);

	int32 GetWeaponIndexSlotByName(FName IdWeaponName);

protected:
	virtual void BeginPlay() override;

	FAdditionalWeaponInfo GetAdditionalInfoWeapon(int32 IndexWeapon);

	void SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo);
};
