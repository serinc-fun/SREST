using UnrealBuildTool;

public class SRESTEditor : ModuleRules
{
    public SRESTEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "SREST"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Slate",
                "SlateCore",
                "UnrealEd",
                "PropertyEditor", 
                "SREST", "TalentCore", "JsonUtilities", "TalentUtilities",
                "DeveloperSettings"
            }
        );
    }
}