# webserv

## Upload file
curl -F 'filename=@Makefile' http://localhost/upload/save_file.py

## Run intra tester -> runs until PUT test
make && build/webserv ./tests/intra_tester/intra_tester.conf
./tests/intra_tester/tester http://localhost:8080
