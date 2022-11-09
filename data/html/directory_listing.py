#!/usr/bin/python

import os
from datetime import datetime, timezone
# import sys
# import cgi
# import cgitb
# cgitb.enable()

print("Content-Type: text/html\r\n")

print("<html>")
print("<head>")
print("<title>Directory Listing</title>")
print("</head>")
print("<body>")
print("<h2>Directory Listing</h2>")
print("<table border='2'>")
print("<tr><th>Name</th><th>Size</th><th>Last Modified</th></tr>")
for file in os.listdir('.'):
    print("<tr>")
    print("<td><a href='%s'>%s</a></td>" % (file, file))
    print("<td>%s</td>" % os.path.getsize(file))
    print("<td>%s</td>" %
          datetime.fromtimestamp(os.path.getmtime(file), timezone.utc))
    print("</tr>")
print("</table>")
print("</body>")
print("</html>")
