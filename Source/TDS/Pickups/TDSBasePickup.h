// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../FunctionLibrary/Types.h"
#include "TDSBasePickup.generated.h"

class USphereComponent;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class TDS_API ATDSBasePickup : public AActor
{
	GENERATED_BODY()

public:
	ATDSBasePickup();

	void PickupSuccess();

	bool GetIsPlayerInOverlap() const { return IsPlayerInOverlap; };

	virtual void Tick(float DeltaTime) override;

protected:
	UNiagaraComponent* NiagaraComponent;

	bool IsPlayerInOverlap = false;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	USphereComponent* CollisionComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintreadWrite, Category = "VFX")
	UNiagaraSystem* PickupVFX;

	UPROPERTY(EditAnywhere, BlueprintreadWrite, Category = "Sound")
	USoundBase* PickupSound = nullptr;
	
	virtual void BeginPlay();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	void SpawnPickupVFX();

private:
	float RotationYaw = 0.0f;

	virtual bool GivePickupTo(APawn* PlayerPawn);

	void GenerateRotationYaw();
};