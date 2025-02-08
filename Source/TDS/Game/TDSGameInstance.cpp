// Top Down Shooter. All Rights Reserved

#include "TDSGameInstance.h"

DEFINE_LOG_CATEGORY_STATIC(TDSGameInstanseLog, All, All);

bool UTDSGameInstance::GetWeaponInfoByName(FName WeaponName, FWeaponInfo& WeaponInfo)
{
	bool bIsFind = false;
	FWeaponInfo* WeaponInfoRow;

	if (WeaponInfoTable)
	{
		WeaponInfoRow = WeaponInfoTable->FindRow<FWeaponInfo>(WeaponName, "", false);
		if (WeaponInfoRow)
		{
			bIsFind = true;
			WeaponInfo = *WeaponInfoRow;
		}
	}
	else
	{
		UE_LOG(TDSGameInstanseLog, Warning, TEXT("UTDSGameInstance::GetWeaponInfoByName - WeaponTable is NULL!"));
	}	

	return bIsFind;
}

bool UTDSGameInstance::GetDropItemInfoByName(FName ItemName, FDropItem& DropItemInfo)
{
	bool bIsFind = false;

	if (DropItemInfoTable)
	{
		FDropItem* DropItemInfoRow;
		TArray<FName>RowNames = DropItemInfoTable->GetRowNames();

		int8 i = 0;
		while (i < RowNames.Num() && !bIsFind)
		{
			DropItemInfoRow = DropItemInfoTable->FindRow<FDropItem>(RowNames[i], "");
			if (DropItemInfoRow->WeaponDropSlot.NameItem == ItemName)
			{
				bIsFind = true;
				DropItemInfo = *DropItemInfoRow;
			}
			++i;
		}
	}
	else
	{
		UE_LOG(TDSGameInstanseLog, Warning, TEXT("UTDSGameInstance::GetDropItemInfoByName - DropItemInfoTable is NULL!"));
	}

	return bIsFind;
}
