#/bin/bash

alias la='ls -al'
alias ll='ls -l'
alias cp="cp -i"
alias mv="mv -i"
alias free='free -m'
alias df='df -h'

export PS1="[\u@\h \W] "
export PS2="> "
export PS3="+ "
export PS4="- "

alias r="/etc/init.d/S50chef restart"
alias s="/etc/init.d/S50chef stop"
alias c="cd /opt/chef/"
alias l="tail -f $(find /opt/chef/log | sort | tail -1)"
