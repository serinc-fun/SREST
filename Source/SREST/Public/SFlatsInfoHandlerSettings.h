// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SFlatsInfoHandlerSettings.generated.h"

/**
 * 
 */
UCLASS(Config = SREST, defaultconfig)
class SREST_API USFlatsInfoHandlerSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	public:

	
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName="GetFlatsInfoHandlerSettings")
	static USFlatsInfoHandlerSettings* GetUSFlatsInfoHandlerSettings()
	{
		return GetMutableDefault<USFlatsInfoHandlerSettings>();
	}
	
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Config, Category = "Handler")
	FString SearchMethod = "construction/apartments/search/list";

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Config, Category = "Handler")
	FString BuyRequestMethod = "construction/apartments/request-view";

};
