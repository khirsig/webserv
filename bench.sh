#!/bin/sh

CONTENT_LEN=1000
CONTENT=`perl -E "say \"-\" x $CONTENT_LEN"`
REQUEST="POST /php/empty.php HTTP/1.1\r\nHost: localhost\r\nContent-Length: $CONTENT_LEN\r\n\r\n$CONTENT"

i=0
while [ $i -ne 10000 ]
do
    echo -ne "$REQUEST" | nc localhost 80 >/dev/null
    if [ $? -ne 0 ]; then
        echo "Failed"
        exit 1
    fi
    i=$(($i+1))
    # echo "$i"
done
