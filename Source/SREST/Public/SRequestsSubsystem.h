// Copyright Serinc All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "SRequestsSubsystem.generated.h"

class USRequestsProcessor;
class USBaseRequestsHandler;
/**
 * 
 */
UCLASS()
class SREST_API USRequestsSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	USBaseRequestsHandler* AddHandlerByClass(UClass* InClass, const FString& InEndpoint = "");
	
	UFUNCTION(BlueprintPure)
	USBaseRequestsHandler* GetHandlerByClass(UClass* InClass, const FString& InEndpoint = "");

	UFUNCTION(BlueprintCallable)
	bool RemoveHandlerByClass(UClass* InClass);
	
	template<class TClass = USBaseRequestsHandler>
	TClass* AddHandler(const FString& InEndpoint = "")
	{
		return Cast<TClass>(AddHandlerByClass(TClass::StaticClass(), InEndpoint));
	}

	template<class TClass = USBaseRequestsHandler>
	TClass* GetHandler(const FString& InEndpoint = "")
	{
		return Cast<TClass>(GetHandlerByClass(TClass::StaticClass(), InEndpoint));
	}

	template<class TClass = USBaseRequestsHandler>
	TClass* RemoveHandler()
	{
		return Cast<TClass>(RemoveHandlerByClass(TClass::StaticClass()));
	}
	
protected:

	UPROPERTY()
	mutable TArray<USBaseRequestsHandler*> Handlers;

	UPROPERTY()
	mutable TMap<FName, USRequestsProcessor*> Processors;
};
