// Top Down Shooter. All Rights Reserved

#pragma once

class AnimUtils
{
public:
	static UAnimMontage* FindAnimToPlay(UAnimMontage* AnimWithAim, UAnimMontage* AnimWithoutAim, bool HasAim)
	{
		if (!AnimWithAim && !AnimWithoutAim)
		{
			return nullptr;
		}
		UAnimMontage* AnimToPlay = nullptr;
		if (HasAim)
		{
			AnimToPlay = AnimWithAim;
		}
		else
		{
			AnimToPlay = AnimWithoutAim;
		}
		return AnimToPlay;
	}

	static UAnimMontage* GetRandomAnimation(const TArray<UAnimMontage*>& Animations)
	{
		if (Animations.Num() == 0)
		{
			return nullptr;
		}

		int32 RandomIndex = FMath::RandRange(0, Animations.Num() - 1);
		return Animations[RandomIndex];
	}
};
