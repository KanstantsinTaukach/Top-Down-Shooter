// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

	virtual void Tick(float DeltaTime) override;

protected:
	UNiagaraComponent* NiagaraComponent;

	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	USphereComponent* CollisionComponent = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	UStaticMeshComponent* PickupStaticMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	USkeletalMeshComponent* PickupSkeletalMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintreadWrite, Category = "VFX")
	UNiagaraSystem* PickupVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintreadWrite, Category = "Sound")
	USoundBase* PickupSound = nullptr;
	
	virtual void BeginPlay();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

private:
	float RotationYaw = 0.0f;

	virtual bool GivePickupTo(APawn* PlayerPawn);

	void InitSelf();

	void GenerateRotationYaw();
};