// Copyright Serinc All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"

/**
 * 
 */
struct FSHandler
{
	virtual      ~FSHandler() = default;
	virtual bool OnHandle(const TArray<uint8>& InContent) { return false; }

	static SREST_API FString ConvertContentToString(const TArray<uint8>& InContent);
};

typedef TSharedPtr<FSHandler> FSHandlerPtr;
typedef TSharedRef<FSHandler> FSHandlerRef;

struct FSHandlerCallback : public FSHandler
{
	friend struct FSRequest;
	
	DECLARE_MULTICAST_DELEGATE(FOnCallback);

	virtual bool OnHandle(const TArray<uint8>& InContent) override
	{
		if (OnCallback.IsBound())
		{
			OnCallback.Broadcast();
			return true;
		}

		return false;
	}

protected:

	FOnCallback OnCallback;
};

struct FSHandlerStringCallback : public FSHandler
{
	friend struct FSRequest;
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCallback, const FString&);

	virtual bool OnHandle(const TArray<uint8>& InContent) override
	{
		if (OnCallback.IsBound())
		{
			OnCallback.Broadcast(ConvertContentToString(InContent));
			return true;
		}

		return false;
	}

protected:

	FOnCallback OnCallback;
};

struct FSHandlerErrorCallback : public FSHandler
{
	friend struct FSRequest;
	
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCallback, const int32&, const FString&);

	FOnCallback OnCallback;
};

template<typename TStruct>
struct TSHandlerUStructCallback : public FSHandler
{
	friend struct FSRequest;
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCallback, const TStruct&);

	virtual bool OnHandle(const TArray<uint8>& InContent) override
	{
		TStruct OutStruct;
		if (FJsonObjectConverter::JsonObjectStringToUStruct(ConvertContentToString(InContent), &OutStruct))
		{
			if (OnCallback.IsBound())
			{
				OnCallback.Broadcast(OutStruct);
				return true;
			}
		}

		return false;
	}

protected:

	FOnCallback OnCallback;
};

template<typename TStruct>
struct TSHandlerUStructArrayCallback : public FSHandler
{
	friend struct FSRequest;
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCallback, const TArray<TStruct>&);

	virtual bool OnHandle(const TArray<uint8>& InContent) override
	{
		TArray<TStruct> OutStructArray;
		if (FJsonObjectConverter::JsonArrayStringToUStruct(ConvertContentToString(InContent), &OutStructArray))
		{
			if (OnCallback.IsBound())
			{
				OnCallback.Broadcast(OutStructArray);
				return true;
			}
		}

		return false;
	}

protected:

	FOnCallback OnCallback;
};
