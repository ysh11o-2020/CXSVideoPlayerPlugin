// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SCXSMedia.h"
#include "Components/Widget.h"
#include "CXSVideoStructures.h"
#include "CXSMedia.generated.h"

/**
 * 
 */
UCLASS()
class CXSVIDEOPLAYERPLUGIN_API UCXSMedia : public UWidget
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFaceNum, const int, FaceNum);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlaying, const bool, bSuccess);

	UCXSMedia();
	virtual ~UCXSMedia() override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CXSMediaPlayer")
	bool bAutoVideo = true;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CXSMediaPlayer")
	FString VideoUrl;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CXSMediaPlayer")
	FVideoData VideoData;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (ClampMin=0.2f,ClampMax=5.0f),Category="CXSMediaPlayer")
	float PlayDelay = 0.2f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CXSMediaPlayer")
	FLinearColor BackGroundColor = FLinearColor(1.0f,1.0f,1.0f,1.0f);

	UFUNCTION(BlueprintCallable,Category="CXSMediaPlayer")
	void PlayVideo(FString Url,FVideoData Data);

	UFUNCTION(BlueprintCallable,Category="CXSMediaPlayer")
	void PauseVideo(bool bPause);

	UFUNCTION(BlueprintCallable,Category="CXSMediaPlayer")
	void StopVideo();

	UFUNCTION(BlueprintCallable,Category="CXSMediaPlayer")
	float GetFps();

	UFUNCTION(BlueprintCallable,BlueprintPure,Category="CXSMediaPlayer")
	UTexture2D* GetTexture();

	//v1.3
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CXSMediaPlayer")
	FString RecordPath = FPaths::ProjectDir().Append(TEXT("Record/Video.avi"));

	UFUNCTION(BlueprintCallable,Category="CXSMediaPlayer")
	bool PlayRecordVideo(FString FilePath);

	UFUNCTION(BlueprintCallable,Category="CXSMediaPlayer")
	void StopRecordVideo();

	UPROPERTY(BlueprintAssignable,Category="CXSMediaPlayer")
	FOnFaceNum OnFaceNum;

	UPROPERTY(BlueprintAssignable,Category="CXSMediaPlayer")
	FOnPlaying OnPlaying;

	void TimerPlayVideo(FString Url,FVideoData m_VideoData);
	FTimerHandle TimerHandle;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	TSharedPtr<SCXSMedia> CXSMediaPlayer;

	void HandleOnFaceNum(int FaceNum);
	void HandleOnPlaying(bool bSuccess);

public:
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif
};
