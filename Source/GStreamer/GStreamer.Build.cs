// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class GStreamer : ModuleRules
{
    public GStreamer(ReadOnlyTargetRules Target) : base(Target)
    {
        CppStandard = CppStandardVersion.Cpp17;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        PCHUsage = PCHUsageMode.NoPCHs; // UseExplicitOrSharedPCHs;
        bUseUnity = false;
        bEnableUndefinedIdentifierWarnings = false;

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "RHI",
                "RenderCore",
                "Slate",
                "SlateCore"
            }
        );

        string GStreamerRoot = System.Environment.GetEnvironmentVariable("IMAST_GSTREAMER");
        System.Console.WriteLine("*********************************************GSTREAMER PATH*********************************************");
        System.Console.WriteLine("GStreamerRoot : " + GStreamerRoot);
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            string GStreamerWindows = Path.Combine(GStreamerRoot, "msvc_x86_64");
            System.Console.WriteLine("GStreamerWindows : " + GStreamerWindows);

            PrivateIncludePaths.Add(Path.Combine(GStreamerWindows, "include"));
            PrivateIncludePaths.Add(Path.Combine(GStreamerWindows, "include", "gstreamer-1.0"));
            PrivateIncludePaths.Add(Path.Combine(GStreamerWindows, "include", "glib-2.0"));
            PrivateIncludePaths.Add(Path.Combine(GStreamerWindows, "lib", "glib-2.0", "include"));

            PublicSystemLibraryPaths.Add(Path.Combine(GStreamerWindows, "lib"));

            PublicAdditionalLibraries.Add(Path.Combine(GStreamerWindows, "lib", "glib-2.0.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(GStreamerWindows, "lib", "gobject-2.0.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(GStreamerWindows, "lib", "gstreamer-1.0.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(GStreamerWindows, "lib", "gstvideo-1.0.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(GStreamerWindows, "lib", "gstapp-1.0.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(GStreamerWindows, "lib", "gstrtspserver-1.0.lib"));

            PublicDelayLoadDLLs.Add("glib-2.0-0.dll");
            PublicDelayLoadDLLs.Add("gobject-2.0-0.dll");
            PublicDelayLoadDLLs.Add("gstreamer-1.0-0.dll");
            PublicDelayLoadDLLs.Add("gstvideo-1.0-0.dll");
            PublicDelayLoadDLLs.Add("gstapp-1.0-0.dll");
            PublicDelayLoadDLLs.Add("gstrtspserver-1.0-0.dll");
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            string GStreamerLinux = Path.Combine(GStreamerRoot, "linux");
            System.Console.WriteLine("GStreamerLinux : " + GStreamerLinux);

            PublicIncludePaths.Add(Path.Combine(GStreamerLinux, "include", "gstreamer-1.0"));
            PublicIncludePaths.Add(Path.Combine(GStreamerLinux, "include", "glib-2.0"));
            PublicIncludePaths.Add(Path.Combine(GStreamerLinux, "include", "glib-2.0", "glib"));
            PublicIncludePaths.Add(Path.Combine(GStreamerLinux, "include", "glib-2.0", "gobject"));
            PublicIncludePaths.Add(Path.Combine(GStreamerLinux, "include", "glib-2.0", "gio"));

            PublicSystemLibraryPaths.Add(Path.Combine(GStreamerLinux, "lib", "x86_64-linux-gnu"));

            PublicAdditionalLibraries.Add(Path.Combine(GStreamerLinux, "lib", "x86_64-linux-gnu", "libgobject-2.0.so"));
            PublicAdditionalLibraries.Add(Path.Combine(GStreamerLinux, "lib", "x86_64-linux-gnu", "libglib-2.0.so"));
            PublicAdditionalLibraries.Add(Path.Combine(GStreamerLinux, "lib", "x86_64-linux-gnu", "libgstreamer-1.0.so"));
            PublicAdditionalLibraries.Add(Path.Combine(GStreamerLinux, "lib", "x86_64-linux-gnu", "libgstrtspserver-1.0.so"));
            PublicAdditionalLibraries.Add(Path.Combine(GStreamerLinux, "lib", "x86_64-linux-gnu", "libgstvideo-1.0.so"));
            PublicAdditionalLibraries.Add(Path.Combine(GStreamerLinux, "lib", "x86_64-linux-gnu", "libgstapp-1.0.so"));
        }
        System.Console.WriteLine("********************************************************************************************************");
    }
}

