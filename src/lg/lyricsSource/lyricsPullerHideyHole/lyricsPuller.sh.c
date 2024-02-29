#! /bin/bash

while read arg; do
  curl https://www.azlyrics.com/lyrics/s3rl/$arg.html -L --user-agent 'Mozilla/5.0' -o "$arg.txt"
  sleep 2
    minimumsize=8
    actualsize=$(du -k "$arg.txt" | cut -f 1)
    if [ $actualsize -ge $minimumsize ]; then
        echo size is over $minimumsize kilobytes
    else
        echo size is under $minimumsize kilobytes
        break
    fi
done
