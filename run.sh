#!/usr/bin/env bash
while true; do
	st="$(date +%s)"
	./bird-up
	en="$(date +%s)"
	echo "You survived for $((${en} - ${st})) seconds"
	read -p "Would you like to play again? (y/n) "
	if [[ $REPLY =~ ^[Yy]$ ]]
	then
		continue
	fi
	break
done

