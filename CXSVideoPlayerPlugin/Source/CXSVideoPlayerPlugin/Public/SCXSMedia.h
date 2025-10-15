// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "queue"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SOverlay.h"
#include "Engine/Texture2D.h"
#include "Rendering/Texture2DResource.h"
#include "Components/Image.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"
#include "Runtime/Core/Public/HAL/Runnable.h"
#include "CXSVideoStructures.h"
THIRD_PARTY_INCLUDES_START

#pragma push_macro("CONSTEXPR")
#undef CONSTEXPR
#pragma push_macro("check")
#undef check
#pragma push_macro("dynamic_cast")
#undef dynamic_cast
#pragma push_macro("PI")
#undef PI
#include "opencv2/core/utility.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/core/fast_math.hpp"
#include "opencv2/imgproc/types_c.h"
#include "opencv2/objdetect.hpp"
#pragma pop_macro("PI")
#pragma pop_macro("check")
#pragma pop_macro("dynamic_cast")
#pragma pop_macro("CONSTEXPR")

THIRD_PARTY_INCLUDES_END

DECLARE_DELEGATE_OneParam(FOnFaceNumDelegate,const int);
DECLARE_DELEGATE_OneParam(FOnPlayingDelegate,const bool);

class FCXSMediaRunnable : public FRunnable
{
public:
	FCXSMediaRunnable(const FString& ThreadName,const FString Url,FVideoData VideoData);
	virtual ~FCXSMediaRunnable() override;

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

	FString m_ThreadName;
	int32 m_ThreadID;

	cv::Scalar m_HsvMin;
	cv::Scalar m_HsvMax;
	
	bool bPause = false;
	FRunnableThread* ThreadIns;
	FEvent* ThreadEvent;
	bool bRun;
	bool bPlay;

	cv::VideoCapture VideoCapture = cv::VideoCapture();
	cv::Mat Mat = cv::Mat();
	cv::Mat FrameRHI = cv::Mat();
	cv::Mat Gray = cv::Mat();
	cv::Mat SmallMat = cv::Mat();
	std::queue<cv::Mat> Frames;
	TArray<FColor> MatRHIData = TArray<FColor>();
	int m_TimeOut = 3000;

	FString m_Url;
	bool m_bAutoFps;
	float m_CustomFps;
	double Video_Fps;
	//v1.3
	double VideoFps;
	//
	FVector2D m_Size;
	bool bVideoOpen;
	ERenderMode M_RenderMode;
	EMedia m_MediaType;

	void PlayPause(bool bPlayPause);
	bool PlayStop();
	bool InitVideo();
	void UpdateFrame();
	void UpdateRHIData();
	float GetFps();

	void FaceDetection(const cv::Mat InMat);
	cv::Mat ChromaKey(const cv::Mat InMat);

	cv::CascadeClassifier FaceCascade;
	std::vector<cv::Rect> Rect;
	bool m_bEnableKey = false;
	int m_Scale = 4;
	EDrawType m_DrawType;
	EHaarXml m_HaarXml;
	cv::Scalar m_Color = CV_RGB(255, 0, 0);
	FOnFaceNumDelegate OnFaceNumDelegate;
	FOnPlayingDelegate OnPlayingDelegate;

	//v1.3
	bool bRecordVideo;
	std::string m_VideoName;
	cv::VideoWriter m_Video;

	bool PlayRecordVideo(const FString& FilePath);
	void StopRecordVideo();
};



class CXSVIDEOPLAYERPLUGIN_API SCXSMedia : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCXSMedia)
	{}
	SLATE_ARGUMENT(FVideoData,VideoData)
	SLATE_ARGUMENT(FLinearColor,BackGroundColor);//v1.2

	SLATE_EVENT(FOnFaceNumDelegate,OnFaceNumDelegate);
	SLATE_EVENT(FOnPlayingDelegate,OnPlayingDelegate);

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	void PlayVideo(FString m_Url,FVideoData m_VideoData);
	void PlayPause(bool bPause);
	void PlayStop();
	void UpdateTexture(const cv::Mat& InMat);
	float GetFps();
	UTexture2D* GetTexture();
	FOnFaceNumDelegate OnFaceNumDelegate;
	FOnPlayingDelegate OnPlayingDelegate;
	void GetFaceNum(int FaceNum);
	void OnPlaying(bool bSuccess);

	bool PlayRecordVideo(const FString& FilePath);
	void StopRecordVideo();

private:
	TSharedPtr<SImage> MediaImage;
	FVector2D MediaSize;
	UTexture2D* MediaTexture;
	FUpdateTextureRegion2D* MediaUpdateTextureRegion;
	FSlateBrush* VideoBrush;
	FCXSMediaRunnable* MediaRunnable;

	FVideoData VideoData;
	FLinearColor BackGroundColor;
	float Fps=30;
};
