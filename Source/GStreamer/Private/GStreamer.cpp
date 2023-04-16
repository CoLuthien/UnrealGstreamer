// Copyright Epic Games, Inc. All Rights Reserved.

#include "GStreamer.h"
#include "GStreamerModule.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include <Interfaces/IPluginManager.h>
#include <HAL/FileManager.h>
#include <Windows/WindowsPlatformProcess.h>

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif
#include <CoreMinimal.h>
#include <string_view>
#define LOCTEXT_NAMESPACE "FGStreamerModule"

static const TArray<FString> DLLPaths{
    L"glib-2.0-0.dll",
    L"avcodec-58.dll",
    L"gobject-2.0-0.dll",
    L"gstreamer-1.0-0.dll",
    L"gstvideo-1.0-0.dll",
    L"gstapp-1.0-0.dll",
    L"gstrtspserver-1.0-0.dll",
    L"x264-160.dll",
};

static FString
GetGstRoot()
{
    auto&& PluginDir = FPaths::ProjectPluginsDir();
    auto&& RootPath = PluginDir + TEXT("/GStreamer/Source/Thirdparty/install");
    UE_LOG(LogTemp, Log, TEXT("Root: %s"), *RootPath);
    return RootPath;
}

void
FGStreamerModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified
    // in the .uplugin file per-module

    FString BinPath, PluginPath;

#if PLATFORM_WINDOWS
    FString RootPath = GetGstRoot();
    if (!RootPath.IsEmpty())
    {
        BinPath = FPaths::Combine(RootPath, TEXT("bin"));
        if (FPaths::DirectoryExists(BinPath))
        {
            UE_LOG(LogTemp, Log, TEXT("Root: %s"), *RootPath);
            auto Path = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*BinPath);
            //SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
            FWindowsPlatformProcess::AddDllDirectory(*Path);
            for (auto const& DLLName : DLLPaths)
            {
                auto&& DLLPath = FPaths::Combine(Path, DLLName);
                void* DLL = FWindowsPlatformProcess::GetDllHandle(*DLLPath);
                m_dll.Add(DLL);
            }
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
    for (void* DLL : m_dll)
    {
        FWindowsPlatformProcess::FreeDllHandle(DLL);
    }
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGStreamerModule, GStreamer)