// Copyright Epic Games, Inc. All Rights Reserved.

#include "GStreamer.h"
#include "GStreamerModule.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include <Interfaces/IPluginManager.h>

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif
#define LOCTEXT_NAMESPACE "FGStreamerModule"

static FString
GetGstRoot()
{
    auto&& PluginDir = IPluginManager::Get().FindPlugin("GStreamer")->GetBaseDir();
    auto RootPath = PluginDir + TEXT("/Sources/Thirdparty/install/bin");
    if (RootPath.IsEmpty())
        RootPath = FPlatformMisc::GetEnvironmentVariable(TEXT("GSTREAMER_1_0_ROOT_MSVC_X86_64"));
    if (RootPath.IsEmpty())
        RootPath = FPlatformMisc::GetEnvironmentVariable(TEXT("GSTREAMER_ROOT_X86_64"));
    if (RootPath.IsEmpty())
        RootPath = FPlatformMisc::GetEnvironmentVariable(TEXT("GSTREAMER_ROOT"));
    return RootPath;
}

void
FGStreamerModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified
    // in the .uplugin file per-module

    FString BinPath, PluginPath;

#if PLATFORM_WINDOWS
    FString RootPath = FPaths::Combine(GetGstRoot(), TEXT("msvc_x86_64"));
    if (!RootPath.IsEmpty())
    {
        BinPath = FPaths::Combine(RootPath, TEXT("bin"));
        if (FPaths::DirectoryExists(BinPath))
        {
            SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
            // SetDllDirectoryW(*BinPath);
            AddDllDirectory(*BinPath);
        }
        else
        {
            BinPath = "";
        }
        PluginPath = FPaths::Combine(RootPath, TEXT("lib"), TEXT("gstreamer-1.0"));
        if (!FPaths::DirectoryExists(PluginPath))
        {
            PluginPath = "";
        }
    }
    else
    {
    }
#endif

    Initialize(TCHAR_TO_ANSI(*BinPath), TCHAR_TO_ANSI(*PluginPath));
}

void
FGStreamerModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that
    // support dynamic reloading, we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGStreamerModule, GStreamer)