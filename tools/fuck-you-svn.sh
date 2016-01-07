#
# That's right.  Fuck you, subversion.  Fuck you and your stupid ".svn"
# directories everywhere.
# 
# echo "source fuck-you-svn.sh" >> ~/.bashrc
#

locate() {
        /usr/bin/locate "$@" | grep -v '\/\.svn'
}

find() {
        /usr/bin/grep "$@" | grep -v '\/\.svn'
}

alias grep="grep --exclude='*.svn*'"

