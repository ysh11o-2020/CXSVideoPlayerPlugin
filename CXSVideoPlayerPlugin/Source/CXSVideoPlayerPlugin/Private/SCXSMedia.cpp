// Fill out your copyright notice in the Description page of Project Settings.


#include "SCXSMedia.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SCXSMedia::Construct(const FArguments& InArgs)
{
	VideoData = InArgs._VideoData;
	BackGroundColor = InArgs._BackGroundColor;//v1.2
	OnFaceNumDelegate = InArgs._OnFaceNumDelegate;
	OnPlayingDelegate = InArgs._OnPlayingDelegate;
	VideoBrush = new FSlateBrush();
	VideoBrush->SetResourceObject(MediaTexture);
	ChildSlot
	[
		SNew(SOverlay)
		+SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SAssignNew(MediaImage,SImage)
			.ColorAndOpacity(BackGroundColor)
			.Image(VideoBrush)
			]
		];
}

void SCXSMedia::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	// SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	if(MediaRunnable && MediaRunnable->bPlay && MediaRunnable->bVideoOpen)
	{
		switch(MediaRunnable->m_MediaType)
		{
		case EMedia::None:
			break;
		case EMedia::MediaCreate:
			{
				//v1.2
				if(MediaRunnable->Frames.size()>0)
				{
					Fps = VideoData.bAutoFps==true?MediaRunnable->Video_Fps:VideoData.CustomFps;
					MediaSize=FVector2D(MediaRunnable->Frames.front().cols,MediaRunnable->Frames.front().rows);
					if(MediaSize.X==0||MediaSize.Y==0)
					{
						MediaRunnable->Frames.pop();
						return;
					}
					MediaUpdateTextureRegion = new FUpdateTextureRegion2D(0,0,0,0,MediaSize.X,MediaSize.Y);
					int64 timestamp = (FDateTime::Now() - FDateTime(1970,1,1)).GetTotalMilliseconds();
					FString TimestampStr = FString::Printf(TEXT("Texture%lld"),timestamp);
					if(MediaTexture)
						MediaTexture->RemoveFromRoot();
					MediaTexture = UTexture2D::CreateTransient(MediaSize.X,MediaSize.Y,PF_B8G8R8A8,FName(*TimestampStr));
					void* TextureData = MediaTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
					FMemory::Memset(TextureData,0,MediaSize.X * MediaSize.Y * 4);
					MediaImage->SetColorAndOpacity(FLinearColor::White);
					MediaTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
					MediaTexture->AddToRoot();
					MediaTexture->UpdateResource();
					MediaRunnable->m_MediaType=EMedia::MediaRun;
				}
			}
			break;

		case EMedia::MediaRun:
			{
				switch (VideoData.RenderMode)
				{
				case ERenderMode::Memory:
					if(!MediaRunnable->Frames.empty())
					{
						UpdateTexture(MediaRunnable->Frames.front());
						MediaTexture->UpdateResource();
						VideoBrush->SetResourceObject(MediaTexture);
						MediaImage->SetImage(VideoBrush);
						MediaRunnable->Frames.pop();
					}
					break;
				case ERenderMode::RHI:
					if(MediaRunnable->MatRHIData.Num()>0)
					{
						MediaTexture->UpdateTextureRegions(0,1,MediaUpdateTextureRegion,MediaSize.X * 4, 4, (uint8*)MediaRunnable->MatRHIData.GetData());
						VideoBrush->SetResourceObject(MediaTexture);
						MediaImage->SetImage(VideoBrush);
					}
					break;
				}
			}
			break;
		case EMedia::MediaStop:
			break;
		}
	}
}

