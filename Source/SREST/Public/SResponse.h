// Copyright Serinc All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class USRequestManager;
/**
 * 
 */
struct SREST_API FSResponse 
{
	int32								Code;
	TWeakObjectPtr<USRequestManager>	Manager;
};
