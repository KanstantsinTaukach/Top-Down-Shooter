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
			else
			{
				WeaponSlots.RemoveAt(i);
				--i;
			}
		}
	}

	MaximumSlotIndex = WeaponSlots.Num();

	if (WeaponSlots.IsValidIndex(0))
	{
		if (!WeaponSlots[0].NameItem.IsNone())
		{
			OnSwitchWeapon.Broadcast(WeaponSlots[0].NameItem, WeaponSlots[0].AdditionalInfo);
		}
	}
}

bool UTDSInventoryComponent::SwitchWeaponToIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo)
{
	bool bIsSuccess = false;
	int8 CorrectIndex = ChangeToIndex;
	if (ChangeToIndex > WeaponSlots.Num() - 1)
	{
		CorrectIndex = 0;
	}
	else
	{
		if (ChangeToIndex < 0)
		{
			CorrectIndex = WeaponSlots.Num() - 1;
		}
	}

	FName NewIdWeapon;
	FAdditionalWeaponInfo NewAdditionalInfo;

	int8 i = 0;
	while (i < WeaponSlots.Num() && !bIsSuccess)
	{
		if (i == CorrectIndex)
		{
			if (!WeaponSlots[i].NameItem.IsNone())
			{
				NewIdWeapon = WeaponSlots[i].NameItem;
				NewAdditionalInfo = WeaponSlots[i].AdditionalInfo;
				bIsSuccess = true;
			}
		}
		++i;
	}

	if (!bIsSuccess)
	{
		//weapon switch is not success
	}

	if (bIsSuccess)
	{
		SetAdditionalInfoWeapon(OldIndex, OldInfo);
		OnSwitchWeapon.Broadcast(NewIdWeapon, NewAdditionalInfo);
	}

	return bIsSuccess;
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



//// need refactoring



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


///need refactoring


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
			if (AmmoSlots[i].Count > 0)
			{
				AvailableAmmoForWeapon = AmmoSlots[i].Count;
				bIsFind = true;
			}
		}
		++i;
	}

	return bIsFind;
}

void UTDSInventoryComponent::AmmoSlotChangeValue(EWeaponType WeaponType, int32 AmmoToSubtract)
{
	bool bIsFind = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !bIsFind)
	{
		if (AmmoSlots[i].WeaponType == WeaponType)
		{
			AmmoSlots[i].Count += AmmoToSubtract;

			if (AmmoSlots[i].Count > AmmoSlots[i].MaxCount)
			{
				AmmoSlots[i].Count = AmmoSlots[i].MaxCount;
			}

			OnAmmoChange.Broadcast(AmmoSlots[i].WeaponType, AmmoSlots[i].Count);

			bIsFind = true;
		}
		++i;
	}
}
