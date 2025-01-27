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

//TODO OMG Refactoring need!!!
bool UTDSInventoryComponent::SwitchWeaponToIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo, bool bIsForward)
{
	bool bIsSuccess = false;
	int8 CorrectIndex = ChangeToIndex;
	if (ChangeToIndex > WeaponSlots.Num() - 1)
		CorrectIndex = 0;
	else
		if (ChangeToIndex < 0)
			CorrectIndex = WeaponSlots.Num() - 1;

	FName NewIdWeapon;
	FAdditionalWeaponInfo NewAdditionalInfo;
	int32 NewCurrentIndex = 0;

	if (WeaponSlots.IsValidIndex(CorrectIndex))
	{
		if (!WeaponSlots[CorrectIndex].NameItem.IsNone())
		{
			if (WeaponSlots[CorrectIndex].AdditionalInfo.Round > 0)
			{
				//good weapon have ammo start change
				bIsSuccess = true;
			}
			else
			{
				UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
				if (myGI)
				{
					//check ammoSlots for this weapon
					FWeaponInfo myInfo;
					myGI->GetWeaponInfoByName(WeaponSlots[CorrectIndex].NameItem, myInfo);

					bool bIsFind = false;
					int8 j = 0;
					while (j < AmmoSlots.Num() && !bIsFind)
					{
						if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Bullets > 0)
						{
							//good weapon have ammo start change
							bIsSuccess = true;
							bIsFind = true;
						}
						j++;
					}
				}
			}
			if (bIsSuccess)
			{
				NewCurrentIndex = CorrectIndex;
				NewIdWeapon = WeaponSlots[CorrectIndex].NameItem;
				NewAdditionalInfo = WeaponSlots[CorrectIndex].AdditionalInfo;
			}
		}
	}

	if (!bIsSuccess)
	{
		if (bIsForward)
		{
			int8 iteration = 0;
			int8 Seconditeration = 0;
			while (iteration < WeaponSlots.Num() && !bIsSuccess)
			{
				iteration++;
				int8 tmpIndex = ChangeToIndex + iteration;
				if (WeaponSlots.IsValidIndex(tmpIndex))
				{
					if (!WeaponSlots[tmpIndex].NameItem.IsNone())
					{
						if (WeaponSlots[tmpIndex].AdditionalInfo.Round > 0)
						{
							//WeaponGood
							bIsSuccess = true;
							NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
							NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
							NewCurrentIndex = tmpIndex;
						}
						else
						{
							FWeaponInfo myInfo;
							UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());

							myGI->GetWeaponInfoByName(WeaponSlots[tmpIndex].NameItem, myInfo);

							bool bIsFind = false;
							int8 j = 0;
							while (j < AmmoSlots.Num() && !bIsFind)
							{
								if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Bullets > 0)
								{
									//WeaponGood
									bIsSuccess = true;
									NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
									NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
									NewCurrentIndex = tmpIndex;
									bIsFind = true;
								}
								j++;
							}
						}
					}
				}
				else
				{
					//go to end of right of array weapon slots
					if (OldIndex != Seconditeration)
					{
						if (WeaponSlots.IsValidIndex(Seconditeration))
						{
							if (!WeaponSlots[Seconditeration].NameItem.IsNone())
							{
								if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
								{
									//WeaponGood
									bIsSuccess = true;
									NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
									NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
									NewCurrentIndex = Seconditeration;
								}
								else
								{
									FWeaponInfo myInfo;
									UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());

									myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

									bool bIsFind = false;
									int8 j = 0;
									while (j < AmmoSlots.Num() && !bIsFind)
									{
										if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Bullets > 0)
										{
											//WeaponGood
											bIsSuccess = true;
											NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
											NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
											NewCurrentIndex = Seconditeration;
											bIsFind = true;
										}
										j++;
									}
								}
							}
						}
					}
					else
					{
						//go to same weapon when start
						if (WeaponSlots.IsValidIndex(Seconditeration))
						{
							if (!WeaponSlots[Seconditeration].NameItem.IsNone())
							{
								if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
								{
									//WeaponGood, it same weapon do nothing
								}
								else
								{
									FWeaponInfo myInfo;
									UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());

									myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

									bool bIsFind = false;
									int8 j = 0;
									while (j < AmmoSlots.Num() && !bIsFind)
									{
										if (AmmoSlots[j].WeaponType == myInfo.WeaponType)
										{
											if (AmmoSlots[j].Bullets > 0)
											{
												//WeaponGood, it same weapon do nothing
											}
											else
											{
												//Not find weapon with amm need init Pistol with infinity ammo
												UE_LOG(LogTemp, Error, TEXT("UTPSInventoryComponent::SwitchWeaponToIndex - Init PISTOL - NEED"));
											}
										}
										j++;
									}
								}
							}
						}
					}
					Seconditeration++;
				}
			}
		}
		else
		{
			int8 iteration = 0;
			int8 Seconditeration = WeaponSlots.Num() - 1;
			while (iteration < WeaponSlots.Num() && !bIsSuccess)
			{
				iteration++;
				int8 tmpIndex = ChangeToIndex - iteration;
				if (WeaponSlots.IsValidIndex(tmpIndex))
				{
					if (!WeaponSlots[tmpIndex].NameItem.IsNone())
					{
						if (WeaponSlots[tmpIndex].AdditionalInfo.Round > 0)
						{
							//WeaponGood
							bIsSuccess = true;
							NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
							NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
							NewCurrentIndex = tmpIndex;
						}
						else
						{
							FWeaponInfo myInfo;
							UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());

							myGI->GetWeaponInfoByName(WeaponSlots[tmpIndex].NameItem, myInfo);

							bool bIsFind = false;
							int8 j = 0;
							while (j < AmmoSlots.Num() && !bIsFind)
							{
								if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Bullets > 0)
								{
									//WeaponGood
									bIsSuccess = true;
									NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
									NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
									NewCurrentIndex = tmpIndex;
									bIsFind = true;
								}
								j++;
							}
						}
					}
				}
				else
				{
					//go to end of LEFT of array weapon slots
					if (OldIndex != Seconditeration)
					{
						if (WeaponSlots.IsValidIndex(Seconditeration))
						{
							if (!WeaponSlots[Seconditeration].NameItem.IsNone())
							{
								if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
								{
									//WeaponGood
									bIsSuccess = true;
									NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
									NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
									NewCurrentIndex = Seconditeration;
								}
								else
								{
									FWeaponInfo myInfo;
									UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());

									myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

									bool bIsFind = false;
									int8 j = 0;
									while (j < AmmoSlots.Num() && !bIsFind)
									{
										if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Bullets > 0)
										{
											//WeaponGood
											bIsSuccess = true;
											NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
											NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
											NewCurrentIndex = Seconditeration;
											bIsFind = true;
										}
										j++;
									}
								}
							}
						}
					}
					else
					{
						//go to same weapon when start
						if (WeaponSlots.IsValidIndex(Seconditeration))
						{
							if (!WeaponSlots[Seconditeration].NameItem.IsNone())
							{
								if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
								{
									//WeaponGood, it same weapon do nothing
								}
								else
								{
									FWeaponInfo myInfo;
									UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());

									myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

									bool bIsFind = false;
									int8 j = 0;
									while (j < AmmoSlots.Num() && !bIsFind)
									{
										if (AmmoSlots[j].WeaponType == myInfo.WeaponType)
										{
											if (AmmoSlots[j].Bullets > 0)
											{
												//WeaponGood, it same weapon do nothing
											}
											else
											{
												//Not find weapon with amm need init Pistol with infinity ammo
												UE_LOG(LogTemp, Error, TEXT("UTPSInventoryComponent::SwitchWeaponToIndex - Init PISTOL - NEED"));
											}
										}
										j++;
									}
								}
							}
						}
					}
					Seconditeration--;
				}
			}
		}
	}

	if (bIsSuccess)
	{
		SetAdditionalInfoWeapon(OldIndex, OldInfo);
		OnSwitchWeapon.Broadcast(NewIdWeapon, NewAdditionalInfo, NewCurrentIndex);
		//OnWeaponAmmoAviable.Broadcast()
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

FName UTDSInventoryComponent::GetWeaponNameBySlotIndex(int32 IndexSlot)
{
	FName Result;

	if (WeaponSlots.IsValidIndex(IndexSlot))
	{
		Result = WeaponSlots[IndexSlot].NameItem;
	}

	return Result;
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

bool UTDSInventoryComponent::SwitchWeaponToInventory(FWeaponSlot NewWeapon, int32 IndexSlot, int32 CurrentIndexWeaponChar, FDropItem& DropItemInfo)
{
	bool Result = false;
	if (WeaponSlots.IsValidIndex(IndexSlot) && GetDropItemInfoFromInventory(IndexSlot, DropItemInfo))
	{
		WeaponSlots[IndexSlot] = NewWeapon;

		SwitchWeaponToIndex(CurrentIndexWeaponChar, -1, NewWeapon.AdditionalInfo, true);

		OnUpdateWeaponSlots.Broadcast(IndexSlot, NewWeapon);

		Result = true;
	}

	return Result;
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

bool UTDSInventoryComponent::TryGetWeaponToInventory(FWeaponSlot NewWeapon)
{
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