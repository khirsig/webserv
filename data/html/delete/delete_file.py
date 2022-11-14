# Script that deletes the file at the given path by html form

import cgi
import cgitb
import os

cgitb.enable()

form = cgi.FieldStorage()

# Subdirectory to delete files from with relative paths
delete_prefix_path = "./upload/uploads/"

method = os.environ.get('REQUEST_METHOD')

if method != 'DELETE' and method != 'POST':
    message = 'Only DELETE and POST method is supported'
else:
    file_item = form['filename'].value
    # Test if the file was uploaded
    if file_item:
        if (file_item == "delete_file.py"):
            message = 'Can not delete itself'
        else:
            fn = os.path.basename(file_item.replace("\"", " /"))
            path = delete_prefix_path + fn
            if (os.path.isfile(path)):
                os.remove(path)
                message = 'The file "' + file_item + '" was deleted successfully'
            else:
                message = 'The file "' + file_item + '" does not exist'
    else:
        message = 'No file was deleted'

print("Content-Type: text/html\r\n\r\n", end='')
print("<html>")
print("<body>")
print("<h3>" + message + "</h3>")
print("</body>")
print("</html>")
