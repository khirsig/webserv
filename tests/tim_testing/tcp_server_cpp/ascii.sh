
x=0
while [ $x -le 255 ]
do
    printf "%3i \'%c\': " $x "$(printf "\x$(printf %x $x)\n")"
    #
    echo -ne "GET http://$(printf "\x$(printf %x $x)\n") HTTP/1.1\nHost: local\n\n" | nc localhost 9001 | head -1
    x=$(( $x + 1 ))
done
