// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBaseRequestsHandler.h"
#include "SFlatsInfoHandler.generated.h"

struct FFlatsInfoEntries;
class UTUPromise;
class UTUFuture;
/**
 * 
 */

UCLASS(Blueprintable)
class SREST_API USFlatsInfoHandler : public USBaseRequestsHandler
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	UTUFuture* GetFlatsInfo(const FString& InID, const int& InBuildingNumber, const int& InEntranceNumber, const int& InFloorNumber);


protected:

	void ProcessComplete(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bInSuccessful) override;

	void ReturnFlatsInfoStruct(const FFlatsInfoEntries& InInfo) const;
	
	void ErrorStringReturn(const int32& InCode, const FString& InString) const;

	
	UPROPERTY()
	UTUPromise* Promise;
	
	UPROPERTY()
	FName IDName;
};
