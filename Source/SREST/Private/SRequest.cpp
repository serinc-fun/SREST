// Copyright Serinc All Rights Reserved.
#include "SRequest.h"

#include "SRequestManager.h"

FSRequest::FSRequest()
	: Type(ESRequestType::End)
	, ContentType(ESRequestContentType::End)
{
	
}

FSRequest::FSRequest(UObject* InOwner)
	: Type(ESRequestType::VPOST)
	, ContentType(ESRequestContentType::Json)
{
	Owner = InOwner;
}

bool FSRequest::Send()
{
	if (Manager.IsValid())
	{
		return Manager->SendRequest(this->AsShared());
	}

	return false;
}

bool FSRequest::Send(const FString& InString)
{
	if (Manager.IsValid())
	{
		return Manager->SendRequest(this->AsShared(), InString);
	}

	return false;
}
