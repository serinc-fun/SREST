// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBaseRequestsHandler.h"
#include "SBackendFlatStateUpdate.generated.h"

class UTUPromise;
struct FTCEditorProjectData;
class UTUFuture;
/**
 * 
 */
UCLASS(Blueprintable)
class SRESTEDITOR_API USBackendFlatStateUpdate : public USBaseRequestsHandler
{
	GENERATED_BODY()
	
	public:
	virtual void OnSetup_Implementation(USRequestsProcessor* InRequestProcessor) override;

	UFUNCTION(BlueprintCallable)
	UTUFuture* UploadToBackend(FTCEditorProjectData InData);


protected:

	UFUNCTION()
	void StringReturnTEST(const FString& InString);
	
	UFUNCTION()
	void ErrorStringReturnTEST(const int32& InCode, const FString& InString);
	
	UPROPERTY()
	UTUPromise* Promise;

	UPROPERTY()
	FName IDName;
};
