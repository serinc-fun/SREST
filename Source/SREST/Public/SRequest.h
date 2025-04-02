// Copyright Serinc All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "SHandler.h"
#include "SRequest.generated.h"

class USRequestManager;

// https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods
UENUM(BlueprintType)
enum class ESRequestType : uint8
{
	VERB_GET, // The GET method requests a representation of the specified resource. Requests using GET should only retrieve data.
	VERB_HEAD, // The HEAD method asks for a response identical to a GET request, but without the response body.
	VERB_POST, // The POST method submits an entity to the specified resource, often causing a change in state or side effects on the server.
	VERB_PUT, // The PUT method replaces all current representations of the target resource with the request payload.
	VERB_DELETE, // The DELETE method deletes the specified resource.
	VERB_CONNECT, // The CONNECT method establishes a tunnel to the server identified by the target resource.
	VERB_OPTIONS, // The OPTIONS method describes the communication options for the target resource.
	VERB_TRACE, // The TRACE method performs a message loop-back test along the path to the target resource.
	VERB_PATCH, // The PATCH method applies partial modifications to a resource.
	End UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ESRequestContentType : uint8
{
	Binary,
	Text,
	Json,
	End UMETA(Hidden)
};
/**
 * 
 */
struct SREST_API FSRequest : public TSharedFromThis<FSRequest>
{
public:

	typedef TMulticastDelegate<void(const FName&, const uint64&, const uint64&)> FOnProgressCallback;
	typedef TMulticastDelegate<void(const FName&, const FString&, const FString&)> FOnHeaderCallback;

private:
	
	friend class USRequestManager;
	
	ESRequestType						Type;
	ESRequestContentType				ContentType;
	FString								Method;
	FString								DynamicMethod;

protected:

	FSHandlerPtr						Error;
	TMap<int32, FSHandlerPtr>			Handlers;
	TWeakObjectPtr<USRequestManager>	Manager;
	TWeakObjectPtr<UObject>				Owner;
	FOnProgressCallback					ProgressCallback;
	FOnHeaderCallback					HeaderCallback;

private:

	FSRequest();
	
public:

	
	
	explicit FSRequest(UObject* InOwner);
	
	bool Send(const FName& InId = NAME_None);
	bool Send(const FString& InString, const FName& InId = NAME_None);

	template<typename TUStruct>
	bool Send(const TUStruct& InStruct, const FName& InId = NAME_None)
	{
		FString LString;

		if (Type == ESRequestType::VERB_GET)
		{
			LString = GetQueryHeaderFromUStruct(TUStruct::StaticStruct(), &InStruct);
			return Send(LString, InId);
		}
		else
		{
			if (FJsonObjectConverter::UStructToJsonObjectString(InStruct, LString, 0, 0, 0, nullptr, false))
			{
				return Send(LString, InId);
			}
		}

		return false;
	}

	FString GetQueryHeaderFromUStruct(const UStruct* StructDefinition, const void* Struct) const;

	void SetDynamicMethodArgs(const FStringFormatNamedArguments& InArguments);

	FOnProgressCallback& BindProgress()
	{
		return ProgressCallback;
	}

	FOnHeaderCallback& BindHeader()
	{
		return HeaderCallback;
	}
	
	FSHandlerCallback::FOnCallback& BindCallback(const int32& InCode)
	{
		if (!Handlers.Contains(InCode))
		{
			Handlers.Add(InCode, MakeShareable(new FSHandlerCallback()));
		}

		return StaticCastSharedPtr<FSHandlerCallback>(Handlers.FindChecked(InCode))->OnCallback;
	}

	FSHandlerRawCallback::FOnCallback& BindRawCallback(const int32& InCode)
	{
		if (!Handlers.Contains(InCode))
		{
			Handlers.Add(InCode, MakeShareable(new FSHandlerRawCallback()));
		}

		return StaticCastSharedPtr<FSHandlerRawCallback>(Handlers.FindChecked(InCode))->OnCallback;
	}
	
	FSHandlerStringCallback::FOnCallback& BindStringCallback(const int32& InCode)
	{
		if (!Handlers.Contains(InCode))
		{
			Handlers.Add(InCode, MakeShareable(new FSHandlerStringCallback()));
		}

		return StaticCastSharedPtr<FSHandlerStringCallback>(Handlers.FindChecked(InCode))->OnCallback;
	}

	template<typename TUStruct>
	typename TSHandlerUStructCallback<TUStruct>::FOnCallback& BindUStructCallback(const int32& InCode)
	{
		if (!Handlers.Contains(InCode))
		{
			Handlers.Add(InCode, MakeShareable(new TSHandlerUStructCallback<TUStruct>()));
		}

		return StaticCastSharedPtr<TSHandlerUStructCallback<TUStruct>>(Handlers.FindChecked(InCode))->OnCallback;
	}

	template<typename TUStruct>
	typename TSHandlerUStructArrayCallback<TUStruct>::FOnCallback& BindUStructArrayCallback(const int32& InCode)
	{
		if (!Handlers.Contains(InCode))
		{
			Handlers.Add(InCode, MakeShareable(new TSHandlerUStructArrayCallback<TUStruct>()));
		}

		return StaticCastSharedPtr<TSHandlerUStructArrayCallback<TUStruct>>(Handlers.FindChecked(InCode))->OnCallback;
	}

	FSHandlerErrorCallback::FOnCallback& BindErrorCallback()
	{
		if (!Error)
		{
			Error = MakeShareable(new FSHandlerErrorCallback());
		}

		return StaticCastSharedPtr<FSHandlerErrorCallback>(Error)->OnCallback;
	}

	FSimpleDelegate OnCompleted;
};

typedef TSharedPtr<FSRequest> FSRequestPtr;
typedef TSharedRef<FSRequest> FSRequestRef;
