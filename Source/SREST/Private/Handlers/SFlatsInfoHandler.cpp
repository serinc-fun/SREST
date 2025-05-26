// Fill out your copyright notice in the Description page of Project Settings.


#include "Handlers/SFlatsInfoHandler.h"

#include "SFlatsInfoHandlerSettings.h"
#include "SRequestsProcessor.h"
#include "SRequest.h"
#include "Async/TUFuture.h"
#include "Async/Letters/TUFlatStateFutureLetter.h"
#include "Interfaces/IHttpResponse.h"

UTUFuture* USFlatsInfoHandler::GetFlatsInfo(const FString& InID, const int& InBuildingNumber, const int& InEntranceNumber, const int& InFloorNumber)
{
	Promise = NewObject<UTUPromise>();
	Promise->AddToRoot();

	//USRequestsProcessor* LRequestProcessor = GetProcessor();
	
	//if (IsValid(LRequestProcessor))
	//{
		//FSRequestRef LRequest = GetProcessor()->CreateRequest(CustomEndpoint, ESRequestType::VERB_GET);

		//LRequest->BindUStructCallback<FFlatsInfoEntries>(200).AddUObject(this, &USFlatsInfoHandler::ReturnFlatsInfoStruct);
		//LRequest->BindErrorCallback().AddUObject(this, &USFlatsInfoHandler::ErrorStringReturn);

		USFlatsInfoHandlerSettings* LConfig = USFlatsInfoHandlerSettings::GetUSFlatsInfoHandlerSettings();
		USSettings* LSRESTConfig = USSettings::GetSettings();
	
		FString LData = "?ProjectID=" + InID + "&BuildingNumber=" + FString::FromInt(InBuildingNumber) + "&EntranceNumber=" + FString::FromInt(InEntranceNumber)
		+ "&FloorNumber=" + FString::FromInt(InFloorNumber) + "&Paging.Offset=0&Paging.Take=99";
		
		CreateSimpleRequest(LSRESTConfig->EndPointBase +  LConfig->SearchMethod + LData, "GET", TMap<FString, FString>(), "");
		//LRequest->Send(LData, IDName);
		return Promise->GetFuture();
	//}
	
	//return Promise->GetFuture();
}

void USFlatsInfoHandler::ProcessComplete(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bInSuccessful)
{
	if (IsValid(Promise))
	{
		FFlatsInfoEntries LStruct;
		if (FJsonObjectConverter::JsonObjectStringToUStruct(InResponse->GetContentAsString(), &LStruct))
		{
			UTUFlatStateFutureLetter* LLetter = NewObject<UTUFlatStateFutureLetter>();
			LLetter->Code = InResponse->GetResponseCode();
			LLetter->Info = LStruct;

			Promise->Resolve(LLetter);
			Promise->RemoveFromRoot();
			Super::ProcessComplete(InRequest, InResponse, bInSuccessful);
			return;
		}
		else
		{
			UTUFlatStateFutureLetter* LLetter = NewObject<UTUFlatStateFutureLetter>();
			LLetter->Code = 400;
			LLetter->Message = "Error Convert, but code is " + FString::FromInt( InResponse->GetResponseCode());

			Promise->Resolve(LLetter);
			Promise->RemoveFromRoot();
			Super::ProcessComplete(InRequest, InResponse, false);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%hs() failed - no promise"), __func__);
	}
	
	Super::ProcessComplete(InRequest, InResponse, bInSuccessful);
}

//////////////
void USFlatsInfoHandler::ReturnFlatsInfoStruct(const FFlatsInfoEntries& InInfo) const
{
	if (IsValid(Promise))
	{
		UTUFlatStateFutureLetter* LLetter = NewObject<UTUFlatStateFutureLetter>();
		LLetter->Code = 200;
		LLetter->Info = InInfo;

		Promise->Resolve(LLetter);
		Promise->RemoveFromRoot();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%hs() failed - no promise"), __func__);
	}
}

void USFlatsInfoHandler::ErrorStringReturn(const int32& InCode, const FString& InString) const
{
	if (IsValid(Promise))
	{
		UTUFlatStateFutureLetter* LLetter = NewObject<UTUFlatStateFutureLetter>();
		LLetter->Code = InCode;
		LLetter->Message = InString;

		UE_LOG(LogTemp, Error, TEXT("USBackendFlatStateUpdate : %s"), *InString);

		Promise->Resolve(LLetter);
		Promise->RemoveFromRoot();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%hs() failed - no promise"), __func__);
	}
}
//////////////