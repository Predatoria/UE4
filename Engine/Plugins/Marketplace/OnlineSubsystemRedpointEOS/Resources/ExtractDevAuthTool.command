#!/bin/bash

set -e

echo Extracting Developer Authentication Tool...
echo "Located in $*"
cd "$*"

# Check for corrupted Dev Auth Tool.
if [ -e EOS_DevAuthTool.app ]; then
    if [ ! -e EOS_DevAuthTool.app/Contents/Frameworks/Electron\ Framework.framework/Electron\ Framework ]; then
        # This is a corrupt / badly packaged version of the Dev Auth Tool for macOS. We need to re-extract it to fix it up.
        echo "Detected corrupted Developer Authentication Tool, removing it so we can re-extract it and fix it up..."
        rm -Rf EOS_DevAuthTool.app
    fi
fi

# Extract the Dev Auth Tool.
if [ -e EOS_DevAuthTool-darwin-x64-1.0.1.zip ]; then
    unzip EOS_DevAuthTool-darwin-x64-1.0.1.zip
fi
if [ -e EOS_DevAuthTool-darwin-x64-1.1.0.zip ]; then
    unzip EOS_DevAuthTool-darwin-x64-1.1.0.zip
fi

# Check if we need to fix up a badly packaged Dev Auth Tool.
if [ ! -e EOS_DevAuthTool.app/Contents/Frameworks/Electron\ Framework.framework/Electron\ Framework ]; then
    if [ -e EOS_DevAuthTool.app/Contents/Frameworks/Electron\ Framework.framework/Versions/Current/Electron\ Framework ] && [ -e EOS_DevAuthTool.app/Contents/Frameworks/Squirrel.framework/Versions/Current ] && [ -e EOS_DevAuthTool.app/Contents/Frameworks/ReactiveObjC.framework/Versions/Current ] && [ -e EOS_DevAuthTool.app/Contents/Frameworks/Mantle.framework/Versions/Current ]; then
        pushd EOS_DevAuthTool.app/Contents/Frameworks
        cp -Rv Electron\ Framework.framework/Versions/Current/Electron\ Framework Electron\ Framework.framework/Electron\ Framework
        cp -Rv Squirrel.framework/Versions/Current Electron\ Framework.framework/Libraries/Squirrel.framework
        cp -Rv ReactiveObjC.framework/Versions/Current Electron\ Framework.framework/Libraries/ReactiveObjC.framework
        cp -Rv Mantle.framework/Versions/Current Electron\ Framework.framework/Libraries/Mantle.framework
        popd
    else
        echo "This SDK contains a broken Developer Authentication Tool, but doesn't contain the required frameworks to fix it up. The Developer Authentication Tool won't work, sorry!"
    fi
fi

# Set executable bit after extraction (Finder sets the executable bits, but 'unzip' does not).
chmod -v -R a+rwx EOS_DevAuthTool.app
