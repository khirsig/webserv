#!/usr/bin/python

# # Import modules for CGI handling
# import cgi
# import cgitb

# # Create instance of FieldStorage

# # Get data from fields
# first_name = form.getvalue('first_name')
# last_name = form.getvalue('last_name')

print("Content-Type: text/html\r\n\r\n", end = '')

import os
print("REQUEST_METHOD: " + os.environ['REQUEST_METHOD'] + "")