void SCXSMedia::PlayVideo(FString m_Url, FVideoData m_VideoData)
{
	if(m_Url.IsEmpty())
	{
		OnPlaying(false);
		return;
	}
	int64 timestamp = (FDateTime::Now() - FDateTime(1970,1,1)).GetTotalMilliseconds();
	FString TimestampStr = FString::Printf(TEXT("%lld"),timestamp);
	VideoData=m_VideoData;
	if(MediaRunnable)
	{
		switch(MediaRunnable->m_MediaType)
		{
		case EMedia::None:
			{
				MediaRunnable->Stop();
				if(MediaRunnable->PlayStop())
				{
					MediaRunnable=nullptr;
					MediaRunnable=new FCXSMediaRunnable(TimestampStr,m_Url,VideoData);
					MediaRunnable->OnFaceNumDelegate.BindRaw(this,&SCXSMedia::GetFaceNum);
					MediaRunnable->OnPlayingDelegate.BindRaw(this,&SCXSMedia::OnPlaying);
				}
			}
			break;
		case EMedia::MediaRun:
			{
				MediaRunnable->Stop();
				if(MediaRunnable->PlayStop())
				{
					MediaRunnable=nullptr;
					MediaRunnable=new FCXSMediaRunnable(TimestampStr,m_Url,VideoData);
					MediaRunnable->OnFaceNumDelegate.BindRaw(this,&SCXSMedia::GetFaceNum);
					MediaRunnable->OnPlayingDelegate.BindRaw(this,&SCXSMedia::OnPlaying);
				}
			}
			break;
		case EMedia::MediaStop:
			{
				MediaRunnable=nullptr;
				MediaRunnable=new FCXSMediaRunnable(TimestampStr,m_Url,VideoData);
				MediaRunnable->OnFaceNumDelegate.BindRaw(this,&SCXSMedia::GetFaceNum);
				MediaRunnable->OnPlayingDelegate.BindRaw(this,&SCXSMedia::OnPlaying);
			}
			break;
		}
	}
	else
	{
		MediaRunnable = new FCXSMediaRunnable(TimestampStr,m_Url,VideoData);
		MediaRunnable->OnFaceNumDelegate.BindRaw(this,&SCXSMedia::GetFaceNum);
		MediaRunnable->OnPlayingDelegate.BindRaw(this,&SCXSMedia::OnPlaying);
	}
}

void SCXSMedia::PlayPause(bool bPause)
{
	if(MediaRunnable)
	{
		MediaRunnable->PlayPause(bPause);
	}
}

void SCXSMedia::PlayStop()
{
	if(MediaRunnable)
	{
		MediaRunnable->PlayStop();
	}
}

void SCXSMedia::UpdateTexture(const cv::Mat& InMat)
{
	if(InMat.empty())
		return;
	int32 width = InMat.cols;
	int32 height = InMat.rows;
	cv::Mat Out;
	if(InMat.channels()==3)
		cv::cvtColor(InMat,Out,CV_RGB2RGBA);
	else if(InMat.channels()==4)
		Out = InMat;
	int DataSize = width * height * 4;
	void* Datas = MediaTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(Datas,(uint8*)Out.data,DataSize);
	MediaTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
	MediaTexture->UpdateResource();
}

float SCXSMedia::GetFps()
{
	if(MediaRunnable)
	{
		return MediaRunnable->GetFps();
	}
	return 0;
}

UTexture2D* SCXSMedia::GetTexture()
{
	if(MediaTexture)
	{
		return MediaTexture;
	}
	return nullptr;
}

void SCXSMedia::GetFaceNum(int FaceNum)
{
	if(OnFaceNumDelegate.IsBound())
	{
		OnFaceNumDelegate.Execute(FaceNum);
	}
}

void SCXSMedia::OnPlaying(bool bSuccess)
{
	if(OnPlayingDelegate.IsBound())
	{
		OnPlayingDelegate.Execute(bSuccess);
	}
}

//v1.3
bool SCXSMedia::PlayRecordVideo(const FString& FilePath)
{
	if(MediaRunnable)
	{
		return MediaRunnable->PlayRecordVideo(FilePath);
	}
	return false;
}

void SCXSMedia::StopRecordVideo()
{
	if(MediaRunnable)
	{
		MediaRunnable->StopRecordVideo();
	}
}

