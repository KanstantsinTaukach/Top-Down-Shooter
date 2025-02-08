// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../FunctionLibrary/Types.h"
#include "TDSInventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnSwitchWeaponSignature, FName, FAdditionalWeaponInfo, int32);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChangeSignature, EWeaponType, AmmoType, int32, Bullets);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponAdditionalInfoChangeSignature, int32, SlotIndex, FAdditionalWeaponInfo, WeaponAdditionalInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateWeaponSlotsSignature, int32, IndexSlotChange, FWeaponSlot, NewInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoEmptySignature, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoAvailableSignature, EWeaponType, WeaponType);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TDS_API UTDSInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTDSInventoryComponent();

	virtual void BeginPlay() override;

	FOnSwitchWeaponSignature OnSwitchWeapon;

	UPROPERTY(BlueprintAssignable)
	FOnAmmoChangeSignature OnAmmoChange;
	UPROPERTY(BlueprintAssignable)
	FOnWeaponAdditionalInfoChangeSignature OnWeaponAdditionalInfoChange;
	UPROPERTY(BlueprintAssignable)
	FOnWeaponAmmoEmptySignature OnWeaponAmmoEmpty;
	UPROPERTY(BlueprintAssignable)
	FOnUpdateWeaponSlotsSignature OnUpdateWeaponSlots;
	UPROPERTY(BlueprintAssignable)
	FOnWeaponAmmoAvailableSignature OnWeaponAmmoAvailable;

	UFUNCTION(BlueprintCallable)
	void AmmoSlotChangeValue(EWeaponType WeaponType, int32 BullestToChange);

	UFUNCTION(BlueprintCallable)
	bool CheckCanTakeAmmo(EWeaponType WeaponType);

	UFUNCTION(BlueprintCallable)
	bool TryGetWeaponToInventory(FWeaponSlot NewWeapon);

	UFUNCTION(BlueprintCallable)
	bool SwitchWeaponToInventory(FWeaponSlot NewWeapon, int32 IndexSlot, FDropItem& DropItemInfo);

	UFUNCTION(BlueprintCallable)
	bool GetDropItemInfoFromInventory(int32 IndexSlotToDrop, FDropItem & DropItemInfo);

	const TArray<FWeaponSlot>& GetWeaponSlots() const { return WeaponSlots; };

	bool SwitchWeaponToIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo, bool bIsForward);

	FName GetWeaponNameBySlotIndex(int32 IndexSlot);
	int32 GetWeaponIndexSlotByName(FName IdWeaponName);

	void SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo);

	bool CheckAmmoForWeapon(EWeaponType WeaponType, int32 &AvailableAmmoForWeapon);

	FAdditionalWeaponInfo GetAdditionalInfoWeapon(int32 IndexWeapon);

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool IsWeaponExistsInInventory() const { return WeaponExistsInInventory; };
	void SetIsWeaponExistsInInventory(bool Exists) { WeaponExistsInInventory = Exists; };

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool GetIsNewPickupWeaponAllowed() const { return IsNewPickupWeaponAllowed; };
	void SetIsNewPickupWeaponAllowed(bool Exists) { IsNewPickupWeaponAllowed = Exists; };

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	TArray<FWeaponSlot> WeaponSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	TArray<FAmmoSlot> AmmoSlots;

	int32 MaximumSlotIndex = 0;

	bool WeaponExistsInInventory = false;
	bool IsNewPickupWeaponAllowed = false;

private:
	int32 NormalizeIndex(int32 Index) const;
	bool TrySwitchToWeapon(int32 Index, int32 OldIndex);
	bool HasAmmoForWeapon(FName WeaponName) const;
	bool FindNextWeapon(int32 StartIndex, int32 OldIndex);
	bool FindPreviousWeapon(int32 StartIndex, int32 OldIndex);

	// this class fields needed for the SwitchWeaponToInventory() function
	int32 CurrentWeaponIndex = -1;
	FName CurrentWeaponId;
	FAdditionalWeaponInfo CurrentAdditionalInfo;
	//
};
