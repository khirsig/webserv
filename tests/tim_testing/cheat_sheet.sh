# list commands that listen on port 41
lsof -i -P | grep ":41"

# start simple tcp server to grep requests
nc -l 9001

# send tcp packages to IP
nc 127.0.0.1 9001 << EOF
GET / HTTP/1.1
Host: localhost:41
EOF



## RUN TCP Clients

c++ tcp_client.cpp -o tcp_client && ./tcp_client 127.0.0.1 9001 $'GET / HTTP/1.1\nHost: localhost\n\n'

python3 tcp_client.py 127.0.0.1 9001 $'GET / HTTP/1.1\nHost: localhost\n\n'
