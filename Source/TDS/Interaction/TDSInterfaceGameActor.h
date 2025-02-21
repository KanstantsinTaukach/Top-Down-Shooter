// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TDSInterfaceGameActor.generated.h"

class UTDSStateEffect;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTDSInterfaceGameActor : public UInterface
{
	GENERATED_BODY()
};

class TDS_API ITDSInterfaceGameActor
{
	GENERATED_BODY()

public:
	virtual EPhysicalSurface GetSurfaceType();

	virtual TArray<UTDSStateEffect*> GetAllCurrentEffects();

	virtual void RemoveEffect(UTDSStateEffect* EffectToRemove);
	virtual void AddEffect(UTDSStateEffect* EffectToAdd);
};
