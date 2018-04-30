#!/bin/sh

cd "$( dirname "${BASH_SOURCE[0]}" )"
EXEC=bin/premier
LOGFILE="${2:=run.log}"

# Logging
"$EXEC" 2>&1 | while read line; do
    echo "$(cat /proc/uptime | cut -d ' ' -f 1) $line" >> "$LOGFILE"
done

sh lcdOff.sh
