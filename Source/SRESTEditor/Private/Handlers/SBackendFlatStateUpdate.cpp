// Fill out your copyright notice in the Description page of Project Settings.


#include ".//Handlers/SBackendFlatStateUpdate.h"

#include "SRequestsProcessor.h"
#include "Async/TUFuture.h"
#include "Async/Letters/TCFutureLetter.h"
#include "TalentCore/API/TCAPITypes.h"
#include "TalentCore/Utilities/TCBaseConfig.h"
#include "Utilities/Handlers/TCBackendFlatStateUpdateConfig.h"

void USBackendFlatStateUpdate::OnSetup_Implementation(USRequestsProcessor* InRequestProcessor)
{
	Super::OnSetup_Implementation(InRequestProcessor);
	
	const auto LConfig = UTCBaseConfig::GetMutableConfig<UTCBackendFlatStateUpdateConfig>();

	if (IsValid(LConfig))
	{
		CustomEndpoint = LConfig->UploadMethod;
		IDName = LConfig->RequestIDName;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%hs() failed - no configuration found"), __func__);
		CustomEndpoint = "construction/projects/import";
		IDName = FName("BackendFlatStateUpdate");
	}
}

UTUFuture* USBackendFlatStateUpdate::UploadToBackend(FTCEditorProjectData InData)
{
	Promise = NewObject<UTUPromise>();
	Promise->AddToRoot();
	
	USRequestsProcessor* LRequestProcessor = GetProcessor();
	
	if (IsValid(LRequestProcessor))
	{
		FSRequestRef LRequest = GetProcessor()->CreateRequest(CustomEndpoint, ESRequestType::VERB_POST);

		LRequest->BindStringCallback(200).AddUObject(this, &USBackendFlatStateUpdate::StringReturnTEST);
		LRequest->BindErrorCallback().AddUObject(this, &USBackendFlatStateUpdate::ErrorStringReturnTEST);
		
		LRequest->Send(InData, IDName);
		return Promise->GetFuture();
	}


	
	return Promise->GetFuture();
}

void USBackendFlatStateUpdate::StringReturnTEST(const FString& InString)
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

void USBackendFlatStateUpdate::ErrorStringReturnTEST(const int32& InCode, const FString& InString)
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