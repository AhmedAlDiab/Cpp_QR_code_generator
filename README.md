# QR_code_generator
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

From the project root directory:
```bash
conan profile detect --force
conan install . --output-folder=build --build=missing
```

This will download and configure all required dependencies.
