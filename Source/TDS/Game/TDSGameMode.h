// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TDSGameMode.generated.h"

class ATDS_HUD;

UCLASS(minimalapi)
class ATDSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATDSGameMode();
	virtual void StartPlay() override;

private:
	UPROPERTY()
	ATDS_HUD* HUD;
};



