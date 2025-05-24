// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBackendFlatStateUpdateSettings.generated.h"

/**
 * 
 */
UCLASS(Config = SREST, defaultconfig)
class SRESTEDITOR_API USBackendFlatStateUpdateSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	
	
public:
	
	UFUNCTION(BlueprintPure, DisplayName="GetBackendFlatStateUpdateSettings")
	static USBackendFlatStateUpdateSettings* GetBackendFlatStateUpdateSettings()
	{
		return GetMutableDefault<USBackendFlatStateUpdateSettings>();
	}
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Config, Category = "Handler")
	FString EndPointBase = "https://admin.talent.ttt-ne.su/api/";
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Config, Category = "Handler")
	FString UploadMethod = "construction/projects/import";
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Config, Category = "Handler")
	FName RequestIDName = FName("BackendFlatStateUpdate");
	
};
