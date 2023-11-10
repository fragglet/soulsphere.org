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

print_progress() {
    echo -n "[" $((commit_index + 1)) "/" ${#commits[@]} "] "
}

# Save current commit_index to navigate_history array.
push_navigate_history() {
    local arrlen=${#navigate_history[@]}
    navigate_history[$arrlen]=$commit_index
    navigate_future=()
}

scan() {
    local arg="$1"
    local direction="$2"
    local new_index=$((commit_index + direction))

    if [[ "$arg" = "" ]]; then
        push_navigate_history
        commit_index=$new_index
        return 0
    fi

    while [[ $new_index -ge 0 ]] &&
          [[ $new_index -lt ${#commits[@]} ]]; do
        commit=${commits[$new_index]}
        if git show "$commit" | grep -iq "$arg"; then
            echo "                                                      "
            push_navigate_history
            commit_index=$new_index
            return 0
        fi
        printf "Searching: commit $commit\r"
        new_index=$((new_index + direction))
    done

    echo "                                                      "
    echo "Failed to find '$arg'"
    return 1
}

debug_print_history_stacks() {
    echo "${navigate_history[@]} < $commit_index > ${navigate_future[@]}"
}

navigate_back() {
    local history_len=${#navigate_history[@]}
    if [[ $history_len -lt 1 ]]; then
        echo "Beginning of history; nothing to go back to."
        return 1
    fi
    # Push to future stack, and...
    local future_len=${#navigate_future[@]}
    navigate_future[$future_len]=$commit_index
    # Pop from history stack:
    local history_top=$((history_len - 1))
    commit_index=${navigate_history[$history_top]}
    unset navigate_history[$history_top]
    return 0
}

navigate_forward() {
    local future_len=${#navigate_future[@]}
    if [[ $future_len -lt 1 ]]; then
        echo "Nothing to go forward to."
        return 1
    fi
    # Push to history stack, and...
    local history_len=${#navigate_history[@]}
    navigate_history[$history_len]=$commit_index
    # Pop from future stack:
    local future_top=$((future_len - 1))
    commit_index=${navigate_future[$future_top]}
    unset navigate_future[$future_top]
    return 0
}

conflict_sources() {
    local commit_id="$1"
    git show $commit_id --name-only --pretty=format: | {
        filenames=()
        i=0
        while read filename; do
            filenames[$i]="$filename"
        done

        git log --pretty="format:%h %s" "$commit_id"^ ^HEAD \
                -- "${filenames[@]}"
    }
}

show_help() {
    echo " y           - Cherry pick this commit and move to next"
    echo " d           - Show full diff"
    echo " n [needle]  - Next; scan forward to find commit"
    echo " p [needle]  - Previous; Scan backward to find commit"
    echo " c           - List previous commits possibly causing conflict"
    echo " b           - Go back to last commit"
    echo " f           - Go forward"
    echo " q           - Quit"
}

read_commit_ids "$1"

# Commits previously moved (by commit index):
navigate_history=()

# Commits get saved here when we navigate back:
navigate_future=()

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
        print_progress
        read -p "Cherrypick? [y/n/p/d/c/q] " response arg
        case $response in
            y)
                try_cherrypick $commit
                commit_index=$((commit_index+1))
                break
                ;;
            p)
                if scan "$arg" -1; then
                    break
                fi
                ;;
            n)
                if scan "$arg" 1; then
                    break
                fi
                ;;
            b)
                if navigate_back; then
                    break
                fi
                ;;
            f)
                if navigate_forward; then
                    break
                fi
                ;;
            d)
                git show $commit
                ;;
            q)
                exit
                ;;
            c)
                conflict_sources $commit
                ;;
            \?)
                show_help
                ;;
        esac
    done
done

