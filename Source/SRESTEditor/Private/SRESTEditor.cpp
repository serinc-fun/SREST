#include "SRESTEditor.h"

#include "NetworkGuidCustomization.h"
#include "PropertyEditorModule.h"

#define LOCTEXT_NAMESPACE "FSRESTEditorModule"

void FSRESTEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	
	PropertyEditorModule.RegisterCustomPropertyTypeLayout("NetworkGuid", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FNetworkGuidCustomization::MakeInstance));
}

void FSRESTEditorModule::ShutdownModule()
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	PropertyEditorModule.UnregisterCustomPropertyTypeLayout("NetworkGuid");
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FSRESTEditorModule, SRESTEditor)