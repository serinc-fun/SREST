// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SSettings.generated.h"

/**
 * 
 */
UCLASS(DefaultConfig, config = SREST)
class SREST_API USSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	
	virtual FName GetSectionName() const override;
	virtual FName GetCategoryName() const override;
	virtual FName GetContainerName() const override;

#if WITH_EDITOR
	virtual FText GetSectionText() const override;
	virtual FText GetSectionDescription() const override;
#endif
	
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName="GetSRESTSettings")
	static USSettings* GetSettings();
	
	int32 GetContentLenghtForDisplayCompleted() const { return ContentLenghtForDisplayCompleted; }
	int32 GetContentLenghtForDisplaySend() const { return ContentLenghtForDisplaySending; }
	
protected:
	/**
	 * Content length for display. Only for completed request
	 */
	UPROPERTY(EditAnywhere, Config, Category = "Logs")
	int32 ContentLenghtForDisplayCompleted = 2048;

	/**
	 * Content length for display. Only for sending request
	 */
	UPROPERTY(EditAnywhere, Config, Category = "Logs")
	int32 ContentLenghtForDisplaySending = 4192;
	
};
