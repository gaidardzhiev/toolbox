#!/bin/sh

./toolbox ls /proc && echo $?

./toolbox cp /proc/cpuinfo /tmp && echo ?

./toolbox mkdir /tmp/test_dir && echo ?

./toolbox cat /proc/cpuinfo && echo $?

./toolbox echo "Hack the World" && echo $?

/usr/bin/sleep 1024 & ps aux | grep "[s]leep 1024" | awk '{print $2}' | xargs -I{} ./toolbox kill 9 {}

./toolbox clear && echo $?

./toolbox rm /tmp/cpuinfo && echo $?

/usr/bin/touch /tmp/file && ./toolbox rm /tmp/file && echo $?

./toolbox head

./toolbox diff

./toolbox file toolbox && echo $?

./toolbox grep "grep" toolbox.c && echo $?

./toolbox rmdir /tmp/test_dir && echo $?

./toolbox ascii2hex

./toolbox sync && echo $?
