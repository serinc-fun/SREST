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

template<bool TUseId = false>
struct FSHandlerCallback : public FSHandler
{
	friend struct FSRequest;
	
	using FOnCallback = std::conditional_t<TUseId, TMulticastDelegate<void(const FName&)>, TMulticastDelegate<void()>>;

	virtual bool OnHandle(const TArray<uint8>& InContent, const FName& InId = NAME_None) override
	{
		if (OnCallback.IsBound())
		{
			OnHandleImpl(InId, std::integral_constant<bool, TUseId>{});
			return true;
		}

		return false;
	}

protected:

	void OnHandleImpl(const FName&, std::false_type)
	{
		OnCallback.Broadcast();
	}

	void OnHandleImpl( const FName& InId, std::true_type)
	{
		OnCallback.Broadcast(InId);
	}
	
	FOnCallback OnCallback;
};

template<bool TUseId = false>
struct FSHandlerStringCallback : public FSHandler
{
	friend struct FSRequest;
	
	using FOnCallback = std::conditional_t<TUseId, TMulticastDelegate<void(const FName&, const FString&)>, TMulticastDelegate<void(const FString&)>>;

	virtual bool OnHandle(const TArray<uint8>& InContent, const FName& InId = NAME_None) override
	{
		if (OnCallback.IsBound())
		{
			OnCallback.Broadcast(ConvertContentToString(InContent));//, InId, std::integral_constant<bool, TUseId>{});
			return true;
		}

		return false;
	}

protected:

	void OnHandleImpl(const FString& InContent, const FName&, std::false_type)
	{
		OnCallback.Broadcast(InContent);
	}

	void OnHandleImpl(const FString& InContent, const FName& InId, std::true_type)
	{
		OnCallback.Broadcast(InId, InContent);
	}
	
	FOnCallback OnCallback;
};

struct FSHandlerErrorCallback : public FSHandler
{
	friend struct FSRequest;
	
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCallback, const int32&, const FString&);

	FOnCallback OnCallback;
};

template<typename TStruct, bool TUseId = false>
struct TSHandlerUStructCallback : public FSHandler
{
	friend struct FSRequest;

	using FOnCallback = std::conditional_t<TUseId, TMulticastDelegate<void(const FName&, const TStruct&)>, TMulticastDelegate<void(const TStruct&)>>;

	virtual bool OnHandle(const TArray<uint8>& InContent, const FName& InId = NAME_None) override
	{
		TStruct OutStruct;
		if (FJsonObjectConverter::JsonObjectStringToUStruct(ConvertContentToString(InContent), &OutStruct))
		{
			if (OnCallback.IsBound())
			{
				OnHandleImpl(OutStruct, InId, std::integral_constant<bool, TUseId>{});				
				return true;
			}
		}

		return false;
	}

protected:

	void OnHandleImpl(const TStruct& InContent, const FName&, std::false_type)
	{
		OnCallback.Broadcast(InContent);
	}

	void OnHandleImpl(const TStruct& InContent, const FName& InId, std::true_type)
	{
		OnCallback.Broadcast(InId, InContent);
	}
	
	FOnCallback OnCallback;
};

template<typename TStruct, bool TUseId = false>
struct TSHandlerUStructArrayCallback : public FSHandler
{
	friend struct FSRequest;
	
	typedef TMulticastDelegate<void(const TArray<TStruct>&)> FOnCallback;

	virtual bool OnHandle(const TArray<uint8>& InContent, const FName& InId = NAME_None) override
	{
		TArray<TStruct> OutStructArray;
		if (FJsonObjectConverter::JsonArrayStringToUStruct(ConvertContentToString(InContent), &OutStructArray))
		{
			if (OnCallback.IsBound())
			{
				OnHandleImpl(OutStructArray, InId, std::integral_constant<bool, TUseId>{});
				return true;
			}
		}

		return false;
	}

protected:

	void OnHandleImpl(const TArray<TStruct>& InContent, const FName&, std::false_type)
	{
		OnCallback.Broadcast(InContent);
	}

	void OnHandleImpl(const TArray<TStruct>& InContent, const FName& InId, std::true_type)
	{
		OnCallback.Broadcast(InId, InContent);
	}
	
	FOnCallback OnCallback;
};
