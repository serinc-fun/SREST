// Fill out your copyright notice in the Description page of Project Settings.

#include "SBaseRequestsHandler.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"

USBaseRequestsHandler::USBaseRequestsHandler()
{
	
}

void USBaseRequestsHandler::Setup(USRequestsProcessor* InRequestProcessor)
{
	if (RequestProcessor != InRequestProcessor)
	{
		RequestProcessor = InRequestProcessor;
		OnSetup_Implementation(RequestProcessor);
	}
}

void USBaseRequestsHandler::CreateSimpleRequest(const FString& InUrl, const FString& InVerb,
	const TMap<FString, FString>& InHeader, const FString& InBody)
{
	FHttpModule& HttpModule = FHttpModule::Get();
	const TSharedRef<IHttpRequest> LRequest = HttpModule.CreateRequest();
	
	LRequest->SetURL(InUrl);
	LRequest->SetVerb(InVerb);
	LRequest->SetContentAsString(InBody);

	TArray<FString> LKeys;
	InHeader.GetKeys(LKeys);

	for (auto LKey : LKeys)
	{
		LRequest->SetHeader(LKey, InHeader[LKey]);
	}
	
	//InHeader.AssignHeadersToRequest(LRequest);

	LRequest->OnProcessRequestComplete().BindUObject(this, &ThisClass::ProcessComplete);

	
	LRequest->ProcessRequest();
}

void USBaseRequestsHandler::ProcessComplete(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bInSuccessful)
{
	OnRequestComplete.Broadcast(InResponse->GetContentAsString(), bInSuccessful);
}


void USBaseRequestsHandler::OnSetup_Implementation(USRequestsProcessor* InRequestProcessor)
{
	
}
