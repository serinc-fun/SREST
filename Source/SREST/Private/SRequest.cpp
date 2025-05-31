// Copyright Serinc All Rights Reserved.
#include "SRequest.h"

#include "Http.h"
#include "SRequestsProcessor.h"

FSRequest::FSRequest()
	: Type(ESRequestType::End)
	, ContentType(ESRequestContentType::End)
{
	
}

FSRequest::FSRequest(UObject* InOwner)
	: Type(ESRequestType::VERB_POST)
	, ContentType(ESRequestContentType::Json)
{
	Owner = InOwner;
}

bool FSRequest::Send(const FName& InId)
{
	if (Manager.IsValid())
	{
		return Manager->SendRequest(this->AsShared(), "", InId);
	}

	return false;
}

bool FSRequest::Send(const FString& InString, const FName& InId)
{
	if (Manager.IsValid())
	{
		return Manager->SendRequest(this->AsShared(), InString, InId);
	}

	return false;
}

void FSRequest::Cancel(const FName& InId)
{
	if (Manager.IsValid())
	{
		return Manager->CancelRequest(this->AsShared(), InId);
	}
}

bool FSRequest::GetQueryHeaderFromUStructPart(FString& InOutString, FProperty* InProperty, const void* InValue, const FString& InBaseName) const
{
	// Copy from ConvertScalarFPropertyToJsonValue method from JsonObjectConverter and modifed
	if (FEnumProperty* LEnumProperty = CastField<FEnumProperty>(InProperty))
	{
		const auto LTypedValue = LEnumProperty->GetUnderlyingProperty()->GetSignedIntPropertyValue(InValue);
		InOutString.Append(*FString::Printf(TEXT("%s%s=%lld"), *InBaseName, *InProperty->GetAuthoredName(), LTypedValue));
		return true;
	}
	else if (FNumericProperty *LNumericProperty = CastField<FNumericProperty>(InProperty))
	{
		if (LNumericProperty->IsFloatingPoint())
		{
			const auto LTypedValue = LNumericProperty->GetFloatingPointPropertyValue(InValue);
			InOutString.Append(*FString::Printf(TEXT("%s%s=%.6f"), *InBaseName, *InProperty->GetAuthoredName(), LTypedValue));
		}
		else if (LNumericProperty->IsInteger())
		{
			const auto LTypedValue = LNumericProperty->GetSignedIntPropertyValue(InValue);
			InOutString.Append(*FString::Printf(TEXT("%s%s=%lld"), *InBaseName, *InProperty->GetAuthoredName(), LTypedValue));
		}
		return true;
	}
	else if (FBoolProperty *LBoolProperty = CastField<FBoolProperty>(InProperty))
	{
		const auto LTypedValue = LBoolProperty->GetPropertyValue(InValue);
		InOutString.Append(*FString::Printf(TEXT("%s%s=%hhd"), *InBaseName, *InProperty->GetAuthoredName(), LTypedValue));
		return true;
	}
	else if (FStrProperty *LStringProperty = CastField<FStrProperty>(InProperty))
	{
		const auto LTypedValue = LStringProperty->GetPropertyValue(InValue);
		InOutString.Append(*FString::Printf(TEXT("%s%s=%s"), *InBaseName, *InProperty->GetAuthoredName(), *LTypedValue));
		return true;
	}
	else if (FTextProperty *LTextProperty = CastField<FTextProperty>(InProperty))
	{
		const auto LTypedValue = LTextProperty->GetPropertyValue(InValue).ToString();
		InOutString.Append(*FString::Printf(TEXT("%s%s=%s"), *InBaseName, *InProperty->GetAuthoredName(), *LTypedValue));
		return true;
	}

	return false;
}

void FSRequest::GetQueryHeaderFromUStruct(FString& InOutString, const UStruct* StructDefinition, const void* Struct, const FString& InBaseName) const
{
	bool bFirst = true;
	
	for (TFieldIterator<FProperty> It(StructDefinition); It; ++It)
	{
		if (!bFirst)
		{
			InOutString.Append(TEXT("&"));
		}
		
		FProperty* LProperty = *It;
		if (LProperty->ArrayDim > 1)
		{
			UE_LOG(LogHttp, Error, TEXT("GetQueryHeaderFromUStruct doesn't support array or any other container, only support 1 level entries."));
			continue;
		}
		
		FString LName = LProperty->GetAuthoredName();

		const void* LValue = LProperty->ContainerPtrToValuePtr<uint8>(Struct);
		if (!GetQueryHeaderFromUStructPart(InOutString, LProperty, LValue, InBaseName))
		{
			if (FStructProperty* LStructProperty = CastField<FStructProperty>(LProperty))
			{
				FString LValueString;
				LStructProperty->ExportTextItem_Direct(LValueString, LValue, nullptr, nullptr, PPF_None);

				if (!LValueString.IsEmpty() && LValueString.Find(TEXT("(")) != 0 && LValueString.Find(TEXT(")")) != LValueString.Len() - 1)
				{
					InOutString.Append(*FString::Printf(TEXT("%s%s=%s"), *InBaseName, *LStructProperty->GetAuthoredName(), *LValueString));
				}
				else
				{
					GetQueryHeaderFromUStruct(InOutString, LStructProperty->Struct, LValue, InBaseName + LName + TEXT("."));
				}
			}
		}

		bFirst = false;
	}
}

FString FSRequest::GetQueryHeaderFromUStruct(const UStruct* StructDefinition, const void* Struct) const
{
	bool bFirst = true;
	FString LRequest = TEXT("?");

	GetQueryHeaderFromUStruct(LRequest, StructDefinition, Struct);

	return LRequest;
}

void FSRequest::SetDynamicMethodArgs(const FStringFormatNamedArguments& InArguments)
{
	DynamicMethod = FString::Format(*Method, InArguments);
}
