#!/usr/bin/python

import os
from datetime import datetime, timezone
import cgi
import cgitb

cgitb.enable()


def human_readable_size(size, decimal_places=2):
    for unit in ['B', 'KiB', 'MiB', 'GiB', 'TiB', 'PiB']:
        if size < 1024.0 or unit == 'PiB':
            break
        size /= 1024.0
    return f"{size:.{decimal_places}f} {unit}"


form = cgi.FieldStorage()
dir = "./"
dir_suffix = os.environ['SCRIPT_FILENAME']
if (dir_suffix != None and dir_suffix != ""):
    dir = dir_suffix

if not os.path.exists(dir):
    print("Content-Type: text/html\r\n\r\n", end='')
    print("<html>")
    print("<body>")
    print("<h3>Directory does not exist</h3>")
    print("</body>")
    print("</html>")
    exit()

print("Content-Type: text/html\r\n")

print("<html>")
print("<head>")
print("<title>Directory Listing</title>")
print("</head>")
print("<body>")
print("<h2>Directory Listing</h2>")
print("<table border='2'>")
print("<tr><th>Type</th><th>Name</th><th>Size</th><th>Last Modified</th></tr>")
print("<tr><td>Directory</td><td><a href='..'>..</a></td><td></td><td></td></tr>")

for file in os.listdir(dir):
    full_path = dir + file
    print("<tr>")
    if (os.path.isdir(full_path)):
        dir_string = "Directory"
    elif ((os.path.islink(full_path))):
        dir_string = "Link"
    elif ((os.path.isfile(full_path))):
        dir_string = "File"
    else:
        dir_string = "Unknown"
    print("<td>%s</td>" % dir_string)
    print("<td><a href='%s'>%s</a></td>" % (file, file))
    print("<td>%s</td>" % human_readable_size(os.path.getsize(full_path)))
    print("<td>%s</td>" %
          datetime.fromtimestamp(os.path.getmtime(full_path), timezone.utc))
    print("</tr>")
print("</table>")
print("</body>")
print("</html>")
