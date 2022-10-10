#!/bin/bash

# REQUEST=$'GET / HTTP/1.1\nHost: localhost\nConnection: close\n\n'
# REQUEST2=$'GET / HTTP/1.1\nHost: localhost\nConnection: keep-alive\n\n'

# cc tcp_client/tcp_client.c -o tcp_client/tcp_client

# echo -ne 'GET / HTTP/1.1\nHost: local\n' | nc localhost 900

x=1
while [ $x -le 20000 ]
do
    printf "A" | nc localhost 900

    echo -ne "GET http:// HTTP/1.1\nHost: locla\n\n" | nc localhost 9001
    # ./tcp_client/tcp_client 127.0.0.1 9001 "$REQUEST2" >/dev/null &
    # ./tcp_client/tcp_client 127.0.0.1 9001 "$REQUEST" >/dev/null
    # echo $?
    # nc 127.0.0.1 9001 <<< "$REQUEST" >/dev/null #> out_nc
    # echo $?
    # python3 ./tcp_client/tcp_client.py 127.0.0.1 9001 "$REQUEST" > out_py
    # echo $?
    # if ! diff -I '^Date.*' -q -u out_c out_nc >/dev/null;
    # then
    #     echo c KO
    #     exit 1
    # fi
    # if ! diff -I '^Date.*' -q -u out_nc out_py >/dev/null;
    # then
    #     echo py KO
    #     exit 1
    # fi
    printf "$x\n"
    x=$(( $x + 1 ))
done

# echo -ne '\n\n' | nc localhost 900

exit 1
