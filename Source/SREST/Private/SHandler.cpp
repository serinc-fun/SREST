// Copyright Serinc All Rights Reserved.
#include "SHandler.h"

FString FSHandler::ConvertContentToString(const TArray<uint8>& InContent)
{
	FUTF8ToTCHAR TCHARData(reinterpret_cast<const ANSICHAR*>(InContent.GetData()), InContent.Num());
	return FString(TCHARData.Length(), TCHARData.Get());
}
