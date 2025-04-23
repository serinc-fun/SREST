// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SBaseRequestsHandler.generated.h"

class USRequestsProcessor;
/**
 * 
 */
UCLASS()
class SREST_API USBaseRequestsHandler : public UObject
{
	GENERATED_BODY()

public:

	USBaseRequestsHandler();

	UFUNCTION(BlueprintCallable)
	void Setup(USRequestsProcessor* InRequestManager);

protected:

	virtual void PostInitProperties() override;
	
	UFUNCTION(BlueprintNativeEvent)
	void OnSetup(USRequestsProcessor* InRequestManager);
	
	UPROPERTY(Transient)
	USRequestsProcessor* RequestManager;

	UPROPERTY(EditDefaultsOnly, Category = Configuration)
	bool bUseInternalManager = false;
	
	UPROPERTY(EditDefaultsOnly, Category = Configuration)
	FString InternalManagerEndpoint;
};
