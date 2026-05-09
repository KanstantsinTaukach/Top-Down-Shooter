// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDSGameMode.h"
#include "../UI/TDS_HUD.h"
#include "UObject/ConstructorHelpers.h"

void ATDSGameMode::StartPlay()
{
	Super::StartPlay();
	
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		GetWorldTimerManager().SetTimerForNextTick([this, PC]()
		{
			HUD = Cast<ATDS_HUD>(PC->GetHUD());
		});
	}
}