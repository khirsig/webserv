# webserv

## Hostname resolving
curl --resolve example.com:80:127.0.0.1 http://example.com/
curl --resolve test.com:80:127.0.0.1 http://test.com/

## Client max body size
curl -X POST -H "Content-Type: text/plain" --data "YES" http://localhost/php/empty.php
curl -X POST -H "Content-Type: text/plain" --data "NONONO" http://localhost/php/empty.php

## Basic requests
curl -v -X GET http://localhost:4242
curl -v -X POST -H "Content-Type: text/plain" --data "YES" http://localhost/php/empty.php
curl -v -X DELETE -d 'filename=Makefile' 'http://localhost/delete/delete_file.py'
curl -v -X OPTIONS http://localhost:4242
curl -v -X UNKNOWN http://localhost:4242

## Upload file
curl -F 'filename=@Makefile' http://localhost/upload/save_file.py
curl -v -X GET http://localhost/upload/uploads/Makefile
curl -v -X DELETE -d 'filename=Makefile' 'http://localhost/delete/delete_file.py'

## SIEGE
siege -b -c10 -r1 http://localhost:80/empty
siege -b -c10 -r1 http://localhost:80/test.py

## Run intra tester -> runs until PUT test
make && build/webserv ./tests/intra_tester/intra_tester.conf
./tests/intra_tester/tester http://localhost:8080
