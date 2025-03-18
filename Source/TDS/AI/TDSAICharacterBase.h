// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TDSAICharacterBase.generated.h"

class UTDSHealthComponent;

UCLASS()
class TDS_API ATDSAICharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ATDSAICharacterBase();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTDSHealthComponent* HealthComponent;

	UFUNCTION()
	virtual void OnAICharacterDeath();

	virtual void BeginPlay() override;

};
