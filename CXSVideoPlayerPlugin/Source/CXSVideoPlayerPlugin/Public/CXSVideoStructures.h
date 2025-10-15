// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetMathLibrary.h"
#include "CXSVideoStructures.generated.h"

UENUM(BlueprintType)
enum class EMedia : uint8
{
	None = 0,
	MediaCreate	UMETA(DisplayName = "Create"),
	MediaRun	UMETA(DisplayName = "Run"),
	MediaStop	UMETA(DisplayName = "Stop"),
	Max
};

UENUM(BlueprintType)
enum class ERenderMode : uint8
{
	Memory	UMETA(DisplayName = "Memory"),
	RHI		UMETA(DisplayName = "RHI"),
	Max
};

UENUM(BlueprintType)
enum class EDrawType : uint8
{
	Rectangle	UMETA(DisplayName = "Rectangle"),
	Circle		UMETA(DisplayName = "Circle"),
	Max
};

UENUM(BlueprintType)
enum class EHaarXml : uint8
{
	Haarcascade_frontalface_alt2	UMETA(DisplayName = "Face"),
	haarcascade_fullbody			UMETA(DisplayName = "Fullbody"),
	haarcascade_lowerbody			UMETA(DisplayName = "Lowerbody"),
	haarcascade_upperbody			UMETA(DisplayName = "Upperbody"),
	Max
};

USTRUCT(BlueprintType)
struct FVideoData
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category="VideoData")
	ERenderMode RenderMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="VideoData")
	bool bAutoFps;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="VideoData")
	float CustomFps;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="VideoData")
	bool bEnableKey;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="VideoData")
	// FLinearColor ChromaKeyThresholdOne;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="VideoData")
	// FLinearColor ChromaKeyThresholdTwo;
	UPROPERTY(VisibleDefaultsOnly,Category="VideoData")
	int RectScale;
	UPROPERTY(VisibleDefaultsOnly,Category="VideoData")
	EDrawType DrawType;
	UPROPERTY(VisibleDefaultsOnly,Category="VideoData")
	FColor Color;
	UPROPERTY(VisibleDefaultsOnly,Category="VideoData")
	EHaarXml HaarXml;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="VideoData")
	int TimeOut;

	FVideoData():
	RenderMode(ERenderMode::Memory),
	bAutoFps(true),
	CustomFps(30),
	bEnableKey(false),
	// ChromaKeyThresholdOne(FLinearColor::MakeFromHSV8(35,43,46)),
	// ChromaKeyThresholdTwo(FLinearColor::MakeFromHSV8(77,255,255)),
	RectScale(4),
	DrawType(EDrawType::Rectangle),
	Color(FColor::Red),
	HaarXml(EHaarXml::haarcascade_fullbody),
	TimeOut(3000){}
};