// Copyright Serinc All Rights Reserved.
#include "NetworkGuid.h"


bool FNetworkGuid::ExportTextItem(FString& ValueStr, FNetworkGuid const& DefaultValue,	UObject* Parent, int32 PortFlags, UObject* ExportRootScope) const
{
	ValueStr += ToString(EGuidFormats::DigitsWithHyphens);

	return true;
}

bool FNetworkGuid::ImportTextItem(const TCHAR*& Buffer, int32 PortFlags, UObject* Parent, FOutputDevice* ErrorText)
{
	if (FPlatformString::Strlen(Buffer) < 32)
	{
		return false;
	}

	if (ParseExact(FString(Buffer).Left(68), EGuidFormats::HexValuesInBraces, *this))
	{
		Buffer += 68;
		return true;
	}
	else if (ParseExact(FString(Buffer).Left(38), EGuidFormats::DigitsWithHyphensInBraces, *this) || ParseExact(FString(Buffer).Left(38), EGuidFormats::DigitsWithHyphensInParentheses, *this))
	{
		Buffer += 38;
		return true;
	}
	else if (ParseExact(FString(Buffer).Left(36), EGuidFormats::DigitsWithHyphens, *this))
	{
		Buffer += 36;
		return true;
	}
	else if (ParseExact(FString(Buffer).Left(35), EGuidFormats::UniqueObjectGuid, *this))
	{
		Buffer += 35;
		return true;
	}
	else if (ParseExact(FString(Buffer).Left(32), EGuidFormats::Digits, *this))
	{
		Buffer += 32;
		return true;
	}	

	return false;
}

FString FNetworkGuid::ToString(EGuidFormats Format) const
{
	return FGuid::ToString(Format);
}

FGuid FNetworkGuid::ToGuid() const
{
	return static_cast<FGuid>(*this);
}
