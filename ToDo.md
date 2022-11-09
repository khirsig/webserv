// TODO: CGIs needed:
Config Parser testen
Config parsing default values
Cookie Management & Session Management,
File Upload,
Delete,
Directory Listing
Seqfault Respone::State()
kqeue EOF wenn wir in write state sind
try/catches check ob alle richtig sind und das richtige abfangen
CGI error catchen und reset und init....
siege -b -c50 -r 1 http://localhost:80/hello.py -> liefert mal 4 bytes mal 0
echo -ne '' | nc localhost ===> EOF stuff handling
cgi kill(pid)

/Applications/Xcode.app/Contents/Developer/usr/bin/python3 fewfew not ending respons



siege -b -c100 -r 100 http://localhost:800

PHP VARS - https://www.php.net/manual/en/reserved.variables.server.php
<!-- 'PATH_INFO'
    Contains any client-provided pathname information trailing the actual script filename but preceding the query string, if available. For instance, if the current script was accessed via the URI http://www.example.com/php/path_info.php/some/stuff?foo=bar, then $_SERVER['PATH_INFO'] would contain /some/stuff. -->
