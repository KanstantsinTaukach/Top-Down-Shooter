// Top Down Shooter. All Rights Reserved

#include "TDSEnvironmentStructure.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Materials/MaterialInterface.h"
#include "../BuffSystem/TDSStateEffect.h"

DEFINE_LOG_CATEGORY_STATIC(TDSEnvironmentStructure, All, All);

ATDSEnvironmentStructure::ATDSEnvironmentStructure()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATDSEnvironmentStructure::BeginPlay()
{
	Super::BeginPlay();
}

EPhysicalSurface ATDSEnvironmentStructure::GetSurfaceType()
{
	EPhysicalSurface Result = ITDSInterfaceGameActor::GetSurfaceType();

	UStaticMeshComponent* MyMesh = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (MyMesh)
	{
		UMaterialInterface* MyMaterial = MyMesh->GetMaterial(0);
		if (MyMaterial)
		{
			Result = MyMaterial->GetPhysicalMaterial()->SurfaceType;
		}
	}
	return Result;
}

TArray<UTDSStateEffect*> ATDSEnvironmentStructure::GetAllCurrentEffects()
{
	return StateEffects;
}

void ATDSEnvironmentStructure::RemoveEffect(UTDSStateEffect* EffectToRemove)
{
	StateEffects.Remove(EffectToRemove);
}

void ATDSEnvironmentStructure::AddEffect(UTDSStateEffect* EffectToAdd)
{
	StateEffects.Add(EffectToAdd);
}
