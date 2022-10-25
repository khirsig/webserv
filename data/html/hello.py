#!/usr/bin/python

# Import modules for CGI handling
import cgi
import cgitb
import time

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get data from fields
first_name = form.getvalue('first_name')
last_name = form.getvalue('last_name')


print("Content-type:text/html\r\n\r\n", flush=True)
time.sleep(3)
print("<html>", flush=True)
print("<head>", flush=True)
print("<title>Hello - Second CGI Program</title>", flush=True)
print("</head>", flush=True)
print("<body>", flush=True)
time.sleep(3)
print("<h2>Hello %s %s</h2>" % (first_name, last_name))
print("</body>", flush=True)
print("</html>", flush=True)

cgi.print_environ()

# print("42")
