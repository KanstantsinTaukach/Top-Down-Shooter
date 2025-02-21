// Top Down Shooter. All Rights Reserved


#include "TDSInterfaceGameActor.h"
#include "../BuffSystem/TDSStateEffect.h"

EPhysicalSurface ITDSInterfaceGameActor::GetSurfaceType()
{
    return EPhysicalSurface::SurfaceType_Default;
}

TArray<UTDSStateEffect*> ITDSInterfaceGameActor::GetAllCurrentEffects()
{
    TArray<UTDSStateEffect*> CurrentEffects;
    return CurrentEffects;
}

void ITDSInterfaceGameActor::RemoveEffect(UTDSStateEffect* EffectToRemove)
{
}

void ITDSInterfaceGameActor::AddEffect(UTDSStateEffect* EffectToAdd)
{
}
