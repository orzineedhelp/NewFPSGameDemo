// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NewFPSGameDemo : ModuleRules
{
	public NewFPSGameDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"NewFPSGameDemo",
			"NewFPSGameDemo/Variant_Horror",
			"NewFPSGameDemo/Variant_Horror/UI",
			"NewFPSGameDemo/Variant_Shooter",
			"NewFPSGameDemo/Variant_Shooter/AI",
			"NewFPSGameDemo/Variant_Shooter/UI",
			"NewFPSGameDemo/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
