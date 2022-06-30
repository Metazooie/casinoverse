using UnrealBuildTool;
using System.IO;
using System;
using Microsoft.Win32;

public class AgentPlugin : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../ThirdParty/")); }
    }

    private string BinariesPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../Binaries/Win64")); }
    }

    public AgentPlugin(ReadOnlyTargetRules Target) : base(Target)
    {


        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicIncludePaths.AddRange(
            new string[] {
                // ... add public include paths required here ...
            }
            );
                
        
        PrivateIncludePaths.AddRange(
            new string[] {
                // ... add other private include paths required here ...
            }
            );
            
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                // ... add other public dependencies that you statically link with here ...
            }
            );
            
        
        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "Json",
                "JsonUtilities",
                // ... add private dependencies that you statically link with here ...    
            }
            );
        
        
        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
                // ... add any modules that your module loads dynamically here ...
            }
            );
        PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "platform-sdk-cpp/x64-windows-unreal/include"));

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            string LibrariesPath = Path.Combine(ThirdPartyPath, "platform-sdk-cpp/x64-windows-unreal/lib");
            DirectoryInfo d = new DirectoryInfo(LibrariesPath);
            FileInfo[] Files = d.GetFiles("*.lib");
            foreach (FileInfo file in Files)
            {
                PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, file.Name));
            }

            //Create plugin binary directory and Copy third party binaries into it
            string ThirdPartyBinariesPath = Path.Combine(ThirdPartyPath, "platform-sdk-cpp/x64-windows-unreal/bin");
            d = new DirectoryInfo(ThirdPartyBinariesPath);
            Directory.CreateDirectory(BinariesPath);
            Files = d.GetFiles("*.*");
            foreach (FileInfo file in Files)
            {
                RuntimeDependencies.Add("$(BinaryOutputDir)/" + file.Name, file.FullName);
                try
                {
                    File.Copy(Path.Combine(ThirdPartyBinariesPath, file.Name), Path.Combine(BinariesPath, file.Name), true);
                }
                catch (Exception e)
                {
                    //If the files were already copied system will throw an exception. 
                    //This will happen when packaging the application from unreal editor
                }
            }
        }
    }
}
