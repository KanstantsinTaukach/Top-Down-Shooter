// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../FunctionLibrary/Types.h"
#include "TDSInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSwitchWeaponSignature, FName, WeaponIdName, FAdditionalWeaponInfo, WeaponAdditionalInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChangeSignature, EWeaponType, AmmoType, int32, Count);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponAdditionalInfoChangeSignature, int32, SlotIndex, FAdditionalWeaponInfo, AdditionalWeaponInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoEmptySignature, EWeaponType, WeaponType);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TDS_API UTDSInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	TArray<FWeaponSlot> WeaponSlots;





	FOnSwitchWeaponSignature OnSwitchWeapon;

	UPROPERTY(BlueprintAssignable)
	FOnAmmoChangeSignature OnAmmoChange;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponAdditionalInfoChangeSignature OnWeaponAdditionalInfoChange;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponAmmoEmptySignature OnWeaponAmmoEmpty;

	UTDSInventoryComponent();

	bool SwitchWeaponToIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo);

	int32 GetWeaponIndexSlotByName(FName IdWeaponName);

	void AmmoSlotChangeValue(EWeaponType WeaponType, int32 AmmoToSubtract);

	void SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo);

	bool CheckAmmoForWeapon(EWeaponType WeaponType, int32 &AvailableAmmoForWeapon);

protected:


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	TArray<FAmmoSlot> AmmoSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	int32 MaximumSlotIndex = 0;

	virtual void BeginPlay() override;

	FAdditionalWeaponInfo GetAdditionalInfoWeapon(int32 IndexWeapon);
};