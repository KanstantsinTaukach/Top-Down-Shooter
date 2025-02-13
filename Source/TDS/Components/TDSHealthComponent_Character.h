// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "TDSHealthComponent.h"
#include "TDSHealthComponent_Character.generated.h"

UCLASS()
class TDS_API UTDSHealthComponent_Character : public UTDSHealthComponent
{
	GENERATED_BODY()
	
public:
	virtual void ChangeCurrentHealth(float HealthValue) override;
};
