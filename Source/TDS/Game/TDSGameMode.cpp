// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDSGameMode.h"
#include "TDSPlayerController.h"
#include "../Character/TDSCharacter.h"
#include "../UI/TDS_HUD.h"
#include "UObject/ConstructorHelpers.h"

ATDSGameMode::ATDSGameMode()
{

}

void ATDSGameMode::StartPlay()
{
	Super::StartPlay();
	
	auto* PC = GetWorld()->GetFirstPlayerController();
	check(PC);
	HUD = Cast<ATDS_HUD>(PC->GetHUD());
	check(HUD);
}