FCXSMediaRunnable::FCXSMediaRunnable(const FString& ThreadName, const FString Url, FVideoData VideoData)
{
	m_Url = Url;
	M_RenderMode = VideoData.RenderMode;
	m_bAutoFps = VideoData.bAutoFps;
	m_CustomFps = VideoData.CustomFps;
	m_bEnableKey = VideoData.bEnableKey;
	m_Scale = VideoData.RectScale;
	m_DrawType = VideoData.DrawType;
	m_Color = CV_RGB(VideoData.Color.R,VideoData.Color.G,VideoData.Color.B);
	m_HaarXml = VideoData.HaarXml;
	m_TimeOut = VideoData.TimeOut;
	ThreadEvent = FPlatformProcess::GetSynchEventFromPool();
	m_ThreadName = ThreadName;
	ThreadIns = FRunnableThread::Create(this,*m_ThreadName,0,TPri_Normal);
	m_ThreadID = ThreadIns->GetThreadID();

	// FLinearColor TempHsvOne = VideoData.ChromaKeyThresholdOne.LinearRGBToHSV();
	// FLinearColor TempHsvTwo = VideoData.ChromaKeyThresholdTwo.LinearRGBToHSV();
	// m_HsvMin = cv::Scalar(FMath::Min(TempHsvOne.R,TempHsvTwo.R),FMath::Min(TempHsvOne.G,TempHsvTwo.G),FMath::Min(TempHsvOne.B,TempHsvTwo.B));
	// m_HsvMax = cv::Scalar(FMath::Max(TempHsvOne.R,TempHsvTwo.R),FMath::Max(TempHsvOne.G,TempHsvTwo.G),FMath::Max(TempHsvOne.B,TempHsvTwo.B));

	m_HsvMin = cv::Scalar(35,43,46);
	m_HsvMax = cv::Scalar(77,255,255);
	
	// UE_LOG(LogTemp,Warning,TEXT("HsvMin:(%f,%f,%f)"),m_HsvMin.val[0],m_HsvMin.val[1],m_HsvMin.val[2]);
	// UE_LOG(LogTemp,Warning,TEXT("HsvMax:(%f,%f,%f)"),m_HsvMax.val[0],m_HsvMax.val[1],m_HsvMax.val[2]);
	
	FString HaarName = TEXT("haarcascade_frontalface_alt2.xml");
	switch (m_HaarXml)
	{
	case EHaarXml::Haarcascade_frontalface_alt2:
		HaarName = TEXT("haarcascade_frontalface_alt2.xml");
		break;
	case EHaarXml::haarcascade_fullbody:
		HaarName = TEXT("haarcascade_fullbody.xml");
		break;
	case EHaarXml::haarcascade_lowerbody:
		HaarName = TEXT("haarcascade_lowerbody.xml");
		break;
	case EHaarXml::haarcascade_upperbody:
		HaarName = TEXT("haarcascade_upperbody.xml");
		break;
	}
	FString FaceXmlDir =FPaths::ConvertRelativePathToFull(FPaths::Combine( FPaths::ProjectDir(),TEXT("Binaries/Win64/ThirdParty/OpenCV/xml/"),HaarName));
	FaceCascade.load(TCHAR_TO_UTF8(*FaceXmlDir));
}

FCXSMediaRunnable::~FCXSMediaRunnable()
{
	
}

bool FCXSMediaRunnable::Init()
{
	return FRunnable::Init();
}

uint32 FCXSMediaRunnable::Run()
{
	FPlatformProcess::Sleep(0.01f);
	bPlay=InitVideo();
	FPlatformProcess::Sleep(0.02f);
	while (bRun)
	{
		if(bPlay && !bPause)
		{
			if(bVideoOpen)
			{
				UpdateFrame();
				if(M_RenderMode==ERenderMode::RHI)
				{
					UpdateRHIData();
				}
				FPlatformProcess::Sleep(Video_Fps);
			}
		}
	}
	return 0;
}

void FCXSMediaRunnable::Stop()
{
	bRun = false;
}

void FCXSMediaRunnable::Exit()
{
	bRun = false;
	bPlay = false;
}

void FCXSMediaRunnable::PlayPause(bool bPlayPause)
{
	bPause = bPlayPause;
	if(!this->bPause)
	{
		if(ThreadEvent)
		{
			ThreadEvent->Trigger();
		}
	}
}

bool FCXSMediaRunnable::PlayStop()
{
	m_MediaType = EMedia::MediaStop;
	bVideoOpen = false;
	bRun = false;
	std::queue<cv::Mat>().swap(Frames);
	if(ThreadEvent)
	{
		FPlatformProcess::ReturnSynchEventToPool(ThreadEvent);
		ThreadEvent = nullptr;
	}
	if(ThreadIns)
	{
		ThreadIns->WaitForCompletion();
		bool bKilled = ThreadIns->Kill(true);
		delete ThreadIns;
		ThreadIns=nullptr;
		VideoCapture.release();
		Mat.release();
		FrameRHI.release();
		Gray.release();
		SmallMat.release();
		MatRHIData.Empty();
		StopRecordVideo();
		return bKilled;
	}
	return false;
}

