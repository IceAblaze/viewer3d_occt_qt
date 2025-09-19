# Viewer 3D (C++ + OpenCascade + Qt)

Visualizzatore 3D parametrico che legge un file JSON (blocco + fori) e costruisce un solido B-Rep con **OpenCascade**, mostrandolo in una GUI **Qt6** con interazione (rotazione, pan, zoom) e la possibilità di visualizzare i cilindri "tool" dei fori.

---

## Requisiti

- **Qt 6** (open-source; moduli `Widgets`, `OpenGLWidgets`)
- **OpenCascade (OCCT)** recente (7.x)
- **CMake 3.16+**
- Toolchain C++ (MSVC su Windows, GCC/Clang su Linux/macOS)

> Suggerito: usare **vcpkg** per installare OCCT e Qt6 rapidamente.

---

## Setup passo-passo su Windows (MSVC x64)

### 0. Struttura cartelle consigliata

- `C:\dev\vcpkg\` → cartella di vcpkg
- `C:\dev\viewer3d_occt_qt\` → cartella del progetto (quella dello zip)

### 1. Preparazione vcpkg

**DOVE:** `C:\dev\vcpkg\`

```powershell
cd C:\dev\vcpkg
git pull
.ootstrap-vcpkg.bat
.cpkg integrate install
```

### 2. Installazione dipendenze

**DOVE:** `C:\dev\vcpkg\`

```powershell
.cpkg install opencascade:x64-windows `
                 qtbase[widgets,opengl]:x64-windows
```

### 3. Configurazione progetto con CMake

**DOVE:** `C:\dev\viewer3d_occt_qt\`

```powershell
cmake -S . -B build `
  -DCMAKE_TOOLCHAIN_FILE=C:\dev\vcpkg\scripts\buildsystems\vcpkg.cmake `
  -DCMAKE_BUILD_TYPE=Release
```

### 4. Compilazione

**DOVE:** `C:\dev\viewer3d_occt_qt\`

```powershell
cmake --build build --config Release
```

L'eseguibile sarà in:  
`C:\dev\viewer3d_occt_qt\build\Release\Viewer3D_OCCT_Qt.exe`

### 5. Esecuzione

**DOVE:** `C:\dev\viewer3d_occt_qt\build\Release\`

```powershell
.\Viewer3D_OCCT_Qt.exe ..\..\sample_ops.json
```

### 6. Deploy

**Qt DLL** con `windeployqt`:  
```powershell
C:\dev\vcpkg\installed\x64-windows\tools\qt6\bin\windeployqt.exe Viewer3D_OCCT_Qt.exe
```

**DLL OpenCascade** (copiate da `C:\dev\vcpkg\installed\x64-windows\bin\TK*.dll` nella stessa cartella dell'eseguibile):  
```powershell
Copy-Item C:\dev\vcpkg\installed\x64-windows\bin\TK*.dll .\
```

Ora puoi distribuire la cartella `Release\` come app portabile.

---

## Uso con Qt Creator

Apri `CMakeLists.txt` con Qt Creator:

- Kit: Qt6 MSVC x64
- Configurazione CMake:  
  `CMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake`

Build & Run direttamente da Qt Creator.

---

## Varianti

- **MinGW**: usa `:x64-mingw-dynamic` come triplet in vcpkg e un kit MinGW coerente.  
- **QMake**: possibile ma sconsigliato se usi vcpkg; richiede link manuale di tutte le librerie OCCT.

---

## Formato JSON di input

```json
[
  {"length": 200, "width": 100, "thickness": 30},
  {
    "uuid": "H1",
    "surfaceCenter": {"x": 50, "y": 50, "z": 30},
    "diameter": 16,
    "depth": 20,
    "processDirection": {"x": 0, "y": 0, "z": -1}
  }
]
```

---

## WebAssembly (avanzato, facoltativo)

- Installa **Emscripten** e un kit **Qt for WebAssembly**
- Compila **OpenCascade** per WASM (OCCT 7.5+)
- Compila questo progetto con kit WebAssembly di Qt

> Nota: integrazione OCC + Qt in WebAssembly è sperimentale e può richiedere modifiche.
