#!/bin/sh
#ffmpeg -video_size 1920x1080 -framerate 30 -f x11grab -i :0.0 output.mp4
#ffmpeg -i voice.m4a -i music.m4a -filter_complex "[0:a]volume=1.2[a0]; [1:a]volume=0.3[a1]; [a0][a1]amix=inputs=2:duration=longest[out]" -map "[out]" -c:a libmp3lame -q:a 2 output.mp3
#ffmpeg -i output.mp4 -i output.mp3 -c:v copy -c:a aac -map 0:v:0 -map 1:a:0 presentation.mp4

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
	echo "EdX username: $EDX"
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
