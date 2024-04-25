#!/bin/bash

# Check if the correct number of arguments was passed
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <image-name> <registry-url> <access-token>"
    exit 1
fi

# Assign the first, second, and third arguments to meaningful variable names
IMAGE="$1"
REGISTRY="$2"
TOKEN="$3"

# Get a list of all tags for the image
RESPONSE=$(curl -s -H "Authorization: Bearer ${TOKEN}" https://${REGISTRY}/v2/${IMAGE}/tags/list)

echo https://${REGISTRY}/v2/${IMAGE}/tags/list

# Check if the tags were retrieved successfully
if [ -z "$RESPONSE" ]; then
    echo "Error: Failed to retrieve the tags."
    exit 1
fi

echo $RESPONSE 

# Output the list of tags for the image
echo $RESPONSE | jq -r '.tags[]'