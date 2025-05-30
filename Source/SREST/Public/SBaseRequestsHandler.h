// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SBaseRequestsHandler.generated.h"

class USRequestsProcessor;
/**
 * 
 */
UCLASS(Abstract)
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
	
	UPROPERTY(EditDefaultsOnly, Category = Configuration)
	FString CustomEndpoint;

	UFUNCTION(BlueprintPure)
	FORCEINLINE USRequestsProcessor* GetProcessor() const { return RequestProcessor; }

private:

	UPROPERTY(Transient)
	USRequestsProcessor* RequestProcessor;
};
