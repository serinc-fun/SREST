// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SRequestsContainer.generated.h"

class USRequestManager;
/**
 * 
 */
UCLASS()
class SREST_API USRequestsContainer : public UObject
{
	GENERATED_BODY()

public:

	USRequestsContainer();

	UFUNCTION(BlueprintCallable)
	void Setup(USRequestManager* InRequestManager);

protected:

	virtual void PostInitProperties() override;
	
	UFUNCTION(BlueprintNativeEvent)
	void OnSetup(USRequestManager* InRequestManager);
	
	UPROPERTY(Transient)
	USRequestManager* RequestManager;

	UPROPERTY(EditDefaultsOnly, Category = Configuration)
	bool bUseInternalManager = false;
	
	UPROPERTY(EditDefaultsOnly, Category = Configuration)
	FString InternalManagerEndpoint;
};
