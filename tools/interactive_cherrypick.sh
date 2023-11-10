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
    have_search_results=false
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
    local matches=$(git show $commit_id --name-only --pretty=format: | {
        filenames=()
        i=0
        while read filename; do
            filenames[$i]="$filename"
        done

        git log --pretty="format:%H" "$commit_id"^ ^HEAD \
                -- "${filenames[@]}"
    })
    if [[ ! -n "$matches" ]]; then
        echo "No relevant past commits found"
        return 1
    fi
    # Add all matching commits to navigate_future, so that we can step
    # through results.
    navigate_future=()
    have_search_results=true
    local future_len=0
    local i
    while [[ $i -lt $commit_index ]]; do
        if echo "${commits[$i]}" | grep -qw "$matches"; then
            navigate_future[$future_len]=$i
            future_len=$((future_len + 1))
        fi
        i=$((i + 1))
    done
    echo "=== ${#navigate_future[@]} potential conflict commits found."
    navigate_forward
}

print_search_status() {
    if [[ ${#navigate_future[@]} -eq 0 ]]; then
        echo "*** No more search results."
        echo
        return 0
    fi
    local next_result_idx=$((${#navigate_future[@]} - 1))
    local next_commit=${navigate_future[$next_result_idx]}
    local next_commit_id=${commits[$next_commit]}
    local commit_descr=$(git log --pretty="%h %s" \
                         "${next_commit_id}^...${next_commit_id}")
    echo "*** ${#navigate_future[@]} more search results after this one."
    echo "*** f - Next: $commit_descr"
    echo
}

show_help() {
    echo " y           - Cherry pick this commit and move to next"
    echo " n [needle]  - Next; scan forward to find commit"
    echo " p [needle]  - Previous; Scan backward to find commit"
    echo " f           - Forward [to next search result]"
    echo " b           - Back to last commit"
    echo " c           - Search previous commits for conflict source"
    echo " d           - Show full diff"
    echo " q           - Quit"
}

read_commit_ids "$1"

# Commits previously moved (by commit index):
navigate_history=()

# Commits get saved here when we navigate back:
navigate_future=()

# If true, navigate_future contains search results that we are
# stepping through.
have_search_results=false

commit_index=0
while [[ $commit_index -lt ${#commits[@]} ]]; do
    commit=${commits[$commit_index]}
    echo
    git --no-pager show --compact-summary $commit
    echo
    if $have_search_results; then
        print_search_status
    fi
    if check_if_applies $commit; then
        echo "Applies cleanly."
    else
        echo "Does not apply cleanly."
    fi
    while true; do
        arg=
        print_progress
        read -p "Cherrypick? [y/n/p/f/b/d/c/q] " response arg
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
                if conflict_sources $commit; then
                    break
                fi
                ;;
            \?)
                show_help
                ;;
        esac
    done
done

