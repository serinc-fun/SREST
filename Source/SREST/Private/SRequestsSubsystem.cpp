// Copyright Serinc All Rights Reserved.

#include "SRequestsSubsystem.h"

#include "SBaseRequestsHandler.h"
#include "SRequestsProcessor.h"

USBaseRequestsHandler* USRequestsSubsystem::AddHandlerByClass(UClass* InClass)
{
	if (auto LFoundHandler = GetHandlerByClass(InClass))
	{
		return LFoundHandler;
	}

	Handlers.Add(NewObject<USBaseRequestsHandler>(this, InClass));
	return Handlers.Last();
}

USBaseRequestsHandler* USRequestsSubsystem::AddHandlerByClassAndEndpoint(UClass* InClass, const FString& InEndpoint)
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

	Handlers.Add(NewObject<USBaseRequestsHandler>(this, InClass));
	Handlers.Last()->Setup(LProcessor);
	return Handlers.Last();
}

USBaseRequestsHandler* USRequestsSubsystem::GetHandlerByClass(UClass* InClass) const
{
	for (const auto& LHandler : Handlers)
	{
		if (IsValid(LHandler) && LHandler->IsA(InClass))
		{
			return LHandler;
		}
	}

	return nullptr;
}

bool USRequestsSubsystem::RemoveHandlerByClass(UClass* InClass)
{
	if (auto LFoundHandler = GetHandlerByClass(InClass))
	{
		return Handlers.Remove(LFoundHandler) > 0;
	}

	return false;
}