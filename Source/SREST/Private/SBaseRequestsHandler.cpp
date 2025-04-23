// Fill out your copyright notice in the Description page of Project Settings.

#include "SBaseRequestsHandler.h"

#include "SRequestsProcessor.h"

USBaseRequestsHandler::USBaseRequestsHandler()
{
	
}

void USBaseRequestsHandler::Setup(USRequestsProcessor* InRequestManager)
{
	if (RequestManager != InRequestManager)
	{
		RequestManager = InRequestManager;
		OnSetup(RequestManager);
	}
}

void USBaseRequestsHandler::PostInitProperties()
{
	UObject::PostInitProperties();

	if (bUseInternalManager)
	{
		auto LInternalManager = NewObject<USRequestsProcessor>(this);
		LInternalManager->SetEndpoint(InternalManagerEndpoint);
		Setup(LInternalManager);
	}
}

void USBaseRequestsHandler::OnSetup_Implementation(USRequestsProcessor* InRequestManager)
{
	
}
