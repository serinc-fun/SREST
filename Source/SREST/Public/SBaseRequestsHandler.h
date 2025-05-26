// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
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

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRequestComplete, const FString&, OutResponce, bool&, OutSuccess);
	DECLARE_DELEGATE_TwoParams(FOnRequestCompleteNative, const FString&, bool&)

	UPROPERTY(BlueprintAssignable)
	FOnRequestComplete OnRequestComplete;
	
	FOnRequestCompleteNative OnRequestCompleteNative;
	
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

	UFUNCTION(BlueprintCallable)
	void CreateSimpleRequest(const FString& InUrl, const FString& InVerb, const TMap<FString, FString>& InHeader, const FString& InBody);

	virtual void ProcessComplete(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bInSuccessful);

private:

	
	UPROPERTY(Transient)
	USRequestsProcessor* RequestProcessor;
};
