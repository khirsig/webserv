// TODO: CGIs needed:
Config Parser testen
Cookie Management & Session Management,
Delete,
Directory Listing
try/catches check ob alle richtig sind und das richtige abfangen
CGI error catchen und reset und init....
cgi kill(pid)
broken piped
time out when request done

/////////////
/// FIXED ///
/////////////

client_body_size should inherit in config
siege -b -c50 -r 1 http://localhost:80/hello.py -> liefert mal 4 bytes mal 0
echo -ne '' | nc localhost ===> EOF stuff handling
kqeue EOF wenn wir in write state sind
/Applications/Xcode.app/Contents/Developer/usr/bin/python3 fewfew not ending respons
siege -b -c100 -r 100 http://localhost:800
File Upload,
Seqfault Respone::State()
Config parsing default values
