// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interaction/TDSInterfaceGameActor.h"
#include "TDSEnvironmentStructure.generated.h"

UCLASS()
class TDS_API ATDSEnvironmentStructure : public AActor, public ITDSInterfaceGameActor
{
	GENERATED_BODY()
	
public:	
	ATDSEnvironmentStructure();

protected:
	virtual void BeginPlay() override;

public:
	EPhysicalSurface GetSurfaceType() override;
};
