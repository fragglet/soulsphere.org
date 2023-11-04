#!/bin/bash

if [ $# -lt 1 ]; then
    echo "Usage: $0 <commit...commit>"
    exit 1
fi

reset_workspace() {
    git reset --hard HEAD >/dev/null
    git restore -S . >/dev/null
    git restore . >/dev/null
}

try_cherrypick() {
    local commit=$1

    reset_workspace
    if ! git cherry-pick -x $commit; then
        echo "Cherrypick failed."
        reset_workspace
    fi
}

check_if_applies() {
    local commit=$1

    reset_workspace
    if git cherry-pick -n $commit > /dev/null; then
        reset_workspace
        return 0
    else
        reset_workspace
        return 1
    fi
}

read_commit_ids() {
    commits=()
    local i=0
    for commit in $(git log --reverse --pretty="%H" $1 "^HEAD"); do
        commits[$i]=$commit
        i=$((i + 1))
    done
    echo ${#commits[@]} commits
}

scan() {
    local arg="$1"
    local direction="$2"
    local new_index=$((commit_index + direction))

    if [[ "$arg" = "" ]]; then
        commit_index=$new_index
        return 0
    fi

    while [[ $new_index -ge 0 ]] &&
          [[ $new_index -lt ${#commits[@]} ]]; do
        commit=${commits[$new_index]}
        if git show "$commit" | grep -iq "$arg"; then
            commit_index=$new_index
            return 0
        fi
        printf "Searching: commit $commit\r"
        new_index=$((new_index + direction))
    done

    echo "Failed to find '$arg'"
    return 1
}

read_commit_ids "$1"

commit_index=0
while [[ $commit_index -lt ${#commits[@]} ]]; do
    commit=${commits[$commit_index]}
    echo
    git --no-pager show --compact-summary $commit
    echo
    if check_if_applies $commit; then
        echo "Applies cleanly."
    else
        echo "Does not apply cleanly."
    fi
    while true; do
        arg=
        read -p "Cherrypick? [y/n/d/q] " -u 5 response arg
        case $response in
            y)
                try_cherrypick $commit
                commit_index=$((commit_index+1))
                break
                ;;
            b)
                scan "$arg" -1
                break
                ;;
            n)
                scan "$arg" 1
                break
                ;;
            d)
                git show $commit
                ;;
            q)
                exit
                ;;
        esac
    done
done

