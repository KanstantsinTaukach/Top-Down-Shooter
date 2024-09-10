// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDSGameMode.h"
#include "TDSPlayerController.h"
#include "../Character/TDSCharacter.h"
#include "../UI/TDS_HUD.h"
#include "UObject/ConstructorHelpers.h"

ATDSGameMode::ATDSGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ATDSPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Character/BP_Character"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ATDSGameMode::StartPlay()
{
	Super::StartPlay();
	
	auto* PC = GetWorld()->GetFirstPlayerController();
	check(PC);
	HUD = Cast<ATDS_HUD>(PC->GetHUD());
	check(HUD);
}

