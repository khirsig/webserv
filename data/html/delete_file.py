import os
import cgi

form = cgi.FieldStorage()
# Get filename here
fileitem = form['filename']

if fileitem.filename:
    fn = os.path.basename(fileitem.filename.replace("\\", " /"))
    os.remove(fileitem.filename)
    print("the file was deleted")
else:
    print("no file was deleted")
