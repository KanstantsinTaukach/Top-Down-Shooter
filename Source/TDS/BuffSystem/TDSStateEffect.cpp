// Top Down Shooter. All Rights Reserved

#include "TDSStateEffect.h"

DEFINE_LOG_CATEGORY_STATIC(UTDSStateEffectLog, All, All);

bool UTDSStateEffect::InitObject()
{
	UE_LOG(UTDSStateEffectLog, Display, TEXT("UTDSStateEffect::InitObject: Init is OK"));
	return true;
}

void UTDSStateEffect::ExecuteEffect(float DeltaTime)
{

}

void UTDSStateEffect::DestroyObject()
{

}

bool UTDSStateEffect::CheckStackableEffect()
{
	return true;
}