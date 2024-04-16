// Copyright Serinc All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NetworkGuid.generated.h"

USTRUCT(BlueprintType)
struct SREST_API FNetworkGuid : public FGuid
{
	GENERATED_BODY()
	
	FNetworkGuid() : FGuid() {}
	FNetworkGuid(const FGuid& InGuid) : FGuid(InGuid) {}

	// TODO Not compiled for linux
	//operator FGuid&() { return *this; }
	//operator FGuid() const { return *this; }

	FNetworkGuid& operator=(const FGuid &InGuid)
	{
		this->A = InGuid.A;
		this->B = InGuid.B;
		this->C = InGuid.C;
		this->D = InGuid.D;
		return *this;
	}

	/**
	* Exports the GUIDs value to a string.
	*
	* @param ValueStr Will hold the string value.
	* @param DefaultValue The default value.
	* @param Parent Not used.
	* @param PortFlags Not used.
	* @param ExportRootScope Not used.
	* @return true on success, false otherwise.
	* @see ImportTextItem
	*/
	bool ExportTextItem(FString& ValueStr, FNetworkGuid const& DefaultValue, UObject* Parent, int32 PortFlags, class UObject* ExportRootScope) const;

	/**
	* Imports the GUIDs value from a text buffer.
	*
	* @param Buffer The text buffer to import from.
	* @param PortFlags Not used.
	* @param Parent Not used.
	* @param ErrorText The output device for error logging.
	* @return true on success, false otherwise.
	* @see ExportTextItem
	*/
	
	bool ImportTextItem(const TCHAR*& Buffer, int32 PortFlags, class UObject* Parent, FOutputDevice* ErrorText);

	FString ToString(EGuidFormats Format = EGuidFormats::DigitsWithHyphens) const;
	FGuid ToGuid() const;
};

template <> struct TIsPODType<FNetworkGuid> { enum { Value = true }; };

template<>
struct TStructOpsTypeTraits<FNetworkGuid> : public TStructOpsTypeTraitsBase2<FNetworkGuid>
{
	enum
	{
		WithExportTextItem = true,
		WithImportTextItem = true,
		WithZeroConstructor = true
	};
};

UE_IMPLEMENT_STRUCT("/Script/SREST", NetworkGuid);
