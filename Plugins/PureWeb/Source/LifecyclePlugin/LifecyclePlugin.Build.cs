// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class LifecyclePlugin : ModuleRules
{
	public LifecyclePlugin (ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        var EngineDir = Path.GetFullPath(Target.RelativeEnginePath);

        PrivateIncludePaths.AddRange(
            new string[] {
                EngineDir + "/Plugins/Media/PixelStreaming/Source/PixelStreaming/Private/"
			}
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core"
            }
            );

        PrivateDependencyModuleNames.AddRange(new string[]
        {
                "Core",
                "CoreUObject",
                "Engine",
                "Sockets",
                "Json",
                "JsonUtilities",
                "InputDevice",
                "PixelStreaming",
                "Http"
        });

        bLegacyPublicIncludePaths = false;
        ShadowVariableWarningLevel = WarningLevel.Error;
        DefaultBuildSettings = BuildSettingsVersion.V2;
    }
}
