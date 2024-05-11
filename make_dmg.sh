#!/bin/sh

set -e

while getopts "v:n" opt; do
  case $opt in
    n) NOTARIZE="true"
    ;;
    v) VERSION=$OPTARG
    ;;
    \?) echo "Invalid option -$OPTARG" >&2
    ;;
  esac
done

if [ -z "$VERSION" ]; then
  echo "Usage: make_dmg.sh -v VERSION [-n]"
  exit 1
fi

# TODO: Manage this script from CMakelists.txt to auto-fill these variables
BUNDLE_ID="com.henryives.harmonicarpeggiator"
ARTIFACTS_PATH=cmake-build-release/HarmonicArpeggiator_artefacts/Release
PRODUCT_NAME="Harmonic Arpeggiator"
VST3_PATH=$ARTIFACTS_PATH/VST3/$PRODUCT_NAME.vst3
AU_PATH=$ARTIFACTS_PATH/AU/$PRODUCT_NAME.component
AUV3_PATH=$ARTIFACTS_PATH/AUv3/$PRODUCT_NAME.appex
STANDALONE_PATH=$ARTIFACTS_PATH/Standalone/$PRODUCT_NAME.app
ARTIFACT_NAME=$PRODUCT_NAME-$VERSION-macOS

echo "Making DMG for $PRODUCT_NAME version $VERSION"
echo "Using the following settings:"
echo "DEVELOPER_ID_APPLICATION=$DEVELOPER_ID_APPLICATION"
echo "NOTARIZATION_USERNAME=$NOTARIZATION_USERNAME"
echo "NOTARIZATION_PASSWORD=$NOTARIZATION_PASSWORD"
echo "NOTARIZATION_TEAM_ID=$NOTARIZATION_TEAM_ID"
echo "BUNDLE_ID=$BUNDLE_ID"
echo "VERSION=$VERSION"
echo "VST3_PATH=$VST3_PATH"
echo "AU_PATH=$AU_PATH"
echo "AUV3_PATH=$AUV3_PATH"
echo "STANDALONE_PATH=$STANDALONE_PATH"
echo "ARTIFACT_NAME=$ARTIFACT_NAME"
echo "NOTARIZE=$NOTARIZE"

# Each plugin must be code signed
echo "Code signing $PRODUCT_NAME plugins"
codesign --force -s "$DEVELOPER_ID_APPLICATION" -v "$VST3_PATH" --deep --strict --options=runtime --timestamp
codesign --force -s "$DEVELOPER_ID_APPLICATION" -v "$AU_PATH" --deep --strict --options=runtime --timestamp
codesign --force -s "$DEVELOPER_ID_APPLICATION" -v "$STANDALONE_PATH" --deep --strict --options=runtime --timestamp

# Check if appdmg is installed
if ! command -v appdmg &> /dev/null
then
    echo "Installing appdmg"
    # workaround for https://github.com/LinusU/node-appdmg/issues/234
    python3 -m pip install setuptools
    npm install -g appdmg
fi
mkdir -p packaging/dmg
mkdir -p packaging/out

# Create directories for the dmg symlinks
echo "Creating symlinks for the DMG"
sudo mkdir -m 755 -p /Library/Audio/Plug-Ins/Components && sudo mkdir -m 755 -p /Library/Audio/Plug-Ins/VST3
ln -s /Library/Audio/Plug-Ins/Components "packaging/dmg/Your Mac's Component folder" || echo "Component symlink already exists"
ln -s /Library/Audio/Plug-Ins/VST3 "packaging/dmg/Your Mac's VST3 folder" || echo "VST3 symlink already exists"
cp -r  "$VST3_PATH" packaging/dmg
cp -r "$AU_PATH" packaging/dmg
cp -r "$STANDALONE_PATH" packaging/dmg

# Run appdmg to create the .dmg
echo "Creating the DMG"
if [ -f "packaging/out/$ARTIFACT_NAME.dmg" ]; then
  echo "Removing existing DMG file"
  rm "packaging/out/$ARTIFACT_NAME.dmg"
fi
cd packaging && appdmg dmg.json "out/$ARTIFACT_NAME.dmg"
codesign -s "$DEVELOPER_ID_APPLICATION" --timestamp -i "$BUNDLE_ID" --force "out/$ARTIFACT_NAME.dmg"

if [ "$NOTARIZE" = "true" ]; then
  if [ -z "$NOTARIZATION_USERNAME" ] || [ -z "$NOTARIZATION_PASSWORD" ] || [ -z "$NOTARIZATION_TEAM_ID" ]; then
    echo "Please set the NOTARIZATION_USERNAME, NOTARIZATION_PASSWORD, and NOTARIZATION_TEAM_ID environment variables"
    exit 1
  else
    echo "Notarizing the DMG with username $NOTARIZATION_USERNAME and team ID $NOTARIZATION_TEAM_ID"
    xcrun notarytool submit "out/$ARTIFACT_NAME.dmg" --apple-id "$NOTARIZATION_USERNAME" --password "$NOTARIZATION_PASSWORD" --team-id "$NOTARIZATION_TEAM_ID" --wait
    xcrun stapler staple "out/$ARTIFACT_NAME.dmg"
  fi
else
  echo "Skipping notarization"
fi
