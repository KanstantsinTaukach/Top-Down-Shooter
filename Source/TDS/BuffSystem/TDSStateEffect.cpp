// Top Down Shooter. All Rights Reserved

#include "TDSStateEffect.h"
#include "../Components/TDSHealthComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(UTDSStateEffectLog, All, All);

bool UTDSStateEffect::InitObject(AActor* Actor)
{
	if (!Actor)
	{
		return false;
	}

	TargetActor = Actor;
	return true;
}

void UTDSStateEffect::DestroyObject()
{
	TargetActor = nullptr;

	if (this && this->IsValidLowLevel())
	{
		this->ConditionalBeginDestroy();
	}
}



bool UTDSStateEffect_ExecuteOnce::InitObject(AActor* Actor)
{
	if (!Super::InitObject(Actor))
	{
		return false;
	}

	ExecuteOnce();
	return true;
}

void UTDSStateEffect_ExecuteOnce::DestroyObject()
{
	Super::DestroyObject();
}

void UTDSStateEffect_ExecuteOnce::ExecuteOnce()
{
	if (TargetActor)
	{
		if (!TargetActor->FindComponentByClass(UTDSHealthComponent::StaticClass()))
		{
			UE_LOG(UTDSStateEffectLog, Warning, TEXT("UTDSStateEffect::ExecuteOnce: HealthComponent not found on %s"), *TargetActor->GetName());
			return;
		}

		auto HealthComponent = Cast<UTDSHealthComponent>(TargetActor->GetComponentByClass(UTDSHealthComponent::StaticClass()));
		if (HealthComponent)
		{
			HealthComponent->RemoveFromCurrentHealth(PowerOfOnceEffect);
		}
	}

	DestroyObject();
}



bool UTDSStateEffect_ExecuteTimer::InitObject(AActor* Actor)
{
	if (!Super::InitObject(Actor))
	{
		return false;
	}

	CurrentPowerOfTimerEffect = PowerOfTimerEffect;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(ExecuteEffectlTimerHandle, this, &UTDSStateEffect_ExecuteTimer::ExecuteOnTimer, EffectUpdateTime, true, -1.0f);
	}

	if (EffectFX)
	{
		FName NameBoneToAttach;
		FVector EffectLocation = FVector(0);
		ParticleEffect = UGameplayStatics::SpawnEmitterAttached(EffectFX, TargetActor->GetRootComponent(), NameBoneToAttach, EffectLocation, FRotator::ZeroRotator,EAttachLocation::SnapToTarget, false);

		if (!ParticleEffect)
		{
			UE_LOG(UTDSStateEffectLog, Warning, TEXT("UTDSStateEffect_ExecuteTimer::InitObject: Failed to spawn ParticleEffect"));
			return false;
		}
	}

	return true;
}

void UTDSStateEffect_ExecuteTimer::DestroyObject()
{
	CurrentPowerOfTimerEffect = PowerOfTimerEffect;

	ParticleEffect->DestroyComponent();
	ParticleEffect = nullptr;

	Super::DestroyObject();
}

void UTDSStateEffect_ExecuteTimer::ExecuteOnTimer()
{
	if (GetWorld() && TargetActor)
	{
		if (!TargetActor->FindComponentByClass(UTDSHealthComponent::StaticClass()))
		{
			UE_LOG(UTDSStateEffectLog, Warning, TEXT("UTDSStateEffect::ExecuteOnce: HealthComponent not found on %s"), *TargetActor->GetName());
			GetWorld()->GetTimerManager().ClearTimer(ExecuteEffectlTimerHandle);
			return;
		}

		auto HealthComponent = Cast<UTDSHealthComponent>(TargetActor->GetComponentByClass(UTDSHealthComponent::StaticClass()));
		if (HealthComponent)
		{
			HealthComponent->RemoveFromCurrentHealth(EffectRate);
			CurrentPowerOfTimerEffect -= EffectRate;

			if (CurrentPowerOfTimerEffect <= 0)
			{
				GetWorld()->GetTimerManager().ClearTimer(ExecuteEffectlTimerHandle);
				DestroyObject();
			}
		}
	}
}