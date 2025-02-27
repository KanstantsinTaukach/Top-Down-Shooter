// Top Down Shooter. All Rights Reserved


#include "TDS_HUD.h"
#include "TDS_GameplayWidget.h"

void ATDS_HUD::BeginPlay()
{
	Super::BeginPlay();

	GameplayWidget = CreateWidget<UTDS_GameplayWidget>(GetWorld(), GameplayWidgetClass);
	check(GameplayWidget);

	GameplayWidget->AddToViewport();
}