bool FCXSMediaRunnable::InitVideo()
{
	if (!m_Url.IsEmpty())
	{
		//if (m_Url.Contains(TEXT("://")))
		{
			std::vector<int> params;
			params.push_back(cv::CAP_PROP_OPEN_TIMEOUT_MSEC);
			params.push_back(m_TimeOut);
			VideoCapture.open(TCHAR_TO_UTF8(*m_Url),cv::CAP_FFMPEG,params);
		}
		if (VideoCapture.isOpened())
		{
			UpdateFrame();
			m_Size=FVector2D(Frames.front().cols,Frames.front().rows);
			MatRHIData.Init(FColor(0,0,0,255),m_Size.X*m_Size.Y);
			if (m_bAutoFps) 
			{
				VideoFps=VideoCapture.get(cv::CAP_PROP_FPS);
				Video_Fps=1/VideoCapture.get(cv::CAP_PROP_FPS);
			}
			else
			{
				VideoFps=m_CustomFps;
				Video_Fps=1/m_CustomFps;
			}
			
			bRun=true;
			m_MediaType=EMedia::MediaCreate;
			bVideoOpen=true;
			if (OnPlayingDelegate.IsBound())
			{
				OnPlayingDelegate.Execute(true);
			}
			//PlayRecordVideo();
			return true;
		}
		else
		{
			bRun=false;
			m_MediaType=EMedia::None;
			bVideoOpen=false;
			if (OnPlayingDelegate.IsBound())
			{
				OnPlayingDelegate.Execute(false);
			}
			return false;
		}
	}
	else
	{
		bRun=false;
		m_MediaType=EMedia::None;
		bVideoOpen=false;
		if (OnPlayingDelegate.IsBound())
		{
			OnPlayingDelegate.Execute(false);
		}
		return false;
	}
}

void FCXSMediaRunnable::UpdateFrame()
{
	if(VideoCapture.isOpened())
	{
		VideoCapture.read(Mat);
		if(m_bEnableKey)
		{
			Frames.push(ChromaKey(Mat));
			if(bRecordVideo && m_Video.isOpened())
			{
				m_Video.write(Mat);
			}
		}
		else
		{
			Frames.push(Mat);
			if(bRecordVideo && m_Video.isOpened())
			{
				m_Video.write(Mat);
			}
		}
	}
	else
	{
		bVideoOpen = false;
	}
}

void FCXSMediaRunnable::UpdateRHIData()
{
	if(!Frames.empty())
	{
		FrameRHI = Frames.front();
		if(bVideoOpen && FrameRHI.data && Frames.size() > 0)
		{
			for(int x = 0; x < m_Size.X; x++)
			{
				for(int y = 0; y < m_Size.Y; y++)
				{
					int i = x + (y * m_Size.X);
					if(Frames.size() > 0)
					{
						MatRHIData[i] = FColor(FrameRHI.data[i * 3 + 2],FrameRHI.data[i * 3 + 1],FrameRHI.data[i * 3 + 0],255);
					}
				}
			}
			if(Frames.size() > 0)
			{
				Frames.pop();
			}
		}
	}
}

float FCXSMediaRunnable::GetFps()
{
	if(VideoCapture.isOpened())
	{
		return VideoCapture.get(cv::CAP_PROP_FPS);
	}
	return 0;
}

