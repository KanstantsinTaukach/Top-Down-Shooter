// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TDSAIInterfaceCombat.generated.h"

UINTERFACE(MinimalAPI)
class UTDSAIInterfaceCombat : public UInterface
{
	GENERATED_BODY()
};

class TDS_API ITDSAIInterfaceCombat
{
	GENERATED_BODY()

public:
	virtual bool CheckCanAttack() const;

	virtual void LightAttack();
	virtual void HeavyAttack();
};
