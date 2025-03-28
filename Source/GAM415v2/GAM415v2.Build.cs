// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GAM415v2 : ModuleRules
{
	public GAM415v2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Niagara", "ProceduralMeshComponent" });
	}
}
