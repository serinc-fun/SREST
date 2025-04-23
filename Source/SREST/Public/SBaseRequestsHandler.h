// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SBaseRequestsHandler.generated.h"

class USRequestsProcessor;
/**
 * 
 */
UCLASS()
class SREST_API USBaseRequestsHandler : public UObject
{
	friend class USRequestsSubsystem;
	
	GENERATED_BODY()

public:

	USBaseRequestsHandler();

private:
	
	void Setup(USRequestsProcessor* InRequestProcessor);

protected:
	
	UFUNCTION(BlueprintNativeEvent)
	void OnSetup(USRequestsProcessor* InRequestProcessor);
	
	UPROPERTY(Transient)
	USRequestsProcessor* RequestProcessor;
	
	UPROPERTY(EditDefaultsOnly, Category = Configuration)
	FString CustomEndpoint;
};
