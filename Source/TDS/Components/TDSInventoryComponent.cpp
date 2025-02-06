// Top Down Shooter. All Rights Reserved

#include "TDSInventoryComponent.h"
#include "../Game/TDSGameInstance.h"

DEFINE_LOG_CATEGORY_STATIC(TDSInventoryComponentLog, All, All);

UTDSInventoryComponent::UTDSInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTDSInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	for (int8 i = 0; i < WeaponSlots.Num(); ++i)
	{
		UTDSGameInstance* GameInstance = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());

		if (GameInstance && !WeaponSlots[i].NameItem.IsNone())
		{
			FWeaponInfo Info;
			if (GameInstance->GetWeaponInfoByName(WeaponSlots[i].NameItem, Info))
			{
				WeaponSlots[i].AdditionalInfo.Round = Info.MaxRound;
			}
		}
	}

	MaximumSlotIndex = WeaponSlots.Num();

	if (WeaponSlots.IsValidIndex(0))
	{
		if (!WeaponSlots[0].NameItem.IsNone())
		{
			OnSwitchWeapon.Broadcast(WeaponSlots[0].NameItem, WeaponSlots[0].AdditionalInfo, 0);
		}
	}
}

bool UTDSInventoryComponent::SwitchWeaponToIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo, bool bIsForward)
{
	if (WeaponSlots.Num() == 0)
	{
		return false;
	}

	int32 CorrectIndex = NormalizeIndex(ChangeToIndex);
	bool bIsSuccess = TrySwitchToWeapon(CorrectIndex, OldIndex);

	if (!bIsSuccess)
	{
		bIsSuccess = bIsForward ? FindNextWeapon(CorrectIndex, OldIndex) : FindPreviousWeapon(CorrectIndex, OldIndex);
	}

	if (bIsSuccess)
	{
		SetAdditionalInfoWeapon(OldIndex, OldInfo);
		OnSwitchWeapon.Broadcast(CurrentWeaponId, CurrentAdditionalInfo, CurrentWeaponIndex);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::SwitchWeaponToIndex - No valid weapon found."));
	}

	return bIsSuccess;
}

int32 UTDSInventoryComponent::NormalizeIndex(int32 Index) const
{
	if (Index < 0)
	{
		return WeaponSlots.Num() - 1;
	}

	if (Index >= WeaponSlots.Num())
	{
		return 0;
	}
		
	return Index;
}

bool UTDSInventoryComponent::TrySwitchToWeapon(int32 Index, int32 OldIndex)
{
	if (!WeaponSlots.IsValidIndex(Index))
	{
		return false;
	}

	const FWeaponSlot& Slot = WeaponSlots[Index];

	if (Slot.NameItem.IsNone())
	{
		return false;
	}

	if (Slot.AdditionalInfo.Round > 0 || HasAmmoForWeapon(Slot.NameItem))
	{
		CurrentWeaponIndex = Index;
		CurrentWeaponId = Slot.NameItem;
		CurrentAdditionalInfo = Slot.AdditionalInfo;
		return true;
	}

	return false;
}

bool UTDSInventoryComponent::HasAmmoForWeapon(FName WeaponName) const
{
	UTDSGameInstance* GameInstance = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
	if (!GameInstance)
	{
		return false;
	}

	FWeaponInfo WeaponInfo;
	if (!GameInstance->GetWeaponInfoByName(WeaponName, WeaponInfo))
	{
		return false;
	}

	for (const FAmmoSlot& AmmoSlot : AmmoSlots)
	{
		if (AmmoSlot.WeaponType == WeaponInfo.WeaponType && AmmoSlot.Bullets > 0)
		{
			return true;
		}
	}

	return false;
}

