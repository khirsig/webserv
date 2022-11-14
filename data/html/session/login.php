<?php
session_save_path('/Users/khirsig/Documents');
session_start();

echo "<html>";
echo "<body>";

// If user is logged in with session, greet user and provide logout link
// else provide form with username and password that posts to create_session.php
if (isset($_SESSION['username'])) {
    echo "You are logged in as " . $_SESSION['username'] . ". Click <a href='logout.php'>here</a> to log out.";
} else {
    echo "<form enctype=\"multipart/form-data\" action=\"create_session.php\" method=\"post\">
		<p>Username: <input type=\"text\" name=\"username\" /></p>
		<p>Password: <input type=\"text\" name=\"password\" /></p>
		<p><input type=\"submit\" value=\"Login\" /></p>
	</form>";
}
echo "</html>";
echo "</body>";

?>
