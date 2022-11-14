<?php
session_start();

echo "<html>";
echo "<head>";
echo "<title>logout.php</title>";
echo "</head>";
echo "<body>";
echo "<h1>logout.php</h1>";

if (isset($_SESSION['username'])) {
	session_destroy();
	echo "You have been logged out.<br>";
} else {
    echo "You are not logged in.<br>";
}
echo "<h4><a href='page.php'>Page</a></h4>";
echo "<h4><a href='login.php'>Login</a></h4>";

echo "</body>";
echo "</html>";
?>