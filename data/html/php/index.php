<?php
// print environment variables in a beautiful way
echo "ENVIRONMENT VARIABLES:";
echo "<PRE>";
print_r($_ENV);
echo "</PRE>";

// print cookies in a beautiful way
echo "</br>";
echo "COOKIES:";
echo "<PRE>";
print_r($_COOKIE);
echo "</PRE>";

// print session variables in a beautiful way
echo "</br>";
echo "SESSION VARIABLES:";
echo "<PRE>";
print_r($_SESSION);
echo "</PRE>";

// PHP INFO
echo "</br>";
echo "PHP INFO:";
phpinfo();

?>