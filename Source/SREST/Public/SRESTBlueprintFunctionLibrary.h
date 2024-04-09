// Copyright Serinc All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NetworkGuid.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SRESTBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SREST_API USRESTBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure)
	static FGuid NetworkGuidToGuid(const FNetworkGuid& InNetworkGuid);

	UFUNCTION(BlueprintPure)
	static FNetworkGuid GuidToNetworkGuid(const FGuid& InGuid);
};
