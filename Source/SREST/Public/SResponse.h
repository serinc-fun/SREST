// Copyright Serinc All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class USRequestsProcessor;
/**
 * 
 */
struct SREST_API FSResponse 
{
	int32								Code;
	TWeakObjectPtr<USRequestsProcessor>	Manager;
};
