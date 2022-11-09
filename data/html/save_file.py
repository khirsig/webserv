#!/usr/bin/python
import cgi
import os
import cgitb
import sys

cgitb.enable()

form = cgi.FieldStorage()
print(form)

for line in sys.stdin:
    print(line)

# sys.stderr.write("try 1 %s \n" % form)
# Get filename here.
# fileitem = form['filename']
# Test if the file was uploaded
# if fileitem.filename:
#     # strip leading path from file name to avoid
#     # directory traversal attacks
#     fn = os.path.basename(fileitem.filename.replace("\"", " /"))
#     open('/tmp/' + fn, 'wb').write(fileitem.file.read())
#     message = 'The file "' + fn + '" was uploaded successfully'
# else:
#     message = 'No file was uploaded'
# print("Content-Type: text/html")
# print
# print("<html>")
# print("<body>")
# print("<p>%s</p>") % (message)
# print("</body>")
# print("</html>")
