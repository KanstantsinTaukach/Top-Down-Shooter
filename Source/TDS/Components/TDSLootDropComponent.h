// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../FunctionLibrary/Types.h"
#include "TDSLootDropComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TDS_API UTDSLootDropComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTDSLootDropComponent();

	void DropLoot(const FVector& Location);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	TArray<FLootDropChance> LootChances;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DropChance = 0.3f;

	virtual void BeginPlay() override;

private:
	bool HasExecuted = false;
};
