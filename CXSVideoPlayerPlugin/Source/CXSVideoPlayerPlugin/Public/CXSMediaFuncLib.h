// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CXSMediaFuncLib.generated.h"

/**
 * 
 */
UCLASS()
class CXSVIDEOPLAYERPLUGIN_API UCXSMediaFuncLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable,Category="CXSMedia | Get Media File Paths")
	static TArray<FString> OpenMediaFile();

};
