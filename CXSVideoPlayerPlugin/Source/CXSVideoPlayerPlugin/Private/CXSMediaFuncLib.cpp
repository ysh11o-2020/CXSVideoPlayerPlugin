// Fill out your copyright notice in the Description page of Project Settings.


#include "CXSMediaFuncLib.h"

#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Windows/WindowsWindow.h"

TArray<FString> UCXSMediaFuncLib::OpenMediaFile()
{
	TArray<FString> Paths;
	const FString FileType = TEXT("SAVE file type (*.mp4; *.avi; *.webm)|*.mp4;*.MP4;*.avi;*.AVI;*.webm;*.WEBM");
	const FString DefaultPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	const TSharedPtr<FGenericWindow> NativeWindow = GEngine->GameViewport->GetWindow()->GetNativeWindow();
	const auto Window = static_cast<FWindowsWindow*>(NativeWindow.Get());
	const auto Hwnd = Window->GetHWnd();
	DesktopPlatform->OpenFileDialog(Hwnd,TEXT("Load Face Pinch File"),DefaultPath,TEXT(""),*FileType,EFileDialogFlags::None,Paths);
	return Paths;
}
