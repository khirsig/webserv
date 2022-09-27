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
Host: en.wikipedia.org
Connection: close
Content-Length: 348
Cookie: $Version=1; Skin=new;

Transfer-Encoding: chunked
Content-MD5: Q2hlY2sgSW50ZWdyaXR5IQ==
If-Modified-Since: Sat, 29 Oct 1994 19:43:31 GMT



# PARSING

#define CR                  '\r'
#define LF                  '\n'

IS_HEADER_CHAR
IS_URL_CHAR
IS_HOST_CHAR


###### TO DO

# Request line
* http://

# HEADER
* start header line with space/tab

# Error Handling

# Debug printer

# Analyse Request
* Check for 1 host
* Check Path for ../
* Content-Length
* Set Connection bool





REQUEST-LINE:
'GET' '/1/../?42#test' 'HTTP/1.1'

HEADER:
'KEY' = 'VALUE'

ANALYZED:
'Host' = ''
'keep-alive' = 'false'




