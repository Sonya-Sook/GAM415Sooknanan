// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MyProject415 : ModuleRules
{
	public MyProject415(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Niagara" });
	}
}
