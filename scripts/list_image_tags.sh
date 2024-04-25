#!/bin/bash

# Check if the correct number of arguments was passed
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <registry-url> <image-name> <access-token>"
    exit 1
fi

# Assign the first, second, and third arguments to meaningful variable names
REGISTRY="$1"
IMAGE="$2"
TOKEN="$3"

# Get a list of all tags for the image
RESPONSE=$(curl -s -H "Authorization: Bearer $(echo ${TOKEN} | base64)" https://${REGISTRY}/v2/${IMAGE}/tags/list)

# Check if the tags were retrieved successfully
if [ -z "$RESPONSE" ]; then
    echo "Error: Failed to retrieve the tags."
    exit 1
fi

# Output the list of tags for the image
echo $RESPONSE | jq -r '.tags | join(" ")'