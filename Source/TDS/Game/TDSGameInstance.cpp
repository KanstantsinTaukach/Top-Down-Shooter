// Top Down Shooter. All Rights Reserved

#include "TDSGameInstance.h"

DEFINE_LOG_CATEGORY_STATIC(TDSGameInstanseLog, All, All);

bool UTDSGameInstance::GetWeaponInfoByName(FName WeaponName, FWeaponInfo& OutInfo)
{
	bool bIsFind = false;
	FWeaponInfo* WeaponInfoRow;

	if (WeaponInfoTable)
	{
		WeaponInfoRow = WeaponInfoTable->FindRow<FWeaponInfo>(WeaponName, "", false);
		if (WeaponInfoRow)
		{
			bIsFind = true;
			OutInfo = *WeaponInfoRow;
		}
	}
	else
	{
		UE_LOG(TDSGameInstanseLog, Warning, TEXT("UTDSGameInstance::GetWeaponInfoByName - WeaponTable is NULL!"));
	}	

	return bIsFind;
}
