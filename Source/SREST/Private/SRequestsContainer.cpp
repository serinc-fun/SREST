// Fill out your copyright notice in the Description page of Project Settings.

#include "SRequestsContainer.h"

#include "SRequestManager.h"

USRequestsContainer::USRequestsContainer()
{
	
}

void USRequestsContainer::Setup(USRequestManager* InRequestManager)
{
	if (RequestManager != InRequestManager)
	{
		RequestManager = InRequestManager;
		OnSetup(RequestManager);
	}
}

void USRequestsContainer::PostInitProperties()
{
	UObject::PostInitProperties();

	if (bUseInternalManager)
	{
		auto LInternalManager = NewObject<USRequestManager>(this);
		LInternalManager->SetEndpoint(InternalManagerEndpoint);
		Setup(LInternalManager);
	}
}

void USRequestsContainer::OnSetup_Implementation(USRequestManager* InRequestManager)
{
	
}
