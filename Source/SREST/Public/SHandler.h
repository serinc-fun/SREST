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
	virtual void OnHeader(const FString& InName, const FString& InValue, const FName& InId = NAME_None) {}
	virtual void OnProgress(const uint64& InBytesSent, const uint64& InBytesReceived, const FName& InId = NAME_None) {}
	virtual bool OnHandle(const TArray<uint8>& InContent, const FName& InId = NAME_None) { return false; }

	static SREST_API FString ConvertContentToString(const TArray<uint8>& InContent);
};

typedef TSharedPtr<FSHandler> FSHandlerPtr;
typedef TSharedRef<FSHandler> FSHandlerRef;

struct FSHandlerDownloadCallback : public FSHandler
{
private:

	struct FSHandlerDownloadCallbackData
	{
		uint64 BytesReceived;
		uint64 BytesTotal;
	};
	
public:
	
	friend struct FSRequest;
	
	typedef TMulticastDelegate<void(const FName&, const TArray<uint8>& /*InContent*/)> FOnCompletedCallback;
	typedef TMulticastDelegate<void(const FName&, const uint64& /*InBytesReceived*/, const uint64& /*InBytesTotal*/)> FOnProgressCallback;

	virtual void OnHeader(const FString& InName, const FString& InValue, const FName& InId) override final
	{
		if (!DownloadData.Contains(InId) && InName.Equals(TEXT("Content-Length")))
		{
			const auto LValue = static_cast<uint64>(FCString::Atoi64(*InValue));
			DownloadData.Add(InId, { 0, LValue });
			OnProgressCallback.Broadcast(InId, 0, LValue);
		}	
	}

	virtual void OnProgress(const uint64& InBytesSent, const uint64& InBytesReceived, const FName& InId) override final
	{
		if (DownloadData.Contains(InId))
		{
			auto LValue = &DownloadData.FindChecked(InId);
			LValue->BytesReceived = InBytesReceived;
			OnProgressCallback.Broadcast(InId, LValue->BytesReceived, LValue->BytesTotal);
		}
	}
	
	virtual bool OnHandle(const TArray<uint8>& InContent, const FName& InId = NAME_None) override
	{
		DownloadData.Remove(InId);
		
		if (OnCompletedCallback.IsBound())
		{
			OnCompletedCallback.Broadcast(InId, InContent);
			return true;
		}

		return false;
	}

protected:

	FOnCompletedCallback OnCompletedCallback;
	FOnProgressCallback OnProgressCallback;

	TMap<FName, FSHandlerDownloadCallbackData> DownloadData;
};

struct FSHandlerRawCallback : public FSHandler
{
	friend struct FSRequest;
	
	typedef TMulticastDelegate<void(const FName&, const TArray<uint8>&)> FOnCallback;;

	virtual bool OnHandle(const TArray<uint8>& InContent, const FName& InId = NAME_None) override
	{
		if (OnCallback.IsBound())
		{
			OnCallback.Broadcast(InId, InContent);
			return true;
		}

		return false;
	}

protected:

	FOnCallback OnCallback;
};

struct FSHandlerCallback : public FSHandler
{
	friend struct FSRequest;
	
	DECLARE_MULTICAST_DELEGATE(FOnCallback);

	virtual bool OnHandle(const TArray<uint8>& InContent, const FName& InId = NAME_None) override
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

	virtual bool OnHandle(const TArray<uint8>& InContent, const FName& InId = NAME_None) override
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

	virtual bool OnHandle(const TArray<uint8>& InContent, const FName& InId = NAME_None) override
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

	virtual bool OnHandle(const TArray<uint8>& InContent, const FName& InId = NAME_None) override
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
