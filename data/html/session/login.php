<?php
session_start();

echo "<html>";
echo "<head>";
echo "<title>login.php</title>";
echo "</head>";
echo "<body>";
echo "<h1>login.php</h1>";

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
