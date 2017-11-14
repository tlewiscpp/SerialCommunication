
#!/bin/bash

##########################################
# install.sh
#
# Intended to be a placeholder until 
# I learn how to add a sudo make 
# install/uninstall to CMake
#
##########################################

baseName="TRMIDoubleCheckBox"
programLongName="TRMIDoubleCheckBox"
programName="TRMIDoubleCheckBox"
iconName="$baseName.png"
skeletonDesktopFileName=".$baseName.desktop.skel"
desktopFileName="$baseName.desktop"

absolutePath="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"
fileName="${absolutePath##*/}"
filePath="$(dirname "$absolutePath")/"
sourceDir="$filePath/inspectionapp/src/"
utilityDir="$filePath/inspection-app/utility/"
resourceDir="$filePath/inspection-app/resources/"
iconPath="$resourceDir/$iconName"
globalBinDir="/usr/bin/"
buildType="Release"

function bailout() {
    rm -rf "$buildDir"
}

function displayHelp() {
    echo "Usage: install-$programName.sh [--install/--uninstall] [build-dir]"
}

function doInternetCheck() {

    echo -n "Checking for Cygwin..."
    cygwinCheck=$(uname -a | grep -i 'cygwin')
    showSuccess

    echo -n "Checking for internet connection..."
    if [[ -z "$cygwinCheck" ]]; then
        internetCheck=$(ping 8.8.8.8 -n1 | grep -i 'received = 1')
    else
        internetCheck=$(ping 8.8.8.8 -c1 -W1 | grep -i '1 received')
    fi

    if [[ -z "$internetCheck" ]]; then
        showFailure
        return 1
    else 
        showSuccess
        return 0
    fi
}

function doCygwinCheck() {
    echo -n "Checking for Cygwin..."
    cygwinCheck=$(uname -a | grep -i 'cygwin')
    msysCheck=$(uname -a | grep -i 'msys')
    showSuccess

    echo -n "Setting sudo to correct value..."
    if [[ -z "$cygwinCheck" && -z "$msysCheck" ]]; then
        SUDO="sudo"
    else
        SUDO=""
    fi
    showSuccess
}

function cleanUp() {
    echo "All cleaned up"
}

function showSuccess() {
    echo "success"
}

function showFailure() {
    echo "failure"
    cleanUp
}

function removeFile() {
    echo -n "Removing \"$1\"..."
    rm -f "$1"
    if [[ "$?" -ne "0" ]]; then
        showFailure
        return 1
    else 
        showSuccess
        return 0
    fi
}

function linkFile() {
    echo -n "Linking \"$1\" to \"$2\"..."
    ln -s -f "$1" "$2"
    if [[ "$?" -ne "0" ]]; then
        showFailure
        return 1
    else 
        showSuccess
        return 0
    fi
}

function suLinkFile() {
    echo -n "Linking \"$1\" to \"$2\"..."
    $SUDO ln -s -f "$1" "$2"
    if [[ "$?" -ne "0" ]]; then
        showFailure
        return 1
    else 
        showSuccess
        return 0
    fi
}

function copyFile() {
    echo -n "Copying \"$1\" to \"$2\"..."
    cp -R "$1" "$2"
    if [[ "$?" -ne "0" ]]; then
        showFailure
        return 1
    else 
        showSuccess
        return 0
    fi
}

function suCopyFile() {
    echo -n "Copying \"$1\" to \"$2\"..."
    $SUDO cp -R "$1" "$2"
    if [[ "$?" -ne "0" ]]; then
        showFailure
        return 1
    else
        showSuccess
        return 0
    fi
}

function appendStringToFile() {
    echo -n "Putting string \"$1\" into file $2..."
    echo "$1" >> "$2"
    if [[ "$?" -ne "0" ]]; then
        showFailure
        return 1
    else 
        showSuccess
        return 0
    fi
}

function changeDirectory() {
    echo -n "Entering directory \"$1\"..."
    cd "$1"
    if [[ "$?" -ne "0" ]]; then
        showFailure
        return 1
    else 
        showSuccess
        return 0
    fi
}

function createDirectory() {
    echo -n "Creating directory \"$1\"..."
    mkdir -p "$1"
    if [[ "$?" -ne "0" ]]; then
        showFailure
        return 1
    else 
        showSuccess
        return 0
    fi
}

function runGitClone() {
    echo -n "Cloning \"$1\" using git..."
    git clone "$1"
    if [[ "$?" -ne "0" ]]; then
        showFailure
        return 1
    else 
        showSuccess
        return 0
    fi
}

function runGitPullOrigin() {
    echo -n "Fetching source for \"$1\" using git..."
    git pull origin master
    if [[ "$?" -ne "0" ]]; then
        showFailure
        return 1
    else 
        showSuccess
        return 0
    fi
}

function runCmake() {
    echo "Running cmake (BuildType = $buildType) from source directory \"$1\""
    declare -a incrementArray
    somethingIncremented=0
    if [[ "$incrementMajorVersion" == 1 ]]; then
        incrementArray[${#incrementArray[@]}]="-DINCREMENT_MAJOR_VERSION=1"
        somethingIncremented=1
    else
        incrementArray[${#incrementArray[@]}]="-DINCREMENT_MAJOR_VERSION=0"
    fi
    if [[ "$incrementMinorVersion" == 1 ]]; then
        incrementArray[${#incrementArray[@]}]="-DINCREMENT_MINOR_VERSION=1"
        somethingIncremented=1
    else
        incrementArray[${#incrementArray[@]}]="-DINCREMENT_MINOR_VERSION=0"
    fi
    if [[ "$incrementPatchVersion" == 1 ]]; then
        incrementArray[${#incrementArray[@]}]="-DINCREMENT_PATCH_VERSION=1"
        somethingIncremented=1
    else
        incrementArray[${#incrementArray[@]}]="-DINCREMENT_PATCH_VERSION=0"
    fi
    if [[ "$somethingIncremented" == 0 ]]; then
        incrementArray[0]="-DINCREMENT_MAJOR_VERSION=0"
        incrementArray[1]="-DINCREMENT_MINOR_VERSION=0"
        incrementArray[2]="-DINCREMENT_PATCH_VERSION=0"
    fi
    incrementArgs=""
    for var in "${incrementArray[@]}"; do
        incrementArgs="$incrementArgs $var"
    done
    if [[ "$buildAndroid" == "1" ]]; then
        androidBuildArgs="-DANDROID_NDK=/usr/local/android-sdk/ndk-bundle/ -DANDROID_SDK=/usr/local/android-sdk/ -DJAVA_HOME=/usr/lib/jvm/java-8-openjdk/ -DANDROID_NATIVE_API_LEVEL=android-21 -DCMAKE_TOOLCHAIN_FILE=$filePath/tools/qt-android-cmake/toolchain/android.toolchain.cmake"
    else
        androidBuildArgs=""
    fi
    if [[ "$verboseOutput" == "1" ]]; then
        verboseOutputArgs="-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON"
    else
        verboseOutputArgs=""
    fi
    msysCheck=$(uname -a | grep -i 'msys')
    if [[ ! -z "$msysCheck" ]]; then
        mingwRoot="/c/Qt/Tools/mingw530_32/bin"
        isMSYS="-DCMAKE_MAKE_PROGRAM=${mingwRoot}/mingw32-make.exe -DCMAKE_C_COMPILER=${mingwRoot}/gcc.exe -DCMAKE_CXX_COMPILER=${mingwRoot}/g++.exe -DMSYS_PLATFORM:BOOL=ON"
    else
        isMSYS=""
    fi
    echo -n "Running command: \"cmake $incrementArgs $verboseOutputArgs $isMSYS $androidBuildArgs -DCMAKE_BUILD_TYPE=$buildType \"$1\"..." 
    cmake $incrementArgs $verboseOutputArgs $isMSYS $androidBuildArgs -DCMAKE_BUILD_TYPE=$buildType "$1" 
    if [[ "$?" -ne "0" ]]; then
        showFailure
        return 1
    else 
        showSuccess
        return 0
    fi
}

function runQmake() {
    echo -n "Running qmake from source directory \"$1\"..."
    qmake "$1"
    if [[ "$?" -ne "0" ]]; then
        showFailure
        return 1
    else 
        showSuccess
        return 0
    fi
}

function runMake() {
    echo -n "Running make..."
    make -j2
    if [[ "$?" -ne "0" ]]; then
        showFailure
        return 1
    else 
        showSuccess
        return 0
    fi
}

function bailout() {
    rm -rf "$buildDir"
}

function generateDesktopFile() {
    copyFile "$utilityDir/$skeletonDesktopFileName" "$buildDir/$desktopFileName" || { echo "Failed to generate desktop file, bailing out"; exit 1; }
    copyFile "$iconPath" "$buildDir/" || { echo "Failed to generate desktop file, bailing out"; exit 1; }  
    appendStringToFile "Exec=$buildDir/inspection-app/$programName" "$buildDir/$desktopFileName" || { echo "Failed to generate desktop file, bailing out"; exit 1; }
    appendStringToFile "Icon=$buildDir/$iconName" "$buildDir/$desktopFileName" || { echo "Failed to generate desktop file, bailing out"; exit 1; }
}


buildDir="build"
appDir="$HOME/.local/share/applications/"

if [[ "$EUID" -eq "0" ]]; then
    echo "This script must NOT be run as root"
    exit 1
    SUDO=
else
    SUDO=sudo
fi

trap bailout INT QUIT TERM

declare -i incrementPatchVersion
declare -i incrementMinorVersion
declare -i incrementMajorVersion

incrementPatchVersion=0
incrementMinorVersion=0
incrementMajorVersion=0
buildAndroid=0
verboseOutput=0

declare -i loopCounter
loopCounter=0
for var in "$@"; do
    if [[ "$var" == "-r" || "$var" == "--r" || "$var" == "-release" || "$var" == "--release" ]]; then
        buildType="Release"
    elif [[ "$var" == "-d" || "$var" == "--d" || "$var" == "-debug" || "$var" == "--debug" ]]; then
        buildType="Debug"
    elif [[ "$var" == "-p" || "$var" == "--p" || "$var" == "-patch" || "$var" == "--patch" || "$var" == "-increment-patch" || "$var" == "--increment-patch" ]]; then
        incrementPatchVersion=1
    elif [[ "$var" == "-i" || "$var" == "--i" || "$var" == "-minor" || "$var" == "--minor" || "$var" == "-increment-minor" || "$var" == "--increment-minor" ]]; then
        incrementMinorVersion=1
    elif [[ "$var" == "-m" || "$var" == "--m" || "$var" == "-major" || "$var" == "--major" || "$var" == "-increment-major" || "$var" == "--increment-major" ]]; then
        incrementMajorVersion=1
    elif [[ "$var" == "-a" || "$var" == "--a" || "$var" == "-android" || "$var" == "--android" ]]; then
        buildAndroid=1
    elif [[ "$var" == "-v" || "$var" == "--v" || "$var" == "-verbose" || "$var" == "--verbose" ]]; then
        verboseOutput=1
    fi
    loopCounter=$((loopCounter+1))
done

if [[ $# -gt 0 ]]; then 
    var=""
    buildDir="$filePath/$buildDir"
    for var in "$@"; do
        if [[ $var == -* ]]; then
            continue
        fi
        buildDir="$var"
    done
else
    buildDir="$filePath/$buildDir"
fi
if ! [[ -d "$buildDir" ]]; then
    createDirectory "$buildDir" || { echo "Unable to make build directory \"$buildDir\", exiting"; exit 1; }
fi

changeDirectory "$buildDir" || { echo "Unable to enter build directory \"$buildDir\", bailing out"; exit 1; }
runCmake "$filePath" || { echo "cmake failed, bailing out"; exit 1; }
runMake || { echo "make failed, bailing out"; exit 1; }
generateDesktopFile || { echo "Could not generate desktop file, bailing out"; exit 1; }
createDirectory "$appDir" || { echo "Could not create directory \"$appDir\", bailing out"; exit 1; }
copyFile "$buildDir/$desktopFileName" "$appDir" || { echo "Could not copy desktop file to application directory, bailing out"; exit 1; }
suLinkFile "$buildDir/inspection-app/$programName" "$globalBinDir"  || { echo "Could not link file, bailing out"; exit 1; }
createDirectory "$HOME/Desktop" || { echo "Could not create directory \"$HOME/Desktop\", bailing out"; exit 1; }
copyFile "$buildDir/$desktopFileName" "$HOME/Desktop/"
$SUDO chmod +x "$HOME/Desktop/" || { echo "Could not mark desktop file executable, bailing out"; exit 1; }

installMessage="$programLongName Installed Successfully!"
totalLength=${#installMessage} 

echo
for (( i=0; i<totalLength+4; i++ )); do  
   echo -n "*"
done
echo
echo "**$installMessage**"
for (( i=0; i<totalLength+4; i++ )); do  
   echo -n "*"
done
echo
echo
exit 0
