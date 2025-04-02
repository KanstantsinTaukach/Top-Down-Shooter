// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TDS : ModuleRules
{
    public TDS(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        { 
          "Core",
          "CoreUObject",
          "Engine",
          "InputCore",
          "HeadMountedDisplay",
          "NavigationSystem",
          "AIModule",
          "UMG",
          "Niagara",
          "PhysicsCore",
          "Slate",
          "GameplayTasks"
        });

        PublicIncludePaths.AddRange(new string[] 
        { 
            "TDS/Character", 
            "TDS/FunctionLibrary", 
            "TDS/Game", 
            "TDS/Components", 
            "TDS/UI", 
            "TDS/Weapons",
            "TDS/BuffSystem",
            "TDS/Environment",
            "TDS/Interaction",
            "TDS/Pickups",
            "TDS/AI",
            "TDS/Animations",
            "TDS/AI/Services",
        });
    }
}