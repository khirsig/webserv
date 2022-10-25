# webserv


## TO-DO

- Port in Host
- timer reset nach send()
- HEAD implementieren
- cleint_max_body_size abfragen request
- error messages ueber poll schreiben

- eni in connection packen
- eni mit enable / disable


// Python CGI Dir listing
// Python CGI Create file

<!-- location ~ "/upl/([0-9a-zA-Z-.]*)$" {
        alias     /storage/www/upl/$1;
        client_body_temp_path  /tmp/upl_tmp;
        dav_methods  PUT DELETE MKCOL COPY MOVE;
        create_full_put_path   on;
        dav_access             group:rw  all:r;
} -->


## Questionable

- Connection keep alive bei einigen errors
- allowed methods wenn GET nicht erlaubt und keine cgi error