void FCXSMediaRunnable::FaceDetection(const cv::Mat InMat)
{
	if(InMat.empty())
		return;
	cv::cvtColor(InMat,Gray,CV_RGB2GRAY);
	if(m_Scale==1)
	{
		FaceCascade.detectMultiScale(Gray,Rect,1.1,3,0);
	}
	else
	{
		double fx=(double)1/m_Scale;
		cv::resize( Gray, SmallMat, cv::Size(), fx, fx, cv::INTER_LINEAR_EXACT );
		cv::equalizeHist( SmallMat, SmallMat );
		FaceCascade.detectMultiScale(SmallMat, Rect, 1.1, 3,0/*,cv::Size(30, 30)*/);
		//FaceCascade.detectMultiScale(SmallMat, Rect, 1.1, 10,0,cv::Size(15, 15));
	}
#pragma omp parallel for
	for (int i = 0; i < Rect.size();i++)
	{
		switch (m_DrawType)
		{
		case EDrawType::Rectangle:
			{
				cv::Rect RectFace = Rect[i];
				cv::rectangle(	InMat, cv::Point(RectFace.x, RectFace.y) * m_Scale,
					cv::Point(RectFace.x + RectFace.width, RectFace.y + RectFace.height) * m_Scale,m_Color,
					2, 8);
			}
			break;
		case EDrawType::Circle:
			{
				cv::Point  Center;
				int Radius;
				Center.x = cvRound((Rect[i].x* m_Scale + Rect[i].width * 0.5* m_Scale));
				Center.y = cvRound((Rect[i].y* m_Scale + Rect[i].height * 0.5* m_Scale));
				Radius = cvRound((Rect[i].width + Rect[i].height) * 0.25* m_Scale);
				cv::circle(InMat, Center, Radius,m_Color, 2);
			}
			break;
		} 
	}
	if (Rect.size()>0)
	{
		int Num=static_cast<int>(Rect.size());
		if (OnFaceNumDelegate.IsBound())
		{
			OnFaceNumDelegate.Execute(Num);
		}
	}
}

cv::Mat FCXSMediaRunnable::ChromaKey(const cv::Mat InMat)
{
	cv::Mat ResMat;
	if(InMat.empty())
		return ResMat;
	cv::cvtColor(InMat,ResMat,cv::COLOR_RGB2RGBA);
	cv::Mat HSVSrc;
	cv::Mat HSVMask;
	cv::cvtColor(InMat,HSVSrc,cv::COLOR_RGB2HSV);
	cv::inRange(HSVSrc, m_HsvMin, m_HsvMax, HSVMask);

	// cv::Mat TempElement = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3),cv::Point(-1,-1));
	// cv::morphologyEx(HSVMask,HSVMask,cv::MORPH_CLOSE,TempElement);
	// cv::erode(HSVMask,HSVMask,TempElement);
	cv::GaussianBlur(HSVMask,HSVMask,cv::Size(3,3),0,0);
	
	int32 MatSize = InMat.cols*InMat.rows;
	ParallelFor(MatSize,[&](int32 Idx)
	{
		if(ResMat.channels()==4)
		{
			ResMat.at<cv::Vec4b>(Idx/InMat.cols,Idx%InMat.cols).val[3] = FMath::Clamp(255-HSVMask.at<uchar>(Idx/InMat.cols,Idx%InMat.cols),0,255);
		}
	});
	
	// for(int32 IdxRow=0;IdxRow<InMat.rows;++IdxRow)
	// {
	// 	for(int32 IdxCol=0;IdxCol<InMat.cols;++IdxCol)
	// 	{
	// 		int32 MaskPointValue = HSVMask.at<uchar>(IdxRow,IdxCol);
	// 		if(ResMat.channels()==4)
	// 			ResMat.at<cv::Vec4b>(IdxRow,IdxCol).val[3] = FMath::Clamp(255-MaskPointValue,0,255);
	// 	}
	// }
	
	return ResMat;
}

bool FCXSMediaRunnable::PlayRecordVideo(const FString& FilePath)
{
	bRecordVideo=true;
	if (bRecordVideo)
	{
		if (!FilePath.IsEmpty())
		{
			m_VideoName=TCHAR_TO_UTF8(*FilePath);
			FString Extension = FPaths::GetExtension(FilePath);
			int fourcc=cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
			Extension=Extension.ToLower();
			if (Extension.Equals("avi",ESearchCase::IgnoreCase))
			{
				fourcc=cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
			}
			else if(Extension.Equals("mp4",ESearchCase::IgnoreCase))
			{
				fourcc=-1;
			}
			else
			{
				bRecordVideo=false;
				return false;
			}
			return m_Video.open(m_VideoName,fourcc,VideoFps,cv::Size(m_Size.X,m_Size.Y));
		}
		bRecordVideo=false;
		return false;
	}
	bRecordVideo=false;
	return false;
}

void FCXSMediaRunnable::StopRecordVideo()
{
	if (bRecordVideo)
	{
		bRecordVideo=false;
		FPlatformProcess::Sleep(0.1f);
		m_Video.release();
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION