param([string]$VcpkgRoot="C:\dev\vcpkg",[string]$Config="Release",[string]$BuildDir="build")
$toolchain = Join-Path $VcpkgRoot "scripts\buildsystems\vcpkg.cmake"
cmake -S . -B $BuildDir -DCMAKE_TOOLCHAIN_FILE=$toolchain -DCMAKE_BUILD_TYPE=$Config
cmake --build $BuildDir --config $Config
