// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
//using Internal;
using UnrealBuildTool;

public class CXSVideoPlayerPlugin : ModuleRules
{
	private string ThirdPartyPath
	{
		get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../ThirdParty/")); }
	}

	public void LoadOpenCV()
	{
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			string OpenCVPath = Path.Combine(ThirdPartyPath, "OpenCV/");
			string IncludePath = Path.Combine(OpenCVPath, "include/");
			string BinPath = "bin/";
			string LibPath = "lib/";
			string XmlPath = "xml/";
			
			PublicIncludePaths.Add(Path.Combine(OpenCVPath,IncludePath));
			PublicSystemLibraryPaths.Add(Path.Combine(OpenCVPath,LibPath));

			DirectoryInfo LibFolder = new DirectoryInfo(Path.Combine(OpenCVPath,LibPath));
			foreach (FileInfo file in LibFolder.GetFiles("*.lib"))
			{
				PublicAdditionalLibraries.Add(file.FullName);
			}

			DirectoryInfo DllFolder = new DirectoryInfo(Path.Combine(OpenCVPath, BinPath));
			foreach (FileInfo file in DllFolder.GetFiles("*.dll"))
			{
				PublicDelayLoadDLLs.Add(file.Name);
				RuntimeDependencies.Add(file.FullName);
			}

			DirectoryInfo XMLFolder = new DirectoryInfo(Path.Combine(OpenCVPath, XmlPath));
			foreach (FileInfo file in XMLFolder.GetFiles("*.xml"))
			{
				RuntimeDependencies.Add("$(ProjectDir)/Binaries/Win64/ThirdParty/OpenCV/xml/"+file.Name,file.FullName);
			}
			PublicDefinitions.Add("WITH_OPENCV=1");
		}
	}

	public CXSVideoPlayerPlugin(ReadOnlyTargetRules Target) : base(Target)
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
				"UMG",
				"RenderCore",
				"RHI",
				//"DesktopPlatform",
				"ApplicationCore",
				"InputCore",
				"Projects"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
		LoadOpenCV();
	}
}
