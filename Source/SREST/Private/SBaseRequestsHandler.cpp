// Fill out your copyright notice in the Description page of Project Settings.

#include "SBaseRequestsHandler.h"

#include "SRequestsProcessor.h"

USBaseRequestsHandler::USBaseRequestsHandler()
{
	
}

void USBaseRequestsHandler::Setup(USRequestsProcessor* InRequestProcessor)
{
	if (RequestProcessor != InRequestProcessor)
	{
		RequestProcessor = InRequestProcessor;
		OnSetup(RequestProcessor);
	}
}


void USBaseRequestsHandler::OnSetup_Implementation(USRequestsProcessor* InRequestProcessor)
{
	
}
