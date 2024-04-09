// Copyright Serinc All Rights Reserved.

#include "SRESTBlueprintFunctionLibrary.h"

FGuid USRESTBlueprintFunctionLibrary::NetworkGuidToGuid(const FNetworkGuid& InNetworkGuid)
{
	return InNetworkGuid;
}

FNetworkGuid USRESTBlueprintFunctionLibrary::GuidToNetworkGuid(const FGuid& InGuid)
{
	return InGuid;
}
