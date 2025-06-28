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

{
	echo "Welcome to the presentation of the Toolbox project!"
	echo
	echo "Toolbox is a compact, statically linked binary that combines many common command line utilities into a single executable. It is designed as a minimalist and efficient toolkit that provides essential *nix commands, all accessible through one binary, simplifying deployment and usage especially in resource constrained or embedded environments."
	echo
	echo "Let's explore the toolbox commands and see them in action!"
	echo
} | pv -qL $SPEED

FILE="./toolbox file toolbox | pv -qL 30"
for ((i=0; i<${#FILE}; i++)); do
	printf "%s" "${FILE:$i:1}"
	sleep 0.02
done
printf "\n"
eval "$FILE"
