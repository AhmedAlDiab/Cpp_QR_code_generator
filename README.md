# QR_code_generator
## Download
Prebuilt executables are available under [**Releases**](https://github.com/AhmedAlDiab/Cpp_QR_code_generator/releases).
v1 - [Download](https://github.com/AhmedAlDiab/Cpp_QR_code_generator/releases/download/v1.0.0/QR_code_generator.exe)

## Building the Project

This project uses **Conan** to manage dependencies.

### 1. Install Conan

You need **Conan v2** installed on your system.

#### Using pip (Windows / Linux / macOS)
```bash
pip install conan
```

#### Using pipx (recommended on Linux)
```bash
pipx install conan
```

Verify installation:
```bash
conan --version
```

---

### 2. Install Dependencies

Make your default profile for the compiler you are using (e.g., Visual Studio, GCC, Clang). You can do this by running:
```bash
conan profile detect --force
```

From the project root directory (for debug mode):
```bash
conan install . -s build_type=Release -s arch=x86_64 -of=conan/x64/rel --build=missing --build=wxwidgets/*
cmake --preset x64-release
cmake --build out/build/x64-release
```

From the project root directory (for release mode):
```bash
conan install . -sbuild_type=Release -of=conan/rel --build=missing
cmake --preset x64-release
cmake --build out/build/x64-release
```

*Same idea for x86 presets.*
(Honstly I can't get it work with x86 arch)

This will download and configure all required dependencies.

# I used the following libraries:
- **qrcodegen**: For generating QR codes. [Link to Project](https://github.com/nayuki/QR-Code-generator/tree/master)
- **wxWidgets** For GUI [Link to Project](https://www.wxwidgets.org/)

obviusly I used **C++** as the programming language for this project.
"I know that my code sucks thanks 👍"