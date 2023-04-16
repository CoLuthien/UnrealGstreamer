// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using System.Linq;
using System.Diagnostics;
using System.Text;
using UnrealBuildTool;


public class GStreamer : ModuleRules
{

    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string ThirdPartyPath
    {
        get
        {
            return Path.GetFullPath(Path.Combine(ModulePath, "../Thirdparty/gstreamer"));
        }
    }
    private string LibraryPath
    {
        get
        {
            return Path.GetFullPath(Path.Combine(ModulePath, "../Thirdparty/install"));
        }
    }
    private void BuildGstreamer()
    {
        string SetupCommand =
            "meson setup " + ThirdPartyPath + "/build " + ThirdPartyPath + " --vsenv";
        string BuildCommand =
            "meson compile -C " + ThirdPartyPath + "/build ";
        string InstallCommand =
            "meson install -C " + ThirdPartyPath + "/build  --destdir " + LibraryPath;
        if (System.IO.Directory.Exists(LibraryPath))
        {
            return;
        }
        ExecuteCommandSync(SetupCommand);
        ExecuteCommandSync(BuildCommand);
        ExecuteCommandSync(InstallCommand);
    }
    public GStreamer(ReadOnlyTargetRules Target) : base(Target)
    {
        CppStandard = CppStandardVersion.Cpp20;
        PCHUsage = PCHUsageMode.NoPCHs; // UseExplicitOrSharedPCHs;
        bUseUnity = false;
        bEnableUndefinedIdentifierWarnings = false;

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "RHI",
                "RenderCore",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Projects",
            });

        BuildGstreamer();

        System.Console.WriteLine("*********************************************GSTREAMER PATH*********************************************");
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {

            PrivateIncludePaths.Add(Path.Combine(LibraryPath, "include"));
            PrivateIncludePaths.Add(Path.Combine(LibraryPath, "include", "gstreamer-1.0"));
            PrivateIncludePaths.Add(Path.Combine(LibraryPath, "include", "glib-2.0"));
            PrivateIncludePaths.Add(Path.Combine(LibraryPath, "lib", "glib-2.0", "include"));


            PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "lib", "glib-2.0.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "lib", "gobject-2.0.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "lib", "gstreamer-1.0.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "lib", "gstvideo-1.0.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "lib", "gstapp-1.0.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "lib", "gstrtspserver-1.0.lib"));

            PublicSystemLibraryPaths.Add(Path.Combine(LibraryPath, "bin"));
            PublicDelayLoadDLLs.Add("glib-2.0-0.dll");
            PublicDelayLoadDLLs.Add("gobject-2.0-0.dll");
            PublicDelayLoadDLLs.Add("gstreamer-1.0-0.dll");
            PublicDelayLoadDLLs.Add("gstvideo-1.0-0.dll");
            PublicDelayLoadDLLs.Add("gstapp-1.0-0.dll");
            PublicDelayLoadDLLs.Add("gstrtspserver-1.0-0.dll");

            RuntimeDependencies.Add("glib-1.0-0.dll");
            RuntimeDependencies.Add("gobject-2.0-0.dll");
            RuntimeDependencies.Add("gstreamer-1.0-0.dll");
            RuntimeDependencies.Add("gstvideo-1.0-0.dll");
            RuntimeDependencies.Add("gstapp-1.0-0.dll");
            RuntimeDependencies.Add("gstrtspserver-1.0-0.dll");
        }
        System.Console.WriteLine("********************************************************************************************************");
    }
    private int ExecuteCommandSync(string command)
    {
        System.Console.WriteLine("Running: " + command);
        var processInfo = new ProcessStartInfo("cmd.exe", "/c " + command)
        {
            CreateNoWindow = true,
            UseShellExecute = false,
            RedirectStandardError = true,
            RedirectStandardOutput = true,
            WorkingDirectory = ModulePath
        };

        StringBuilder sb = new StringBuilder();
        Process p = Process.Start(processInfo);
        p.OutputDataReceived += (sender, args) => System.Console.WriteLine(args.Data);
        p.BeginOutputReadLine();
        p.WaitForExit();

        return p.ExitCode;
    }

}

