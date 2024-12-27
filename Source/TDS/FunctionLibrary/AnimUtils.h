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
};
