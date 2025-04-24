// Copyright Serinc All Rights Reserved.

#include "SRequestsSubsystem.h"

#include "SBaseRequestsHandler.h"
#include "SRequestsProcessor.h"


USBaseRequestsHandler* USRequestsSubsystem::AddHandlerByClass(UClass* InClass, const FString& InEndpoint)
{
	USRequestsProcessor* LProcessor = nullptr;
	if (Processors.Contains(*InEndpoint))
	{
		LProcessor = Processors.FindChecked(*InEndpoint);
	}

	if (!IsValid(LProcessor))
	{
		LProcessor = NewObject<USRequestsProcessor>(this);
		LProcessor->SetEndpoint(InEndpoint);

		Processors.Add(*InEndpoint, LProcessor);
	}

	auto LNewHandler = NewObject<USBaseRequestsHandler>(this, InClass);
	Handlers.Add(LNewHandler);
	LNewHandler->Setup(LProcessor);
	return LNewHandler;
}

USBaseRequestsHandler* USRequestsSubsystem::GetHandlerByClass(UClass* InClass, const FString& InEndpoint)
{
	for (const auto& LHandler : Handlers)
	{
		if (IsValid(LHandler) && LHandler->IsA(InClass))
		{
			return LHandler;
		}
	}

	return AddHandlerByClass(InClass, InEndpoint);
}

bool USRequestsSubsystem::RemoveHandlerByClass(UClass* InClass)
{
	if (auto LFoundHandler = GetHandlerByClass(InClass))
	{
		return Handlers.Remove(LFoundHandler) > 0;
	}

	return false;
}