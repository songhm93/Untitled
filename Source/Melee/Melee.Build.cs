// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Melee : ModuleRules
{
	public Melee(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG" ,"AIModule", "GameplayTasks", "NavigationSystem", "PhysicsCore", "HTTP", "Json", "JsonUtilities"});
	}
}
