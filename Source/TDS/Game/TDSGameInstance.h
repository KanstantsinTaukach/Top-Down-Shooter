// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../FunctionLibrary/Types.h"
#include "Engine/DataTable.h"
#include "../Weapons/TDS_WeaponDefault.h"
#include "TDSGameInstance.generated.h"

UCLASS()
class TDS_API UTDSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSetting")
	UDataTable* WeaponInfoTable = nullptr;

	UFUNCTION(BlueprintCallable)
	bool GetWeaponInfoByName(FName WeaponName, FWeaponInfo& OutInfo);
};
