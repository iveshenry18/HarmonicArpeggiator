#!/bin/sh

echo "Setting environment variables"

export NOTARIZATION_USERNAME=<your apple id>
export NOTARIZATION_PASSWORD=<your app-specific password>
export NOTARIZATION_TEAM_ID=<your team id>
export DEVELOPER_ID_APPLICATION=<the Common Name of your Developer ID Application certificate>

echo "Environment variables set"