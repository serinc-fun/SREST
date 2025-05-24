// Fill out your copyright notice in the Description page of Project Settings.


#include ".//Handlers/SBackendFlatStateUpdateHandler.h"

#include "SRequestsProcessor.h"
#include "Async/TUFuture.h"
#include "Async/Letters/TCFutureLetter.h"
#include "TalentCore/API/TCAPITypes.h"
#include "TalentCore/Utilities/TCBaseConfig.h"
#include "SBackendFlatStateUpdateSettings.h"

void USBackendFlatStateUpdateHandler::OnSetup_Implementation(USRequestsProcessor* InRequestProcessor)
{
	Super::OnSetup_Implementation(InRequestProcessor);
	
	const auto LConfig = UTCBaseConfig::GetMutableConfig<USBackendFlatStateUpdateSettings>();

	if (IsValid(LConfig))
	{
		CustomEndpoint = LConfig->EndPointBase + LConfig->UploadMethod;
		IDName = LConfig->RequestIDName;
	}
}

UTUFuture* USBackendFlatStateUpdateHandler::UploadToBackend(FTCEditorProjectData InData)
{
	Promise = NewObject<UTUPromise>();
	Promise->AddToRoot();
	
	USRequestsProcessor* LRequestProcessor = GetProcessor();
	
	if (IsValid(LRequestProcessor))
	{
		FSRequestRef LRequest = GetProcessor()->CreateRequest(CustomEndpoint, ESRequestType::VERB_POST);

		LRequest->BindStringCallback(200).AddUObject(this, &USBackendFlatStateUpdateHandler::StringReturnTEST);
		LRequest->BindErrorCallback().AddUObject(this, &USBackendFlatStateUpdateHandler::ErrorStringReturnTEST);
		
		LRequest->Send(InData, IDName);
	}


	
	return Promise->GetFuture();
}

void USBackendFlatStateUpdateHandler::StringReturnTEST(const FString& InString)
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

void USBackendFlatStateUpdateHandler::ErrorStringReturnTEST(const int32& InCode, const FString& InString)
{
	if (IsValid(Promise))
	{
		UTUFutureLetter* LLetter = NewObject<UTUFutureLetter>();
		LLetter->Code = InCode;
		LLetter->Message = InString;

		UE_LOG(LogTemp, Error, TEXT("USBackendFlatStateUpdateHandler : %s"), *InString);

		Promise->Resolve(LLetter);
		Promise->RemoveFromRoot();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%hs() failed - no promise"), __func__);
	}
}