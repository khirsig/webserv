def recv_all(sock):
    BUFF_SIZE = 4096 # 4 KiB
    data = b''
    while True:
        part = sock.recv(BUFF_SIZE)
        data += part
        if len(part) < BUFF_SIZE:
            break
    return data


#----- A simple TCP client program in Python using send() function -----
import sys
import socket

if len(sys.argv) != 4:
    print("usage:\npython3 tcp_client.py 127.0.0.1 9001 MSG_DATA")
    exit(1)

# Create a client socket
clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM);

# Connect to the server
clientSocket.connect((sys.argv[1], int(sys.argv[2])));

# Send data to server
data = sys.argv[3];
clientSocket.send(data.encode());

# Receive data from server
# dataFromServer = clientSocket.recv(1024);
dataFromServer = recv_all(clientSocket);

# Print to the console
print(dataFromServer.decode(), end='');
