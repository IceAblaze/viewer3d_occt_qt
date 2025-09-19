param([string]$VcpkgRoot="C:\dev\vcpkg",[string]$Config="Release",[string]$BuildDir="build")
$bin = Join-Path $BuildDir $Config
$exe = Join-Path $bin "Viewer3D_OCCT_Qt.exe"
$windeploy = Join-Path $VcpkgRoot "installed\x64-windows\tools\qt6\bin\windeployqt.exe"
& $windeploy $exe
Copy-Item (Join-Path $VcpkgRoot "installed\x64-windows\bin\TK*.dll") $bin -Force -ErrorAction SilentlyContinue
Compress-Archive -Path $bin\* -DestinationPath (Join-Path $BuildDir "Viewer3D_OCCT_Qt-portable.zip") -Force
