// Top Down Shooter. All Rights Reserved

#pragma once

class TDSUtils
{
public:
	template<typename T> static T* GetTDSPlayerComponent(AActor* PlayerPawn)
	{
		if (!PlayerPawn)
		{
			return nullptr;
		}

		const auto Component = PlayerPawn->GetComponentByClass(T::StaticClass());
		return Cast<T>(Component);
	}
};