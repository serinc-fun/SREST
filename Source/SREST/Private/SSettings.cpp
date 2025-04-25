// Fill out your copyright notice in the Description page of Project Settings.


#include "SSettings.h"

FName USSettings::GetSectionName() const
{
	return "SREST";
}

FName USSettings::GetContainerName() const
{
	return "Project";
}

FName USSettings::GetCategoryName() const
{
	return "SREST";
}

#if WITH_EDITOR
FText USSettings::GetSectionText() const
{
	return NSLOCTEXT("SRestSettings", "SRestTextSection", "SREST");
}

FText USSettings::GetSectionDescription() const
{
	return NSLOCTEXT("SRestSettings", "SRestTextSectionDescription", "Settings for SREST");
}
#endif

USSettings* USSettings::GetSettings()
{
	return GetMutableDefault<USSettings>();
}

