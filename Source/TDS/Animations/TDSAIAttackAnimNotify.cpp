// Top Down Shooter. All Rights Reserved

#include "TDSAIAttackAnimNotify.h"

void UTDSAIAttackAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	OnNotified.Broadcast(MeshComp);
	Super::Notify(MeshComp, Animation);
}
