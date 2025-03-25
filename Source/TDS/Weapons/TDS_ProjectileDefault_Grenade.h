// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "TDS_ProjectileDefault.h"
#include "TDS_ProjectileDefault_Grenade.generated.h"

UCLASS()
class TDS_API ATDS_ProjectileDefault_Grenade : public ATDS_ProjectileDefault
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileType")
	bool HasDelayBeforeExplosion = false;

	virtual void Tick(float DeltaTime) override;

	void TimerExplode(float DeltaTime);

	void BulletCollisionSphereHit(class UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void ImpactProjectile() override;

	void Explode();

	void UpdateLuringNoise();

protected:
	virtual void BeginPlay() override;

private:
	bool TimerEnabled = false;
	float TimerToExplode = 0.0f;
	float TimeToExplode = 3.5f;

	bool IsLuringEnemies = true;
	float NoiseUpdateInterval = 0.01f;
	float NoiseUpdateTimer = 0.0f;

	bool IsSoundPlaying = false;

	UPROPERTY()
	UAudioComponent* FlightSoundComponent;

	UFUNCTION()
	void OnSoundFinished();

	void GetActorsInRange(UWorld* World, const FVector& Origin, float Radius, TArray<AActor*>& OutActors);
};