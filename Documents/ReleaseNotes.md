# NanaGet Release Notes

**NanaGet 1.3 (1.3.1390.0)**

- Introduce the portable mode support.
- Use x-generate to define language resources in AppX manifest to solve the
  Windows AppX language default fallback issue feedback by GiorgioUrbani.
- Defer package updates while the app is running in Windows 11 23H2 or later.
  (Suggested by AndromedaMelody.)
- Use Mile.Windows.UniCrt instead of VC-LTL.
- Migrate from Mile.Project.Windows to Mile.Project.Configurations.
- Update Mile.Windows.Helpers to 1.0.1171.
- Update Mile.Xaml to 2.5.1250.
- Update Mile.Json to 1.0.1057.
- Update Mile.Aria2 to 1.0.1050.
- Update Mile.Project.Helpers to 1.0.975.
- Update .NET projects to .NET 10.
- Migrate solution from sln to slnx.
- Improve several implementations and fix several issues.

**NanaGet 1.2 (1.2.789.0)**

- Fix no label issue in Tile mode when using Windows 10's Start. (Thanks to
  hl2guide.)
- Continue to fix "Layout cycle detected. Layout could not complete." issue.
- Remove 32-bit x86 support because supported 32-bit x86 Windows versions don't
  support running on 32-bit only x86 processors.
- Update Mile.Xaml to 2.1.661.
- Update Mile.Aria2 to 1.0.230.
- Use WINRT_NO_SOURCE_LOCATION to reduce the binary size.
- Use WTL NuGet package instead of ThirdParty\WindowsTemplateLibrary project.
- Update Mile.Windows.Helpers to 1.0.534.
- Use Mile.Json to 1.0.659 instead of JSON for Modern C++.
- Backport the documentation changes from https://github.com/M2Team/NanaZip/pull/435.
  (Thanks to sirredbeard.)
- Ignore the text scale factor for solving the UI layout issues like
  https://github.com/M2Team/NanaBox/issues/10. (Feedbacked by prakharb5.)
- Improve maintainer tools for introducing automatic packaging support when
  building all targets for NanaGet.
- Improve several implementations.

**NanaGet 1.1 (1.1.400.0)**

- Add German translation. (Contributed by uDEV2019.)
- Add firewall rules for  in AppX manifest. (Suggested by
  AndromedaMelody.)
- Add Traditional Chinese translation. (Contributed by SakuraNeko.)
- Add Japanese translation. (Contributed by hez2010.)
- Refresh application and file type icons. (Designed by Shomnipotence.)
- Update the minimum system requirement to Windows 10 Version 2004 (Build
  19041) or later for solving issues in the XAML Islands.
- Fix twice initialization issue. (Thanks to driver1998.)
- Use NuGet package version of Mile.Aria2 instead of Aria2 mainline for getting
  smaller binary size and full native ARM64 support.
- Use NuGet package version of Mile.Xaml and update to 2.0.616.
- Refresh the UI with Windows 11 XAML control styles and the immersive Mica 
  effects.
- Migrate implementation from Windows.Data.Json to JSON for Modern C++.
- Use Git submodule version of Mile.Project.Windows.
- Add Mile.Windows.Helpers 1.0.15.
- Use separate modal dialogs instead of XAML content dialogs.
- Show error dialog when Windows Runtime XAML meeting the unhandled exception.
- Improve the save download session support.

**NanaGet 1.0 (1.0.36.0)**

- Use aria2 as transfer engine.
- Add English and Simplified Chinese support.
- Packaging with MSIX for modern deployment experience.
- Modernize the UI with XAML Islands with dark and light mode support. (Dynamic
  dark and light mode switching is supported for Windows 11 and later.)
- Add full High DPI and partial Accessibility support.
