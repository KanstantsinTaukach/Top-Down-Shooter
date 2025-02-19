// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TDSStateEffect.generated.h"

UCLASS(Blueprintable, BlueprintType)
class TDS_API UTDSStateEffect : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TArray<TEnumAsByte<EPhysicalSurface>> InteractEffects;

	virtual bool InitObject();

	virtual void ExecuteEffect(float DeltaTime);

	virtual void DestroyObject();

	virtual bool CheckStackableEffect();
};
