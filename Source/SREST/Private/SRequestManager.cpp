// Copyright Serinc All Rights Reserved.
#include "SRequestManager.h"

#include "SHandler.h"
#include "SRestTokenInterface.h"
#include "Http.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"

USRequestManager::USRequestManager()
{
	
}

FSRequestRef USRequestManager::CreateRequest(const FString& InMethod, const ESRequestType& InType)
{
	return CreateRequest(GetOuter(), InMethod, InType);
}

FSRequestRef USRequestManager::CreateRequest(UObject* InOwner, const FString& InMethod, const ESRequestType& InType)
{
	FSRequestRef LRequest = MakeShareable(new FSRequest(InOwner));
	LRequest->Type = InType;
	LRequest->Method = InMethod;
	LRequest->ContentType = ESRequestContentType::Json;
	LRequest->Manager = this;
	Requests.Add(LRequest);
	return LRequest;
}

bool USRequestManager::SendRequest(const FSRequestRef& InRequest, const FString& InContent)
{
	if (!Requests.Contains(InRequest))
		return false;
	
	const bool LIsExist = ProcessingRequests.ContainsByPredicate([&](const FSProcessingRequest& InItem) -> bool
	{
		return InItem.RequestPtr == InRequest;
	});

	if (LIsExist)
		return true;
	
	const auto LRequest = FHttpModule::Get().CreateRequest();
	switch (InRequest->Type)
	{
		case ESRequestType::VGET: LRequest->SetVerb(TEXT("GET")); break;
		case ESRequestType::VPOST: LRequest->SetVerb(TEXT("POST")); break;
		case ESRequestType::VPUT: LRequest->SetVerb(TEXT("PUT")); break;
		case ESRequestType::VDELETE: LRequest->SetVerb(TEXT("DELETE")); break;
		default: ;
	}
	
	switch (InRequest->ContentType)
	{
		case ESRequestContentType::Binary: LRequest->SetHeader(TEXT("Content-Type"), TEXT("application/octet-stream")); break;
		case ESRequestContentType::Text: LRequest->SetHeader(TEXT("Content-Type"), TEXT("text/plain")); break;
		case ESRequestContentType::Json: LRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json")); break;
		default: ;
	}

	FString LToken;
	if (InRequest->Owner.IsValid() && InRequest->Owner->Implements<USRestTokenInterface>())
	{
		LToken = ISRestTokenInterface::Execute_GetRestToken(InRequest->Owner.Get());
	}

	FString LURL = InRequest->DynamicMethod.Len() > 1 ? Endpoint + InRequest->DynamicMethod : Endpoint + InRequest->Method;

	if (InRequest->Type == ESRequestType::VGET)
	{
		LURL.Append(InContent);
	}
	
	LRequest->SetURL(LURL);
	
	if (InRequest->Type != ESRequestType::VGET)
		LRequest->SetContentAsString(InContent);
	
	LRequest->SetHeader(TokenName, LToken.Len() > 5 ? LToken : TokenValue);
	LRequest->OnProcessRequestComplete().BindUObject(this, &USRequestManager::OnRequestCompleted);

	if (LRequest->ProcessRequest())
	{
		UE_LOG(LogHttp, Warning, TEXT("Request: %s, Payload: %s"), *LRequest->GetURL(), *InContent);
		ProcessingRequests.AddUnique({InRequest, LRequest });
		return true;
	}

	UE_LOG(LogHttp, Error, TEXT("Failed Request: %s, Payload: %s"), *LRequest->GetURL(), *InContent);
	return false;
}

void USRequestManager::SetEndpoint(const FString& InEndpoint)
{
	Endpoint = InEndpoint;
}

const FString& USRequestManager::GetEndpoint() const
{
	return Endpoint;	
}

void USRequestManager::SetTokenHeaderName(const FString& InName)
{
	TokenName = InName;
}

const FString& USRequestManager::GetTokenHeaderName() const
{
	return TokenName;
}

void USRequestManager::SetTokenHeaderValue(const FString& InValue)
{
	TokenValue = InValue;
}

const FString& USRequestManager::GetTokenHeaderValue() const
{
	return TokenValue;
}

void USRequestManager::OnRequestCompleted(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bConnectedSuccessfully)
{
	const auto LLambda = [&](const FSProcessingRequest& InItem) -> bool
	{
		return InItem.SystemRequestPtr == InRequest;
	};
	
	const auto LFoundRequest = ProcessingRequests.FindByPredicate(LLambda);

	if (LFoundRequest && LFoundRequest->RequestPtr.IsValid())
	{
		const auto LCode = InResponse.IsValid() ? InResponse->GetResponseCode() : -5000;
		if (const auto LFoundHandler = LFoundRequest->RequestPtr->Handlers.Find(LCode))
		{
			const auto LRealHandler = (*LFoundHandler);
			if (!LRealHandler->OnHandle(InResponse->GetContent()))
			{
				if (LFoundRequest->RequestPtr->Error.IsValid())
				{
					StaticCastSharedPtr<FSHandlerErrorCallback>(LFoundRequest->RequestPtr->Error)->OnCallback.Broadcast(LCode, InResponse->GetContentAsString());
				}
				
				UE_LOG(LogHttp, Warning, TEXT("Error: %d, Payload: %s"), LCode, *InResponse->GetContentAsString());
			}
		}
		else if (InResponse.IsValid())
		{
			if (LFoundRequest->RequestPtr->Error.IsValid())
			{
				StaticCastSharedPtr<FSHandlerErrorCallback>(LFoundRequest->RequestPtr->Error)->OnCallback.Broadcast(LCode, InResponse->GetContentAsString());
			}
			
			UE_LOG(LogHttp, Warning, TEXT("Error: %d, Payload: %s"), LCode, *InResponse->GetContentAsString());
		}
		// ReSharper disable once CppExpressionWithoutSideEffects
		LFoundRequest->RequestPtr->OnCompleted.ExecuteIfBound();
	}

	ProcessingRequests.RemoveAll(LLambda);
}