bool UTDSInventoryComponent::FindNextWeapon(int32 StartIndex, int32 OldIndex)
{
	for (int32 Offset = 1; Offset < WeaponSlots.Num(); ++Offset)
	{
		int32 NextIndex = NormalizeIndex(StartIndex + Offset);
		if (NextIndex == OldIndex)
		{
			break;
		}

		if (TrySwitchToWeapon(NextIndex, OldIndex))
		{
			return true;
		}
	}

	return false;
}

bool UTDSInventoryComponent::FindPreviousWeapon(int32 StartIndex, int32 OldIndex)
{
	for (int32 Offset = 1; Offset < WeaponSlots.Num(); ++Offset)
	{
		int32 PrevIndex = NormalizeIndex(StartIndex - Offset);
		if (PrevIndex == OldIndex)
		{
			break;
		}

		if (TrySwitchToWeapon(PrevIndex, OldIndex))
		{
			return true;
		}
	}

	return false;
}

FAdditionalWeaponInfo UTDSInventoryComponent::GetAdditionalInfoWeapon(int32 IndexWeapon)
{
	FAdditionalWeaponInfo result;

	if (WeaponSlots.IsValidIndex(IndexWeapon))
	{
		bool bIsFind = false;
		int8 i = 0;
		while (i < WeaponSlots.Num() && !bIsFind)
		{
			if (i == IndexWeapon)
			{
				result = WeaponSlots[i].AdditionalInfo;
				bIsFind = true;
			}
			++i;
		}

		if (!bIsFind)
		{
			UE_LOG(TDSInventoryComponentLog, Warning, TEXT("UTDSInventoryComponent::GetAdditionalInfoWeapon - Don't find weapon with index - %d"), IndexWeapon);
		}
	}
	else
	{
		UE_LOG(TDSInventoryComponentLog, Warning, TEXT("UTDSInventoryComponent::GetAdditionalInfoWeapon - Uncorrect weapon index - %d"), IndexWeapon);
	}

	return result;
}

FName UTDSInventoryComponent::GetWeaponNameBySlotIndex(int32 IndexSlot)
{
	FName Result;

	if (WeaponSlots.IsValidIndex(IndexSlot))
	{
		Result = WeaponSlots[IndexSlot].NameItem;
	}

	return Result;
}

int32 UTDSInventoryComponent::GetWeaponIndexSlotByName(FName IdWeaponName)
{
	int32 result = -1;
	int8 i = 0;
	bool bIsFind = false;
	while (i < WeaponSlots.Num() && !bIsFind)
	{
		if (WeaponSlots[i].NameItem == IdWeaponName)
		{
			bIsFind = true;
			result = i;
		}
		++i;
	}

	return result;
}

void UTDSInventoryComponent::SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo)
{
	if (WeaponSlots.IsValidIndex(IndexWeapon))
	{
		bool bIsFind = false;
		int8 i = 0;
		while (i < WeaponSlots.Num() && !bIsFind)
		{
			if (i == IndexWeapon)
			{
				WeaponSlots[i].AdditionalInfo = NewInfo;
				bIsFind = true;

				OnWeaponAdditionalInfoChange.Broadcast(IndexWeapon, NewInfo);
			}
			++i;
		}

		if (!bIsFind)
		{
			UE_LOG(TDSInventoryComponentLog, Warning, TEXT("UTDSInventoryComponent::SetAdditionalInfoWeapon - Don't find weapon with index - %d"), IndexWeapon);
		}
	}
	else
	{
		UE_LOG(TDSInventoryComponentLog, Warning, TEXT("UTDSInventoryComponent::SetAdditionalInfoWeapon - Uncorrect weapon index - %d"), IndexWeapon);
	}
}

bool UTDSInventoryComponent::CheckAmmoForWeapon(EWeaponType WeaponType, int32& AvailableAmmoForWeapon)
{
	AvailableAmmoForWeapon = 0;
	bool bIsFind = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !bIsFind)
	{
		if (AmmoSlots[i].WeaponType == WeaponType)
		{
			if (AmmoSlots[i].Bullets > 0)
			{
				AvailableAmmoForWeapon = AmmoSlots[i].Bullets;
				bIsFind = true;
			}
		}
		++i;
	}

	if (bIsFind == false)
	{
		OnWeaponAmmoEmpty.Broadcast(WeaponType);
	}
	return bIsFind;
}

