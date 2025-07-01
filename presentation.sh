#!/bin/sh
#printf "ffmpeg -video_size 1920x1080 -framerate 30 -f x11grab -i :0.0 output.mp4\n"

TITLE="Toolbox"
NAME="Mr. Ivan Gaydardzhiev"
GITHUB="gaidardzhiev"
EDX="Gaidardzhiev"
CITY="Samos"
COUNTRY="Greece"
DATE=$(date)
SPEED=30

{
	echo "Project: $TITLE"
	echo "Presenter: $NAME"
	echo "GitHub: https://github.com/$GITHUB"
	echo "edX: https://edx.org/user/$EDX"
	echo "Location: $CITY, $COUNTRY"
	echo "Date: $DATE"
	echo
} | pv -qL $SPEED

FILE="./toolbox file toolbox | pv -qL 30"
for ((i=0; i<${#FILE}; i++)); do
	printf "%s" "${FILE:$i:1}"
	sleep 0.02
done
printf "\n"
eval "$FILE"
