# webserv

## Upload file
curl -F 'filename=@Makefile' http://localhost/upload/save_file.py

## Run intra tester -> runs until PUT test
make && build/webserv ./tests/intra_tester/intra_tester.conf
./tests/intra_tester/tester http://localhost:8080

## SIEGE
siege -b -c10 -r1 http://localhost:80
siege -b -c10 -r1 http://localhost:80/test.py
siege -b -c10 -r1 --content-type="image/png" 'http://localhost:80/test.py POST < ./data/html/upload/uploads/2500.png'