void UTDSInventoryComponent::AmmoSlotChangeValue(EWeaponType WeaponType, int32 BullestToChange)
{
	bool bIsFind = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !bIsFind)
	{
		if (AmmoSlots[i].WeaponType == WeaponType)
		{
			AmmoSlots[i].Bullets += BullestToChange;

			if (AmmoSlots[i].Bullets > AmmoSlots[i].MaxBullets)
			{
				AmmoSlots[i].Bullets = AmmoSlots[i].MaxBullets;
			}

			OnAmmoChange.Broadcast(AmmoSlots[i].WeaponType, AmmoSlots[i].Bullets);

			bIsFind = true;
		}
		++i;
	}
}

bool UTDSInventoryComponent::CheckCanTakeAmmo(EWeaponType WeaponType)
{
	bool Result = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !Result)
	{
		if (AmmoSlots[i].WeaponType == WeaponType && AmmoSlots[i].Bullets < AmmoSlots[i].MaxBullets)
		{
			Result = true;
		}
		++i;
	}

	return Result;
}

bool UTDSInventoryComponent::TryGetWeaponToInventory(FWeaponSlot NewWeapon)
{
	for (int j = 0; j < WeaponSlots.Num(); ++j)
	{
		if (WeaponSlots[j].NameItem == NewWeapon.NameItem)
		{
			return false;
		}
	}

	int8 i = 0;
	while (i < WeaponSlots.Num())
	{
		if (WeaponSlots[i].NameItem.IsNone() && WeaponSlots.IsValidIndex(i))
		{
			WeaponSlots[i] = NewWeapon;
			OnUpdateWeaponSlots.Broadcast(i, NewWeapon);
			return true;
		}
		++i;
	}

	return false;
}

bool UTDSInventoryComponent::SwitchWeaponToInventory(FWeaponSlot NewWeapon, int32 IndexSlot, FDropItem& DropItemInfo)
{
	for (int i = 0; i < WeaponSlots.Num(); ++i)
	{
		if (WeaponSlots[i].NameItem == NewWeapon.NameItem)
		{
			UE_LOG(TDSInventoryComponentLog, Display, TEXT("UTDSInventoryComponent::SwitchWeaponToInventory - Weapon already exists in inventory"));
			return false;
		}
	}

	if (WeaponSlots.IsValidIndex(IndexSlot) && GetDropItemInfoFromInventory(IndexSlot, DropItemInfo))
	{
		WeaponSlots[IndexSlot] = NewWeapon;

		SwitchWeaponToIndex(IndexSlot, IndexSlot, NewWeapon.AdditionalInfo, true);

		OnUpdateWeaponSlots.Broadcast(IndexSlot, NewWeapon);
		UE_LOG(TDSInventoryComponentLog, Display, TEXT("UTDSInventoryComponent::SwitchWeaponToInventory - Weapon switched successfully"));

		return true;
	}

	UE_LOG(TDSInventoryComponentLog, Error, TEXT("UTDSInventoryComponent::SwitchWeaponToInventory - Invalid slot or falied to get drop item"));
	return false;
}

bool UTDSInventoryComponent::GetDropItemInfoFromInventory(int32 IndexSlotToDrop, FDropItem& DropItemInfo)
{
	bool Result = false;

	FName DropItemName = GetWeaponNameBySlotIndex(IndexSlotToDrop);

	UTDSGameInstance* MyGameInstance = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
	if (MyGameInstance)
	{
		Result = MyGameInstance->GetDropItemInfoByName(DropItemName, DropItemInfo);
		DropItemInfo.WeaponSlotInfo.AdditionalInfo = WeaponSlots[IndexSlotToDrop].AdditionalInfo;
	}

	return Result;
}