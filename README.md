# 42-webserv

This repository contains one of my projects of the core curriculum at [42 Heilbronn], done together with [tjensen42] and [hepple].

The project webserv is about implementing an HTTP web server in C++.
As with all C++ projects at 42 Heilbronn, the code has to be written according to the C++98 standard.

## Description

The task in this project is to write a web server with several features, based on the HTTP/1.1 standard.
The requirements include:

- config file (user-provided or default)
- i/o multiplexing using `poll()`, `select()`, `kqueue()`, or equivalent
- CGIs
- file upload
- HTTP/1.1 methods `GET`, `POST`, and `DELETE` 
- accurate HTTP response status codes
- serve fully static websites
- directory listing
- chunked requests
- cookies and basic session management _(bonus)_

The behavior of [nginx] is to be considered as a general reference.

## Approach

As a general approach, we decided to do a lot of testing with nginx to get a good overview of the required behavior.
To make sure we followed the HTTP/1.1 standard, we followed the guidelines provided in the [HTTP/1.1 RFC].

For input/output multiplexing, we decided to use `kqueue`. The downside of this is that our web server is only running on __macOS__.

In terms of the [config file], we kept close to nginx. Supported options include:

- set hostname
- turn directory listing on or off
- set an index file
- allow specific HTTP methods
- CGI setup
- set maximum body size for requests
- set custom error pages

We chose to handle the methods `POST` and `DELETE` by CGI.

## How to Use

Clone this repository:
```bash
git clone https://github.com/hepple42/42-webserv.git
```
Change to the cloned directory and build the project:
```bash
cd 42-webserv && make
```
Run the server with our default config file:
```bash
./build/webserv
```
Alternatively, specify a custom config file:
```bash
./build/webserv [config_file_path]
```

## Basic Tests

<details>
  <summary>Some basic tests for evaluation.</summary>

#### Hostname Resolving
```bash
curl --resolve example.com:80:127.0.0.1 http://example.com/
curl --resolve test.com:80:127.0.0.1 http://test.com/
```

#### Client Max Body Size
```bash
curl -X POST -H "Content-Type: text/plain" --data "YES" http://localhost/php/empty.php
curl -X POST -H "Content-Type: text/plain" --data "NONONO" http://localhost/php/empty.php
```

#### Requests
```bash
curl -v -X GET http://localhost:4242
curl -v -X POST -H "Content-Type: text/plain" --data "YES" http://localhost/php/empty.php
curl -v -X DELETE -d 'filename=Makefile' 'http://localhost/delete/delete_file.py'
curl -v -X OPTIONS http://localhost:4242
curl -v -X UNKNOWN http://localhost:4242
```

#### File Upload
```bash
curl -F 'filename=@Makefile' http://localhost/upload/save_file.py
curl -v -X GET http://localhost/upload/uploads/Makefile
curl -v -X DELETE -d 'filename=Makefile' 'http://localhost/delete/delete_file.py'
```

#### Siege
```bash
siege -b -c10 -r1 http://localhost:80/empty
siege -b -c10 -r1 http://localhost:80/test.py
```

</details>


[42 Heilbronn]: https://www.42heilbronn.de/learncoderepeat
[tjensen42]: https://github.com/tjensen42
[khirsig]: https://github.com/khirsig

[nginx]: https://www.nginx.com/
[HTTP/1.1 RFC]: https://www.rfc-editor.org/rfc/rfc2616
[config file]: https://github.com/hepple42/42-webserv/blob/main/webserv.conf
