// Fill out your copyright notice in the Description page of Project Settings.


#include "CXSMedia.h"
#include "SCXSMedia.h"

#define LOCTEXT_NAMESPACE "CXSMedia"

UCXSMedia::UCXSMedia()
{
}

UCXSMedia::~UCXSMedia()
{
	StopVideo();
}

void UCXSMedia::PlayVideo(FString Url, FVideoData Data)
{
	if(GetWorld()->GetTimerManager().IsTimerActive(TimerHandle))
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,[=,this]()
	{
		UCXSMedia::TimerPlayVideo(Url,Data);
	},1.0f,false,PlayDelay);
}

void UCXSMedia::PauseVideo(bool bPause)
{
	if(CXSMediaPlayer)
	{
		CXSMediaPlayer->PlayPause(bPause);
	}
}

void UCXSMedia::StopVideo()
{
	if(CXSMediaPlayer)
	{
		CXSMediaPlayer->PlayStop();
	}
}

float UCXSMedia::GetFps()
{
	if(CXSMediaPlayer)
	{
		return CXSMediaPlayer->GetFps();
	}
	return 0;
}

UTexture2D* UCXSMedia::GetTexture()
{
	if(CXSMediaPlayer)
	{
		return CXSMediaPlayer->GetTexture();
	}
	return nullptr;
}

bool UCXSMedia::PlayRecordVideo(FString FilePath)
{
	if (CXSMediaPlayer)
	{
		if (!FilePath.IsEmpty())
		{
			FilePath=FPaths::ConvertRelativePathToFull(*FilePath);
			//去掉文件名 只要目录信息
			FString m_FilePath=FPaths::GetPath(FilePath);
			//检查目录信息是否正确
			if (!FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*m_FilePath))
			{
				//目录信息不正确时创建目录
				FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*m_FilePath);
			}
		}
		else
		{
			int64 timestamp = (FDateTime::Now() - FDateTime(1970, 1, 1)).GetTotalMilliseconds();
			FString TimestampStr = FString::Printf(TEXT("RecordVideo_%lld"), timestamp);
			FilePath=FPaths::ProjectDir().Append(FString::Printf(TEXT("RecordVideo/%s.avi"), *TimestampStr));
			FString m_FilePath=FPaths::GetPath(FilePath);
			//检查目录信息是否正确
			if (!FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*m_FilePath))
			{
				//目录信息不正确时创建目录
				FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*m_FilePath);
			}
		}
		return CXSMediaPlayer->PlayRecordVideo(FilePath);
	}
	return false;
}

void UCXSMedia::StopRecordVideo()
{
	if(CXSMediaPlayer)
	{
		CXSMediaPlayer->StopRecordVideo();
	}
}

void UCXSMedia::TimerPlayVideo(FString Url, FVideoData m_VideoData)
{
	if(CXSMediaPlayer)
	{
		CXSMediaPlayer->PlayVideo(Url,m_VideoData);
	}
}

TSharedRef<SWidget> UCXSMedia::RebuildWidget()
{
	if ( IsDesignTime() )
	{
		return SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("CXS Media", "CXS Media"))
			];
	}
	else
	{
		CXSMediaPlayer=SNew(SCXSMedia)
		.VideoData(VideoData)
		.BackGroundColor(BackGroundColor)
		.OnFaceNumDelegate(BIND_UOBJECT_DELEGATE(FOnFaceNumDelegate,HandleOnFaceNum))
		.OnPlayingDelegate(BIND_UOBJECT_DELEGATE(FOnPlayingDelegate,HandleOnPlaying));
		if (bAutoVideo)
		{
			if (CXSMediaPlayer)
			{
				CXSMediaPlayer->PlayVideo(VideoUrl,VideoData);
			}
		}
		return CXSMediaPlayer.ToSharedRef();
	}
}

void UCXSMedia::HandleOnFaceNum(int FaceNum)
{
	if(OnFaceNum.IsBound())
	{
		OnFaceNum.Broadcast(FaceNum);
	}
}

void UCXSMedia::HandleOnPlaying(bool bSuccess)
{
	if(OnPlaying.IsBound())
	{
		OnPlaying.Broadcast(bSuccess);
	}
}

#if WITH_EDITOR
const FText UCXSMedia::GetPaletteCategory()
{
	return LOCTEXT("CXSMedia", "CXSMedia");
}
#endif
#undef LOCTEXT_NAMESPACE
