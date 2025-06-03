#!/bin/sh

TITLE="Toolbox"
NAME="Mr. Ivan Gaydardzhiev"
GITHUB="gaidardzhiev"
EDX="Gaidardzhiev"
CITY="Samos"
COUNTRY="Greece"
DATE=$(date)
SPEED=20

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
	echo "Welcome to the presentation of my project!"
	echo
	echo "Toolbox is a compact, statically linked binary that combines many common command line utilities into a single executable. It is designed as a minimalist and efficient toolkit that provides essential Unix commands, all accessible through one binary, simplifying deployment and usage especially in resource constrained or embedded environments."
	echo
} | pv -qL $SPEED
