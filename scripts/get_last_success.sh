#!/bin/bash

get_last_success() {
    local branch=$1
    local repo=$2

    commits=$(gh api \
        -H "Accept: application/vnd.github+json" \
        -H "Authorization: token ${GH_TOKEN}" \
        "/repos/${repo}/commits?sha=${branch}&per_page=100" \
        --jq '.[].sha' 2>/dev/null)

    if [ $? -ne 0 ] || [ -z "$commits" ]; then
        exit 1
    fi

    for commit_sha in $commits; do
        check_runs=$(gh api \
            -H "Accept: application/vnd.github+json" \
            -H "Authorization: token ${GH_TOKEN}" \
            "/repos/${repo}/commits/${commit_sha}/check-runs" \
            --jq '.check_runs[] | {conclusion: .conclusion, name: .name}' 2>/dev/null)

        echo $check_runs

        if [ -n "$check_runs" ]; then
            failed_runs=$(echo "$check_runs" | jq -r 'select(.conclusion != "success") | .name')
            if [ -z "$failed_runs" ]; then
                echo "${commit_sha}"
                exit 0
            fi
        fi
    done

    exit 1
}

main() {
    local branch=${1:-$(gh api /repos/${GITHUB_REPOSITORY}/default_branch --jq .name 2>/dev/null)}
    local repo=${2:-$GITHUB_REPOSITORY}

    if [ -z "$branch" ] || [ -z "$repo" ]; then
        exit 1
    fi

    get_last_success "$branch" "$repo"
}

main "$@"