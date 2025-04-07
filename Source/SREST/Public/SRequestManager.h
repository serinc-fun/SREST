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
	FName Id;
	FSRequestPtr RequestPtr;
	FHttpRequestPtr SystemRequestPtr;
	bool IsCompleted = false;
	int32 Code = INDEX_NONE;

	bool operator==(const FSProcessingRequest& InR) const
	{
		return InR.Id == Id && InR.RequestPtr == RequestPtr && InR.SystemRequestPtr == SystemRequestPtr;
	}
};

UCLASS()
class SREST_API USRequestManager : public UObject
{
	GENERATED_BODY()

public:

	USRequestManager();

	FSRequestRef CreateRequest(const FString& InMethod, const ESRequestType& InType = ESRequestType::VERB_GET);
	FSRequestRef CreateRequest(UObject* InOwner, const FString& InMethod, const ESRequestType& InType = ESRequestType::VERB_GET, const ESRequestContentType& InContentType = ESRequestContentType::Json);
	bool SendRequest(const FSRequestRef& InRequest, const FString& InContent = "", const FName& InId = NAME_None);
	void CancelRequest(const FSRequestRef& InRequest, const FName& InId);
	
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

	void OnRequestCode(FHttpRequestPtr InRequest, int32 InStatusCode);
	void OnRequestHeader(FHttpRequestPtr InRequest, const FString& InHeaderName, const FString& InHeaderValue);
	void OnRequestProgress(FHttpRequestPtr InRequest, uint64 InBytesSent, uint64 InBytesReceived);
	void OnRequestCompleted(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bConnectedSuccessfully);

	TSharedPtr<FSProcessingRequest> GetRequestDataBySystemRequest(FHttpRequestRef InSystemRequest) const;
	
	TArray<FSRequestPtr> Requests;
	TArray<TSharedPtr<FSProcessingRequest>> ProcessingRequests;
};






