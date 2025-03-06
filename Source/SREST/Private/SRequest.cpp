// Copyright Serinc All Rights Reserved.
#include "SRequest.h"

#include "Http.h"
#include "SRequestManager.h"

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

FString FSRequest::GetQueryHeaderFromUStruct(const UStruct* StructDefinition, const void* Struct) const
{
	bool bFirst = true;
	FString LRequest = TEXT("?");
	
	for (TFieldIterator<FProperty> It(StructDefinition); It; ++It)
	{
		if (!bFirst)
		{
			LRequest.Append(TEXT("&"));
		}
		
		FProperty* LProperty = *It;
		if (LProperty->ArrayDim > 1)
		{
			UE_LOG(LogHttp, Error, TEXT("GetQueryHeaderFromUStruct doesn't support array or any other container, only support 1 level entries."));
			continue;
		}
		
		FString LName = LProperty->GetAuthoredName();

		const void* LValue = LProperty->ContainerPtrToValuePtr<uint8>(Struct);

		// Copy from ConvertScalarFPropertyToJsonValue method from JsonObjectConverter and modifed
		if (FEnumProperty* LEnumProperty = CastField<FEnumProperty>(LProperty))
		{
			const auto LTypedValue = LEnumProperty->GetUnderlyingProperty()->GetSignedIntPropertyValue(LValue);
			LRequest.Append(*FString::Printf(TEXT("%s=%lld"), *LName, LTypedValue));
		}
		else if (FNumericProperty *LNumericProperty = CastField<FNumericProperty>(LProperty))
		{
			if (LNumericProperty->IsFloatingPoint())
			{
				const auto LTypedValue = LNumericProperty->GetFloatingPointPropertyValue(LValue);
				LRequest.Append(*FString::Printf(TEXT("%s=%.6f"), *LName, LTypedValue));
			}
			else if (LNumericProperty->IsInteger())
			{
				const auto LTypedValue = LNumericProperty->GetSignedIntPropertyValue(LValue);
				LRequest.Append(*FString::Printf(TEXT("%s=%lld"), *LName, LTypedValue));
			}
		}
		else if (FBoolProperty *LBoolProperty = CastField<FBoolProperty>(LProperty))
		{
			const auto LTypedValue = LBoolProperty->GetPropertyValue(LValue);
			LRequest.Append(*FString::Printf(TEXT("%s=%hhd"), *LName, LTypedValue));
		}
		else if (FStrProperty *LStringProperty = CastField<FStrProperty>(LProperty))
		{
			const auto LTypedValue = LStringProperty->GetPropertyValue(LValue);
			LRequest.Append(*FString::Printf(TEXT("%s=%s"), *LName, *LTypedValue));
		}
		else if (FTextProperty *LTextProperty = CastField<FTextProperty>(LProperty))
		{
			const auto LTypedValue = LTextProperty->GetPropertyValue(LValue).ToString();
			LRequest.Append(*FString::Printf(TEXT("%s=%s"), *LName, *LTypedValue));
		}

		bFirst = false;
	}

	return LRequest;
}

void FSRequest::SetDynamicMethodArgs(const FStringFormatNamedArguments& InArguments)
{
	DynamicMethod = FString::Format(*Method, InArguments);
}
