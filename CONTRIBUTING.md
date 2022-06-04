﻿## Contributing to NanaGet

### How to become a contributor

- Direct contributions
  - **Create pull requests directly.**
  - Please send e-mails to Mouri_Naruto@Outlook.com if you have any
    questions.
- Feedback suggestions and bugs.
  - We use GitHub issues to track bugs and features.
  - For bugs and general issues please 
    [file a new issue](https://github.com/M2Team/NanaGet/issues/new).

### Code contribution guidelines

#### Prerequisites

- Visual Studio 2019 Version 16.11 or later.
  - You also need install ARM64 components (MSVC Toolchain and ATL/MFC) if you
    want to compile ARM64 version of NanaGet.
- Windows 10 Version 20H1 SDK or later.
  - You also need install ARM64 components if you want to compile ARM64 version
    of NanaGet.

#### How to build all targets of NanaGet

Run `BuildAllTargets.bat` in the root of the repository.

#### How to modify or debugging NanaGet

Open `NanaGet.sln` in the root of the repository.

#### Code style and conventions

- C++: [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md)
- C#: Follow the .NET Core team's [C# coding style](https://github.com/dotnet/corefx/blob/master/Documentation/coding-guidelines/coding-style.md)

For all languages respect the [.editorconfig](https://editorconfig.org/) file 
specified in the source tree. Many IDEs natively support this or can with a 
plugin.

### Translation contribution notice

All `comment` in `resw` files should be kept English for make it better to 
maintenance in the future.

For new one to contribute translation, here is a good example:
https://github.com/M2Team/NanaGet/pull/2.
