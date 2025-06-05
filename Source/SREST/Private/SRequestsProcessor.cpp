// Copyright Serinc All Rights Reserved.
#include "SRequestsProcessor.h"

#include "SHandler.h"
#include "SRestTokenInterface.h"
#include "Http.h"
#include "HttpModule.h"
#include "SSettings.h"
#include "Interfaces/IHttpResponse.h"

USRequestsProcessor::USRequestsProcessor()
{
	
}

FSRequestRef USRequestsProcessor::CreateRequest(const FString& InMethod, const ESRequestType& InType)
{
	return CreateRequest(GetOuter(), InMethod, InType);
}

FSRequestRef USRequestsProcessor::CreateRequest(UObject* InOwner, const FString& InMethod, const ESRequestType& InType/* = ESRequestType::VGET*/, const ESRequestContentType& InContentType/* = ESRequestContentType::Json*/)
{
	FSRequestRef LRequest = MakeShareable(new FSRequest(InOwner));
	LRequest->Type = InType;
	LRequest->Method = InMethod;
	LRequest->ContentType = InContentType;
	LRequest->Manager = this;	
	Requests.Add(LRequest);
	return LRequest;
}

bool USRequestsProcessor::SendRequest(const FSRequestRef& InRequest, const FString& InContent, const FName& InId)
{
	if (!Requests.Contains(InRequest))
		return false;

	// Clear completed, double solution for correct work
	ProcessingRequests.RemoveAll([](const TSharedPtr<FSProcessingRequest>& InItem)
	{
		return InItem->IsCompleted;
	});
	
	const bool LIsExist = ProcessingRequests.ContainsByPredicate([&](const TSharedPtr<FSProcessingRequest>& InItem) -> bool
	{
		return InItem->RequestPtr == InRequest && InItem->Id == InId;
	});

	if (LIsExist)
		return true;
	
	const auto LRequest = FHttpModule::Get().CreateRequest();
	switch (InRequest->Type)
	{
		case ESRequestType::VERB_GET: LRequest->SetVerb(TEXT("GET")); break;
		case ESRequestType::VERB_POST: LRequest->SetVerb(TEXT("POST")); break;
		case ESRequestType::VERB_PUT: LRequest->SetVerb(TEXT("PUT")); break;
		case ESRequestType::VERB_DELETE: LRequest->SetVerb(TEXT("DELETE")); break;
		case ESRequestType::VERB_HEAD: LRequest->SetVerb(TEXT("HEAD")); break;
		case ESRequestType::VERB_CONNECT: LRequest->SetVerb(TEXT("CONNECT")); break;
		case ESRequestType::VERB_OPTIONS: LRequest->SetVerb(TEXT("OPTIONS")); break;
		case ESRequestType::VERB_TRACE: LRequest->SetVerb(TEXT("TRACE")); break;
		case ESRequestType::VERB_PATCH: LRequest->SetVerb(TEXT("PATCH")); break;
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

	if (InRequest->Type == ESRequestType::VERB_GET || InRequest->Type == ESRequestType::VERB_HEAD)
	{
		LURL.Append(InContent);
	}
	
	LRequest->SetURL(LURL);
	
	if (InRequest->Type != ESRequestType::VERB_GET && InRequest->Type != ESRequestType::VERB_HEAD)
		LRequest->SetContentAsString(InContent);

	if (!TokenName.IsEmpty())
		LRequest->SetHeader(TokenName, LToken.Len() > 5 ? LToken : TokenValue);
	
	LRequest->OnHeaderReceived().BindUObject(this, &USRequestsProcessor::OnRequestHeader);
	LRequest->OnStatusCodeReceived().BindUObject(this, &USRequestsProcessor::OnRequestCode);
	LRequest->OnRequestProgress64().BindUObject(this, &USRequestsProcessor::OnRequestProgress);
	LRequest->OnProcessRequestComplete().BindUObject(this, &USRequestsProcessor::OnRequestCompleted);

	if (LRequest->ProcessRequest())
	{		
		UE_LOG(LogHttp, Warning, TEXT("Request: %s, Payload: %s"), *LRequest->GetURL(), InContent.Len() < Settings->GetContentLenghtForDisplaySend() ? *InContent : TEXT("over size for display!"));
		
		ProcessingRequests.AddUnique(MakeShareable(new FSProcessingRequest { InId, InRequest, LRequest, false }));
		return true;
	}

	UE_LOG(LogHttp, Error, TEXT("Failed Request: %s, Payload: %s"), *LRequest->GetURL(), InContent.Len() < Settings->GetContentLenghtForDisplaySend() ? *InContent : TEXT("over size for display!"));
	return false;
}

void USRequestsProcessor::CancelRequest(const FSRequestRef& InRequest, const FName& InId)
{
	const auto LLambda = [&](const TSharedPtr<FSProcessingRequest>& InItem) -> bool
	{
		return InItem->RequestPtr == InRequest && InItem->Id == InId;
	};

	const auto LFoundRequest = ProcessingRequests.FindByPredicate(LLambda);

	if (LFoundRequest && (*LFoundRequest)->RequestPtr.IsValid())
	{
		const TSharedRef<FSProcessingRequest> LRealRequest = LFoundRequest->ToSharedRef();
		LRealRequest->IsCompleted = true;
		LRealRequest->SystemRequestPtr->CancelRequest();
	}
}

void USRequestsProcessor::SetEndpoint(const FString& InEndpoint)
{
	Endpoint = InEndpoint;
}

const FString& USRequestsProcessor::GetEndpoint() const
{
	return Endpoint;	
}

void USRequestsProcessor::SetTokenHeaderName(const FString& InName)
{
	TokenName = InName;
}

const FString& USRequestsProcessor::GetTokenHeaderName() const
{
	return TokenName;
}

void USRequestsProcessor::SetTokenHeaderValue(const FString& InValue)
{
	TokenValue = InValue;
}

const FString& USRequestsProcessor::GetTokenHeaderValue() const
{
	return TokenValue;
}

void USRequestsProcessor::OnRequestCode(FHttpRequestPtr InRequest, int32 InStatusCode)
{
	auto LFoundRequest = GetRequestDataBySystemRequest(InRequest.ToSharedRef());
	if (LFoundRequest.IsValid() && LFoundRequest->RequestPtr.IsValid())
	{
		const TSharedRef<FSProcessingRequest> LRealRequest = LFoundRequest.ToSharedRef();
		LRealRequest->Code = InStatusCode;		
	}
}

void USRequestsProcessor::OnRequestHeader(FHttpRequestPtr InRequest, const FString& InHeaderName, const FString& InHeaderValue)
{
	auto LFoundRequest = GetRequestDataBySystemRequest(InRequest.ToSharedRef());
	if (LFoundRequest.IsValid() && LFoundRequest->RequestPtr.IsValid())
	{
		const TSharedRef<FSProcessingRequest> LRealRequest = LFoundRequest.ToSharedRef();
		if (const auto LFoundHandler = LRealRequest->RequestPtr->Handlers.Find(LRealRequest->Code))
		{
			const auto LRealHandler = (*LFoundHandler);
			LRealHandler->OnHeader(InHeaderName, InHeaderValue, LRealRequest->Id);
		}
	}
}

void USRequestsProcessor::OnRequestProgress(FHttpRequestPtr InRequest, uint64 InBytesSent, uint64 InBytesReceived)
{
	auto LFoundRequest = GetRequestDataBySystemRequest(InRequest.ToSharedRef());
	if (LFoundRequest.IsValid() && LFoundRequest->RequestPtr.IsValid())
	{
		const TSharedRef<FSProcessingRequest> LRealRequest = LFoundRequest.ToSharedRef();
		if (const auto LFoundHandler = LRealRequest->RequestPtr->Handlers.Find(LRealRequest->Code))
		{
			const auto LRealHandler = (*LFoundHandler);
			LRealHandler->OnProgress(InBytesSent, InBytesReceived, LRealRequest->Id);
		}
	}
}

void USRequestsProcessor::OnRequestCompleted(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bConnectedSuccessfully)
{
	auto LFoundRequest = GetRequestDataBySystemRequest(InRequest.ToSharedRef());
	if (!LFoundRequest.IsValid())
		return;

	if (LFoundRequest.IsValid() && LFoundRequest->RequestPtr.IsValid())
	{
		const TSharedRef<FSProcessingRequest> LRealRequest = LFoundRequest.ToSharedRef();
		
		const auto LCode = InResponse.IsValid() ? InResponse->GetResponseCode() : -5000;
		if (const auto LFoundHandler = LRealRequest->RequestPtr->Handlers.Find(LCode))
		{
			const auto LRealHandler = (*LFoundHandler);
			if (!LRealHandler->OnHandle(InResponse->GetContent(), LRealRequest->Id))
			{
				if (LRealRequest->RequestPtr->Error.IsValid())
				{
					StaticCastSharedPtr<FSHandlerErrorCallback>(LRealRequest->RequestPtr->Error)->OnCallback.Broadcast(LCode, InResponse->GetContentAsString());
				}
				
				UE_LOG(LogHttp, Warning, TEXT("Error: %d, Payload: %s"), LCode, InResponse->GetContentLength() < Settings->GetContentLenghtForDisplayCompleted() ? *InResponse->GetContentAsString() : TEXT("over size for display!"));
			}
		}
		else if (InResponse.IsValid())
		{
			if (LRealRequest->RequestPtr->Error.IsValid())
			{
				StaticCastSharedPtr<FSHandlerErrorCallback>(LRealRequest->RequestPtr->Error)->OnCallback.Broadcast(LCode, InResponse->GetContentAsString());
			}
			
			UE_LOG(LogHttp, Warning, TEXT("Error: %d, Payload: %s"), LCode, InResponse->GetContentLength() < Settings->GetContentLenghtForDisplayCompleted() ? *InResponse->GetContentAsString() : TEXT("over size for display!"));
		}
		
		LRealRequest->IsCompleted = true;
	}

	if (LFoundRequest.IsValid())
	{
		ProcessingRequests.RemoveSingle(LFoundRequest);
	}
}

TSharedPtr<FSProcessingRequest> USRequestsProcessor::GetRequestDataBySystemRequest(FHttpRequestRef InSystemRequest) const
{
	const auto LLambda = [&](const TSharedPtr<FSProcessingRequest>& InItem) -> bool
	{
		return InItem->SystemRequestPtr == InSystemRequest;
	};
	
	if (const auto LFoundRequest = ProcessingRequests.FindByPredicate(LLambda))
		return *LFoundRequest;
	return nullptr;
}

void USRequestsProcessor::PostInitProperties()
{
	Super::PostInitProperties();

	Settings = USSettings::GetSettings();
}
