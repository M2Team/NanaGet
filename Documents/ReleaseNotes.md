# NanaGet Release Notes

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
