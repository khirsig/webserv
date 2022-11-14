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
	echo "You have been logged out. Click <a href='login.php'>here</a> to log back in.";
} else {
	echo "You are not logged in. Click <a href='login.php'>here</a> to log in.";
}
echo "</body>";
echo "</html>";
?>