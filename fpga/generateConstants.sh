#!/usr/bin/env bash

cd "$(dirname "${BASH_SOURCE[0]}")"

grep "#define [A-Z0-9_]\+ '[a-zA-Z]'" ../arduino/AFsignals.h | while read line
do 
    name="$(echo "$line" | cut -d ' ' -f 2)"
    letter="$(echo "$line" | cut -d ' ' -f 3 | cut -d "'" -f 2)"
    hex="$(python -c "print(hex(ord('$letter'))[2:])")"

    echo "    constant $name : std_logic_vector(7 downto 0) := x\"$hex\"; -- '$letter'"
done
