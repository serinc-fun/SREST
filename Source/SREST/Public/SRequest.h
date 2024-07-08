// Copyright Serinc All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "SHandler.h"
#include "SRequest.generated.h"

class USRequestManager;

UENUM(BlueprintType)
enum class ESRequestType : uint8
{
	VGET,
	VPOST,
	VPUT,
	VDELETE,
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

private:

	FSRequest();
	
public:
	
	explicit FSRequest(UObject* InOwner);
	
	bool Send();
	bool Send(const FString& InString);

	template<typename TUStruct>
	bool Send(const TUStruct& InStruct)
	{
		FString LString;

		if (Type == ESRequestType::VGET)
		{
			LString = GetQueryHeaderFromUStruct(TUStruct::StaticStruct(), &InStruct);
			return Send(LString);
		}
		else
		{
			if (FJsonObjectConverter::UStructToJsonObjectString(InStruct, LString, 0, 0, 0, nullptr, false))
			{
				return Send(LString);
			}
		}

		return false;
	}

	FString GetQueryHeaderFromUStruct(const UStruct* StructDefinition, const void* Struct) const;

	void SetDynamicMethod(const FStringFormatNamedArguments& InArguments);
	
	FSHandlerCallback::FOnCallback& BindCallback(const int32& InCode)
	{
		if (!Handlers.Contains(InCode))
		{
			Handlers.Add(InCode, MakeShareable(new FSHandlerCallback()));
		}

		return StaticCastSharedPtr<FSHandlerCallback>(Handlers.FindChecked(InCode))->OnCallback;
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
