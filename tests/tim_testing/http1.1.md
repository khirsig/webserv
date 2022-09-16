# Request-Line

- Method Path?Query Protocol

-> Seperator only by spaces

# Methods (is case-sensitive)
"OPTIONS"
"GET"
"HEAD"
"POST"
"PUT"
"DELETE"
"TRACE"
"CONNECT"

# Request-URI
Request-URI    = "*" | absoluteURI | abs_path | authority

http_URL = "http:" "//" host [ ":" port ] [ abs_path [ "?" query ]]

- start with / or http://
- does not contain chars <= 31 && == 127
- does not contain % without 2 following HEX chars

# Request Header Fields
https://de.wikipedia.org/wiki/Liste_der_HTTP-Headerfelder


Header: value1, value2

Header: value1,
        value2

- zeichen vor doppelpunkt
- nur whitespaces nicht erlaubt

# Headers to handle
Connection: close
Cookie: $Version=1; Skin=new;
Content-Length: 348
Content-MD5: Q2hlY2sgSW50ZWdyaXR5IQ==
Host: en.wikipedia.org
If-Modified-Since: Sat, 29 Oct 1994 19:43:31 GMT




GET / HTTP/1.1
Host: local
Connection: close
Content-Length: 0