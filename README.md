# webserv


## TO-DO

- Port in Host
- timer reset nach send()
- HEAD implementieren
- cleint_max_body_size abfragen request
- error messages ueber poll schreiben

- eni in connection packen
- eni mit enable / disable

- check for 0 and -1 in write/read/etc

## Questionable

- Connection keep alive bei einigen errors
- allowed methods wenn GET nicht erlaubt und keine cgi error


## Upload file
curl -F 'filename=@/Users/tjensen/Documents/code/webserv/Makefile' http://localhost/upload/save_file.py
