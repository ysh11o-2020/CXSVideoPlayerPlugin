// Copyright Epic Games, Inc. All Rights Reserved.

#include "CXSVideoPlayerPlugin.h"

#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FCXSVideoPlayerPluginModule"

void FCXSVideoPlayerPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
#if PLATFORM_WINDOWS
	FString BinariesPath = FPaths::ConvertRelativePathToFull(FPaths::Combine(*FPaths::ProjectDir(),TEXT("Binaries/Win64/ThirdParty/OpenCV/xml/")));
	if(!IFileManager::Get().DirectoryExists(*BinariesPath))
	{
		FString OpenCVXml;
		OpenCVXml=FPaths::ConvertRelativePathToFull(IPluginManager::Get().FindPlugin(TEXT("CXSVideoPlayerPlugin"))->GetBaseDir() + "/Source/ThirdParty/OpenCV/xml/");
		TArray<FString> XmlArray;
		IFileManager::Get().FindFiles(XmlArray,*OpenCVXml,TEXT(".xml"));
		for(auto Xml:XmlArray)
		{
			IFileManager::Get().Copy(*(BinariesPath+Xml),*(OpenCVXml+Xml));
		}
	}
	FString OpenCVPath;
	OpenCVPath = FPaths::ConvertRelativePathToFull(IPluginManager::Get().FindPlugin(TEXT("CXSVideoPlayerPlugin"))->GetBaseDir() + "/Source/ThirdParty/OpenCV/bin/");

	DllHandle = FPlatformProcess::GetDllHandle(*(OpenCVPath + "opencv_world480.dll"));
	FfmpegDllHandle = FPlatformProcess::GetDllHandle(*(OpenCVPath + "opencv_videoio_ffmpeg480_64.dll"));
	MsmfDllHandle = FPlatformProcess::GetDllHandle(*(OpenCVPath + "opencv_videoio_msmf480_64.dll"));
#endif
}

void FCXSVideoPlayerPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	if(DllHandle)
	{
		FPlatformProcess::FreeDllHandle(DllHandle);
		DllHandle = nullptr;
	}
	if(FfmpegDllHandle)
	{
		FPlatformProcess::FreeDllHandle(FfmpegDllHandle);
		FfmpegDllHandle = nullptr;
	}
	if(MsmfDllHandle)
	{
		FPlatformProcess::FreeDllHandle(MsmfDllHandle);
		MsmfDllHandle = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FCXSVideoPlayerPluginModule, CXSVideoPlayerPlugin)