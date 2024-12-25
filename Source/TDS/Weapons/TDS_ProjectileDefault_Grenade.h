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

protected:
	virtual void BeginPlay() override;

private:
	bool TimerEnabled = false;
	float TimerToExplode = 0.0f;
	float TimeToExplode = 2.0f;
};
