#!/bin/sh
#
# Copyright(C) Simon Howard
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
# SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
# IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
#
#
# Tool for managing subversion branches.
#

# Bomb out with an error if a subprocess fails.

set -eu

# Exit with an error

error() {
    if [ $# != 0 ]; then
        echo "$@" > /dev/stderr
    fi
    exit 1
}

# Get the specified repository property

repository_prop() {
    prop="$1"
    path="$2"

    propval=`svn info "$path" | grep "^$prop:"`

    if [ ".$propval" = "." ]; then
        error "Failed to read '$prop' property for '$path'."
    fi

    echo "$propval" | sed "s/$prop: //"
}

# Check the specifide URL / WC path has the required properties

check_properties() {
    properties=`svn proplist "$1"`

    if ! echo "$properties" | grep -q branch_helper:parent_url; then
        error "Parent URL not set for $1"
    fi

    if ! echo "$properties" | grep -q branch_helper:last_merged; then
        error "Last merged property not set for $1"
    fi
}

# Expand a repository path to a full URL (if it is already a full URL,
# acts as a passthrough)

expand_url() {
    case "$1" in
        *://*)
            # No conversion needed
            echo "$1"
            ;;
        /*)
            rroot=`repository_prop "Repository Root" .`
            echo "$rroot$1"
            ;;
        *)
            error "'$1' should be a URL or full repository path, eg. /trunk"
            ;;
    esac
}

# Get the parent URL for the specified URL / WC path

parent_url() {
    parent=`svn propget branch_helper:parent_url "$1"`
    expand_url "$parent"
}

# Get the last-merged revision for the specified URL / WC path

last_merged() {
    svn propget branch_helper:last_merged "$1"
}

# Check if there are changes to the current working directory

have_changes() {
    changes=`svn status | grep -c -v "^\\?"`

    [ ".$changes" != ".0" ]
}

# Check the working directory is clean

check_clean() {
    if have_changes; then
        echo "There are changes in the current working directory."
        echo "Use 'svn revert -R .' to undo them before merging."
        exit 1
    fi
}

# Undo local changes to a property

revert_property() {
    property="$1"
    wc_path="$2"

    # There is no proper way to "revert" a property with svn.  If the
    # property previously existed, set it back to its previous value.
    # If it didn't exist before, delete it.

    if svn proplist -r BASE "$wc_path" | grep -q $property; then
        svn propget -r BASE $property "$wc_path" |                \
            svn propset -F /dev/stdin $property "$wc_path" > /dev/null
    else
        svn propdel $property "$wc_path" > /dev/null
    fi
}

# Set the parent repository and last-merged revision

set_parent() {
    if [ $# != 2 ]; then
        help
    fi

    svn propset "branch_helper:parent_url" "$1" . > /dev/null
    svn propset "branch_helper:last_merged" "$2" . > /dev/null
}

# Merge changes from parent

pull_changes() {

    check_properties .

    parent_url=`parent_url .`
    last_merged=`last_merged .`

    # Check the working directory is clean

    check_clean

    # Get the revision of the last change to the parent

    update_revision=`repository_prop "Last Changed Rev" "$parent_url"`

    echo "Merging to revision $update_revision..." > /dev/stderr

    # Do the merge

    if ! svn merge -r$last_merged:$update_revision "$parent_url"; then
        error "Merge failed."
    fi

    svn propset branch_helper:last_merged "$update_revision" . > /dev/null
}

# Merge a branch to the working directory.

merge_branch() {

    if [ $# != 1 ]; then
        help
    fi

    branch_url=`expand_url "$1"`

    # Get branch properties

    check_properties "$branch_url"
    parent_url=`parent_url "$branch_url"`
    last_merged=`last_merged "$branch_url"`

    check_clean

    # Do the merge

    echo "Merging branch ..." > /dev/stderr

    if ! svn merge --reintegrate $parent_url@$last_merged $branch_url; then
        error "Merge failed."
    fi

    revert_property branch_helper:parent_url .
    revert_property branch_helper:last_merged .
}

# Create a new branch from the current directory.

create_branch() {

    if [ $# != 1 ]; then
        help
    fi

    branch_url=$1
    expanded_branch_url=`expand_url "$1"`

    check_clean

    revision=`repository_prop "Revision" .`
    url=`repository_prop "URL" .`

    set_parent "$branch_url" $revision

    errmsg=""

    if ! svn cp . "$expanded_branch_url"; then
        errmsg="Failed to create branch"
    fi

    # Undo property changes

    svn revert . > /dev/null

    if [ ".$errmsg" != "." ]; then
        error "$errmsg"
    fi
}

# Create a tag from the current directory at the specified URL/repository path

create_tag() {

    if [ $# != 1 ]; then
        help
    fi

    tag_url=`expand_url "$1"`

    check_clean

    if ! svn cp . "$tag_url"; then
        error "Failed to create tag"
    fi
}

# Switch branch by repository path (helper for svn switch)

switch_branch() {

    if [ $# != 1 ]; then
        help
    fi

    branch_path="$1"
    branch_url=`expand_url "$1"`

    echo "Switching to $branch_path ..." > /dev/stderr

    if ! svn switch "$branch_url"; then
        echo "Failed to switch branch."
    fi
}

# List all changes on a branch relative to its parent branch

diff_branch() {

    if [ $# = 0 ]; then

        # Convenience zero-parameter version for comparing the branch
        # at the current directory.  It is not possible to compare
        # a URL against a working path.

        check_properties "."
        parent_url=`parent_url "."`
        last_merged=`last_merged "."`

        wd_url=`repository_prop "URL" .`
        wd_revision=`repository_prop "Revision" .`

        # Diff the branch against its parent

        target1="$parent_url@$last_merged"
        target2="$wd_url@$wd_revision"

    elif [ $# = 1 ]; then
        # Get full branch URL

        branch_url=`expand_url "$1"`

        # Get branch properties

        check_properties "$branch_url"
        parent_url=`parent_url "$branch_url"`
        last_merged=`last_merged "$branch_url"`

        # Diff the branch against its parent

        target1="$parent_url@$last_merged"
        target2="$branch_url"
    else
        help
    fi

    svn diff "$target1" "$target2"
}

# Extended version of expand_url used for diff_paths that accepts "."
# as a target

expand_diff_url() {
    if [ ".$1" = ".." ]; then
        wd_url=`repository_prop "URL" .`
        wd_revision=`repository_prop "Revision" .`

        echo "$wd_url@$wd_revision"
    else
        expand_url "$1"
    fi
}

# Compare two branches, using expanded path

diff_paths() {
    if [ $# = 1 ]; then
        branch1_url=`expand_diff_url "."`
        branch2_url=`expand_diff_url "$1"`
    elif [ $# = 2 ]; then
        branch1_url=`expand_diff_url "$1"`
        branch2_url=`expand_diff_url "$2"`
    else
        help
    fi

    svn diff "$branch1_url" "$branch2_url"
}

# Help text

help() {
    progname=`basename "$0"`

    cat <<END
Usage:

$progname branch <branch path>
    - Create a branch from the current directory at the specified path
      within the subversion repository.

$progname diff <path 1> [ <path 2> ]                           (di)
    - Show differences between two repository paths.  If a second path is
      not specified, the current directory is used.

$progname diff_branch [ <branch path> ]                        (db)
    - List all changes on the specified branch, relative to its parent
      branch (working directory is used if the branch is not specified).

$progname merge_branch <branch path>                           (mb)
    - Merge a child branch into the current directory.

$progname pull
    - Pull in (merge) latest changes from the parent branch.

$progname switch <branch path>                                 (sw)
    - Switch to a different branch within the repository.

$progname tag <tag path>
    - Tag the current directory at the specified path within the subversion
      repository.

END

    if [ $# != 0 ]; then
        exit "$1"
    else
        exit 1
    fi
}

# Pick which command we want

if [ $# = 0 ]; then
    help 0
fi

cmd="$1"
shift

case "$cmd" in
    pull)
        pull_changes "$@"
        ;;
    set_parent)
        set_parent "$@"
        ;;
    branch|br)
        create_branch "$@"
        ;;
    diff|di)
        diff_paths "$@"
        ;;
    diff_branch|db)
        diff_branch "$@"
        ;;
    merge_branch|mb)
        merge_branch "$@"
        ;;
    switch|sw)
        switch_branch "$@"
        ;;
    tag)
        create_tag "$@"
        ;;
    *)
        help 0
        ;;
esac

exit 0

