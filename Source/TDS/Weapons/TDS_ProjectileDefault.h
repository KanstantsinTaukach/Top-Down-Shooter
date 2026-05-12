// Top Down Shooter. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../FunctionLibrary/Types.h"
#include "TDS_ProjectileDefault.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;

UCLASS()
class TDS_API ATDS_ProjectileDefault : public AActor
{
	GENERATED_BODY()
	
public:	
	ATDS_ProjectileDefault();

	FProjectileInfo GetProjectileSettings() const { return ProjectileSettings; };

	UFUNCTION(BlueprintCallable)
	void InitProjectile(FProjectileInfo InitParam);

	UFUNCTION()
	virtual void BulletCollisionSphereHit(class UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void BulletCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void BulletCollisionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	virtual void ImpactProjectile();

	UFUNCTION(NetMulticast, Reliable)
	void InitVisualMeshProjectile_Multicast(UStaticMesh* NewMesh, FTransform MeshTransform);
	UFUNCTION(NetMulticast, Reliable)
	void InitVisualTrailFX_Multicast(UParticleSystem* NewParticleSystem, FTransform ParticleTransform);

protected:
	bool ShouldSpawnStateEffect = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Components")
	USphereComponent* BulletCollisionSphere = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Components")
	UStaticMeshComponent* BulletMesh = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Components")
	UParticleSystemComponent* BulletFX = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Components")
	UProjectileMovementComponent* BulletProjectileMovement = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FProjectileInfo ProjectileSettings;

	virtual void BeginPlay() override;
};
