#!/usr/bin/env bash
while true; do
	st="$(date +%s)"
	./bird-up
	en="$(date +%s)"
	echo "You survived for $((${en} - ${st})) seconds"
	read -p "Would you like to play again? (y/n) " -n 1 -r
	if [[ $REPLY =~ ^[Yy]$ ]]
	then
		continue
	fi
	break
done

