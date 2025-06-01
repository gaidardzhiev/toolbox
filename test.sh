#!/bin/sh

fls() {
	./toolbox ls /proc && return 0 || return 2
}

fcp() {
	./toolbox cp /proc/cpuinfo /tmp && return 0 || return 3
}

fmkdir() {
	./toolbox mkdir /tmp/test_dir && return 0 || return 4
}

fcat() {
	./toolbox cat /proc/cpuinfo && return 0 || return 5
}

fecho() {
	./toolbox echo "Hack the World" && return 0 || return 6
}

fkill() {
	/usr/bin/sleep 1024 & ps aux | grep "[s]leep 1024" | awk '{print $2}' | xargs -I{} ./toolbox kill 9 {} && return 0 || return 7
}

fclear() {
	./toolbox clear && return 0 || return 8
}

frm() {
	{ /usr/bin/touch /tmp/file; ./toolbox rm /tmp/file; } && return 0 || return 9
}

fhead() {
	./toolbox head && return 0 || return 10
}

fdiff() {
	./toolbox diff toolbox.c toolbox.1 && return 0 || return 11
}

ffile() {
	./toolbox file toolbox && return 0 || return 12
}

ffgrep() {
	./toolbox grep "grep" toolbox.c && return 0 || return 13
}

frmdir() {
	./toolbox rmdir /tmp/test_dir && return 0 || return 14
}

#fa2x() {
#	#TODO: test ./toolbox ascii2hex && return 0 || return 15
#}

fsync() {
	./toolbox sync && return 0 || return 16
}

{ fls && fcp && fmkdir && fcat && fecho && fkill && fclear && frm && fhead && fdiff && ffile && ffgrep && frmdir && fsync; RET=$?; } || exit 1

[ "$RET" -eq 0 ] 2>/dev/null || printf "%s\n" "$RET"
