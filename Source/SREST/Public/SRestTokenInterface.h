// Copyright Serinc All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NetworkGuid.h"
#include "UObject/Interface.h"
#include "SRestTokenInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USRestTokenInterface : public UInterface
{
	GENERATED_BODY()
};

class SREST_API ISRestTokenInterface
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FString GetRestToken() const;
};
