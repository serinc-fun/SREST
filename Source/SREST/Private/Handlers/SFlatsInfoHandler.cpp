// Fill out your copyright notice in the Description page of Project Settings.


#include "Handlers/SFlatsInfoHandler.h"

#include "SRequestsProcessor.h"
#include "SRequest.h"
#include "Async/TUFuture.h"
#include "Async/Letters/TCFutureLetter.h"

UTUFuture* USFlatsInfoHandler::GetFlatsInfo(const FString& InID, const int& InBuildingNumber, const int& InEntranceNumber, const int& InFloorNumber)
{
	Promise = NewObject<UTUPromise>();
	Promise->AddToRoot();

	USRequestsProcessor* LRequestProcessor = GetProcessor();
	
	if (IsValid(LRequestProcessor))
	{
		FSRequestRef LRequest = GetProcessor()->CreateRequest(CustomEndpoint, ESRequestType::VERB_GET);

		LRequest->BindStringCallback(200).AddUObject(this, &USFlatsInfoHandler::ReturnFlatsInfo);
		LRequest->BindErrorCallback().AddUObject(this, &USFlatsInfoHandler::ErrorStringReturn);
		
		FString LData = "?ProjectID=" + InID + "&BuildingNumber=" + FString::FromInt(InBuildingNumber) + "&EntranceNumber=" + FString::FromInt(InEntranceNumber)
		+ "&FloorNumber=" + FString::FromInt(InFloorNumber) + "&Paging.Offset=0&Paging.Take=99";
		
		LRequest->Send(LData, IDName);
		return Promise->GetFuture();
	}
	
	return Promise->GetFuture();
}


void USFlatsInfoHandler::ReturnFlatsInfo(const FString& InString) const
{
	if (IsValid(Promise))
	{
		UTUFutureLetter* LLetter = NewObject<UTUFutureLetter>();
		LLetter->Code = 200;
		LLetter->Message = InString;

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
		UTUFutureLetter* LLetter = NewObject<UTUFutureLetter>();
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
