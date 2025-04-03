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

FSRequestRef USRequestManager::CreateRequest(UObject* InOwner, const FString& InMethod, const ESRequestType& InType/* = ESRequestType::VGET*/, const ESRequestContentType& InContentType/* = ESRequestContentType::Json*/)
{
	FSRequestRef LRequest = MakeShareable(new FSRequest(InOwner));
	LRequest->Type = InType;
	LRequest->Method = InMethod;
	LRequest->ContentType = InContentType;
	LRequest->Manager = this;	
	Requests.Add(LRequest);
	return LRequest;
}

bool USRequestManager::SendRequest(const FSRequestRef& InRequest, const FString& InContent, const FName& InId)
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
	
	LRequest->SetHeader(TokenName, LToken.Len() > 5 ? LToken : TokenValue);
	LRequest->OnHeaderReceived().BindUObject(this, &USRequestManager::OnRequestHeader);
	LRequest->OnRequestProgress64().BindUObject(this, &USRequestManager::OnRequestProgress);
	LRequest->OnProcessRequestComplete().BindUObject(this, &USRequestManager::OnRequestCompleted);

	if (LRequest->ProcessRequest())
	{		
		UE_LOG(LogHttp, Warning, TEXT("Request: %s, Payload: %s"), *LRequest->GetURL(), InContent.Len() < 4192 ? *InContent : TEXT("over size for display!"));
		
		ProcessingRequests.AddUnique(MakeShareable(new FSProcessingRequest { InId, InRequest, LRequest, false }));
		return true;
	}

	UE_LOG(LogHttp, Error, TEXT("Failed Request: %s, Payload: %s"), *LRequest->GetURL(), InContent.Len() < 4192 ? *InContent : TEXT("over size for display!"));
	return false;
}

void USRequestManager::CancelRequest(const FSRequestRef& InRequest, const FName& InId)
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

void USRequestManager::OnRequestHeader(FHttpRequestPtr InRequest, const FString& InHeaderName, const FString& InHeaderValue)
{
	const auto LLambda = [&](const TSharedPtr<FSProcessingRequest>& InItem) -> bool
	{
		return InItem->SystemRequestPtr == InRequest;
	};

	const auto LFoundRequest = ProcessingRequests.FindByPredicate(LLambda);

	if (LFoundRequest && (*LFoundRequest)->RequestPtr.IsValid())
	{
		const TSharedRef<FSProcessingRequest> LRealRequest = LFoundRequest->ToSharedRef();
		LRealRequest->RequestPtr->HeaderCallback.Broadcast((*LFoundRequest)->Id, InHeaderName, InHeaderValue);
	}
}

void USRequestManager::OnRequestProgress(FHttpRequestPtr InRequest, uint64 InBytesSent, uint64 InBytesReceived)
{
	const auto LLambda = [&](const TSharedPtr<FSProcessingRequest>& InItem) -> bool
	{
		return InItem->SystemRequestPtr == InRequest;
	};

	const auto LFoundRequest = ProcessingRequests.FindByPredicate(LLambda);

	if (LFoundRequest && (*LFoundRequest)->RequestPtr.IsValid())
	{
		const TSharedRef<FSProcessingRequest> LRealRequest = LFoundRequest->ToSharedRef();
		LRealRequest->RequestPtr->ProgressCallback.Broadcast((*LFoundRequest)->Id, InBytesSent, InBytesReceived);
	}
}

void USRequestManager::OnRequestCompleted(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bConnectedSuccessfully)
{
	const auto LLambda = [&](const TSharedPtr<FSProcessingRequest>& InItem) -> bool
	{
		return InItem->SystemRequestPtr == InRequest;
	};

	if (!ProcessingRequests.ContainsByPredicate(LLambda))
		return;
	
	auto LFoundRequest = ProcessingRequests.FindByPredicate(LLambda);

	if (LFoundRequest && (*LFoundRequest)->RequestPtr.IsValid())
	{
		const TSharedRef<FSProcessingRequest> LRealRequest = LFoundRequest->ToSharedRef();
		
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
				
				UE_LOG(LogHttp, Warning, TEXT("Error: %d, Payload: %s"), LCode, InResponse->GetContentLength() < 2048 ? *InResponse->GetContentAsString() : TEXT("over size for display!"));
			}
		}
		else if (InResponse.IsValid())
		{
			if (LRealRequest->RequestPtr->Error.IsValid())
			{
				StaticCastSharedPtr<FSHandlerErrorCallback>(LRealRequest->RequestPtr->Error)->OnCallback.Broadcast(LCode, InResponse->GetContentAsString());
			}
			
			UE_LOG(LogHttp, Warning, TEXT("Error: %d, Payload: %s"), LCode, InResponse->GetContentLength() < 2048 ? *InResponse->GetContentAsString() : TEXT("over size for display!"));
		}
		// ReSharper disable once CppExpressionWithoutSideEffects
		LRealRequest->RequestPtr->OnCompleted.ExecuteIfBound();
		LRealRequest->IsCompleted = true;
	}

	if (LFoundRequest)
	{
		ProcessingRequests.RemoveSingle(*LFoundRequest);
	}
}
