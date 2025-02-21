// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interaction/TDSInterfaceGameActor.h"
#include "TDSEnvironmentStructure.generated.h"

class UTDSStateEffect;

UCLASS()
class TDS_API ATDSEnvironmentStructure : public AActor, public ITDSInterfaceGameActor
{
	GENERATED_BODY()
	
public:	
	ATDSEnvironmentStructure();

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<UTDSStateEffect*> StateEffects;

	virtual void BeginPlay() override;

public:
	EPhysicalSurface GetSurfaceType() override;

	TArray<UTDSStateEffect*> GetAllCurrentEffects() override;

	void RemoveEffect(UTDSStateEffect* EffectToRemove) override;
	void AddEffect(UTDSStateEffect* EffectToAdd) override;
};
