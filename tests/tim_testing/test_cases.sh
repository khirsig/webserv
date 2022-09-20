
echo -ne "    GET /  HTTP/1.1\nHost: test.de\n\n" | nc localhost 9001

echo -ne "\nGET /  HTTP/1.1\nHost: test.de\n\n" | nc localhost 9001

echo -ne "\rGET /  HTTP/1.1\nHost: test.de\n\n" | nc localhost 9001

echo -ne " \rGET /  HTTP/1.1\nHost: test.de\n\n" | nc localhost 9001

echo -ne " \nGET /  HTTP/1.1\nHost: test.de\n\n" | nc localhost 9001

echo -ne "\r\rGET /  HTTP/1.1\nHost: test.de\n\n" | nc localhost 9001

echo -ne "\r\tGET /  HTTP/1.1\nHost: test.de\n\n" | nc localhost 9001

echo -ne "GET http://hello.de/  HTTP/1.1\n\n" | nc localhost 9001

echo -ne "GET http://hello.de/  HTTP/1.1\nHost: test.de\n\n" | nc localhost 9001

echo -ne "GET ../  HTTP/1.1\nHost: test.de\n\n" | nc localhost 9001

echo -ne "GET /test.de  HTTP/1.1\nHost: localhost\n\n" | nc localhost 9001

