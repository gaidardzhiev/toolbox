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
	{ /usr/bin/touch /tmp/file && ./toolbox rm /tmp/file; } && return 0 || return 9
}

./toolbox head

./toolbox diff

./toolbox file toolbox && echo $?

./toolbox grep "grep" toolbox.c && echo $?

./toolbox rmdir /tmp/test_dir && echo $?

./toolbox ascii2hex

./toolbox sync && echo $?
