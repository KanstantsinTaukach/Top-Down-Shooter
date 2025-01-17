// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../FunctionLibrary/Types.h"
#include "TDSInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSwitchWeaponSignature, FName, WeaponIdName, FAdditionalWeaponInfo, WeaponAdditionalInfo, int32, NewCurrentIndexWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChangeSignature, EWeaponType, AmmoType, int32, Bullets);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponAdditionalInfoChangeSignature, int32, SlotIndex, FAdditionalWeaponInfo, WeaponAdditionalInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoEmptySignature, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateWeaponSlotsSignature, int32, IndexSlotChange, FWeaponSlot, NewInfo);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TDS_API UTDSInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	bool CheckCanTakeWeapon(int32& FreeSlot);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	TArray<FWeaponSlot> WeaponSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	TArray<FAmmoSlot> AmmoSlots;

	int32 MaximumSlotIndex = 0;

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

	UFUNCTION(BlueprintCallable)
	void AmmoSlotChangeValue(EWeaponType WeaponType, int32 BullestToChange);

	UFUNCTION(BlueprintCallable)
	bool CheckCanTakeAmmo(EWeaponType WeaponType);

	UFUNCTION(BlueprintCallable)
	bool TryGetWeaponToInventory(FWeaponSlot NewWeapon);

	void SwitchWeaponToInventory();

	const TArray<FWeaponSlot>& GetWeaponSlots() const { return WeaponSlots; };

	bool SwitchWeaponToIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo, bool bIsForward);

	int32 GetWeaponIndexSlotByName(FName IdWeaponName);

	void SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo);

	bool CheckAmmoForWeapon(EWeaponType WeaponType, int32 &AvailableAmmoForWeapon);

	FAdditionalWeaponInfo GetAdditionalInfoWeapon(int32 IndexWeapon);
};