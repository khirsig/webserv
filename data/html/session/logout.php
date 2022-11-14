<?php
session_save_path('/Users/khirsig/Documents');
session_start();

echo "<html>";
echo "<body>";

if (isset($_SESSION['username'])) {
	session_destroy();
	echo "You have been logged out. Click <a href='login.php'>here</a> to log back in.";
} else {
	echo "You are not logged in. Click <a href='login.php'>here</a> to log in.";
}
echo "</html>";
echo "</body>";
?>