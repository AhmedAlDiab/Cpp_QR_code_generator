# Cpp_QR_code_generator

## Download

Prebuilt executables are available under [**Releases**](https://github.com/AhmedAlDiab/Cpp_QR_code_generator/releases).

- v1 - [Download](https://github.com/AhmedAlDiab/Cpp_QR_code_generator/releases/download/v1.0.0/QR_code_generator.exe)

## Building the Project

This project uses **Conan** to manage dependencies.

### On Visual Studio

- Download Conan Extension for Visual Studio [MarketPlace](https://marketplace.visualstudio.com/items?itemName=conan-io.conan-vs-extension)
- Download the wxwidgets library from the extension
- run this command before building:

    ```bash
    rmdir /s /q conan 2>nul
    conan profile detect --force
    conan install . -of=conan --build=missing
    ```

- finally you can build it to any arch. you want (x64 , x86) on release or debug.

# I used the following libraries

- **qrcodegen**: For generating QR codes. [Link to Project](https://github.com/nayuki/QR-Code-generator/tree/master)
- **wxWidgets** For GUI [Link to Project](https://www.wxwidgets.org/)

Obviously I used **C++** as the programming language for this project.
"I know that my code sucks thanks 👍"
