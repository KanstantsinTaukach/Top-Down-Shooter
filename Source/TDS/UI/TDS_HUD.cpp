// Top Down Shooter. All Rights Reserved


#include "TDS_HUD.h"
#include "TSD_GameplayWidget.h"

void ATDS_HUD::BeginPlay()
{
	Super::BeginPlay();

	GameplayWidget = CreateWidget<UTSD_GameplayWidget>(GetWorld(), GameplayWidgetClass);
	check(GameplayWidget);

	GameplayWidget->AddToViewport();
}