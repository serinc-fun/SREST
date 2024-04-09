// Copyright Serinc All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "SRequest.h"
#include "UObject/Object.h"
#include "Interfaces/IHttpRequest.h"
#include "SRestTokenInterface.h"
#include "SRequestManager.generated.h"

struct SREST_API FSProcessingRequest
{
	FDeRequestPtr DeRequest;
	FHttpRequestPtr Request;

	bool operator==(const FSProcessingRequest& InR) const
	{
		return InR.DeRequest == DeRequest && InR.Request == Request;
	}
};

UCLASS()
class SREST_API USRequestManager : public UObject
{
	GENERATED_BODY()

public:

	USRequestManager();

	FDeRequestRef CreateRequest(UObject* InOwner, const FString& InMethod, const ESRequestType& InType = ESRequestType::VGET);
	bool SendRequest(const FDeRequestRef& InRequest, const FString& InContent = "");

	UFUNCTION(BlueprintCallable)
	void SetEndpoint(const FString& InEndpoint);
	
	UFUNCTION(BlueprintPure)
	const FString& GetEndpoint() const;

	UFUNCTION(BlueprintCallable)
	void SetTokenHeaderName(const FString& InName);
	
	UFUNCTION(BlueprintPure)
	const FString& GetTokenHeaderName() const;

	UFUNCTION(BlueprintCallable)
	void SetTokenHeaderValue(const FString& InValue);
	
	UFUNCTION(BlueprintPure)
	const FString& GetTokenHeaderValue() const;
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Endpoint;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString TokenName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString TokenValue;
	
	void OnRequestCompleted(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bConnectedSuccessfully);

	TArray<FDeRequestPtr> Requests;
	TArray<FSProcessingRequest> ProcessingRequests;
};






