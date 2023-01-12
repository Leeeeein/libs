#!/bin/bash

# Check if git is installed
if ! [ -x "$(command -v git)" ]; then
  echo 'Error: git is not installed.' >&2
  exit 1
fi

# Check if the current directory is a git repository
if ! [ -d .git ]; then
  echo 'Error: the current directory is not a git repository.' >&2
  exit 1
fi

# Add all changes to the staging area
git add .

# Commit the changes with a message
read -p "Enter commit message: " commitMessage
git commit -m "$commitMessage"

# Check if the repository is already associated with a remote
if ! git remote show | grep -q "origin"; then
  # If not, ask for the repository URL and set it as the origin
  read -p "Enter repository URL: " repositoryURL
  git remote add origin "$repositoryURL"
fi

# Push the changes to the origin
git push -u origin master
