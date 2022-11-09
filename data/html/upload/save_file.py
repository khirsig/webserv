#!/usr/bin/python
import cgi
import os
import cgitb
import sys

cgitb.enable()

form = cgi.FieldStorage()
# print(form)

# for line in sys.stdin:
#     print(line)

# sys.stderr.write("try 1 %s \n" % form)
# Get filename here.

UPLOAD_DIR = 'uploads/'

file_item = form['filename']

# Test if the file was uploaded
if file_item.filename:
    # strip leading path from file name to avoid
    # directory traversal attacks
    fn = os.path.basename(file_item.filename.replace("\"", " /"))
    open(UPLOAD_DIR + fn, 'wb').write(file_item.file.read())
    message = 'The file "' + fn + '" was uploaded successfully'
else:
    message = 'No file was uploaded'

print("Content-Type: text/html\r\n\r\n", end = '')

print("<html>")
print("<body>")
print("<h3>" + message + "</h3>")
print("<h3><a href=\"uploads/\">See all uploads -></a></h3>")
print("</body>")
print("</html>")
