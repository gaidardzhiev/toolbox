#!/bin/sh

TITLE="Toolbox"
NAME="Mr. Ivan Gaydardzhiev"
GITHUB="gaidardzhiev"
EDX="gaidardzhiev"
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
	echo "Much like BusyBox and ToyBox, Toolbox offers streamlined replacements for many standard utilities you would typically find in GNU coreutils, util-linux, and other common Unix toolsets. While toolbox commands may not include every advanced feature of their full fledged GNU counterparts, they cover the core functionality expected by most users and scripts, ensuring familiar behavior and compatibility."
	echo
	echo "You can find the full source code on my GitHub repository."
	echo
	echo "Thank you for watching this presentation."
	echo "Feel free to reach out via GitHub for questions or collaboration."
	echo
	echo "Goodbye!"
} | pv -qL $SPEED
