<?php
session_start();

echo "<html>";
echo "<head>";
echo "<title>page.php</title>";
echo "</head>";
echo "<body>";
echo "<h1>page.php</h1>";
echo "</body>";

if(!isset($_SESSION['username'])) {
    echo "You are not logged in.";
    echo "<h4><a href='login.php'>Login</a></h4>";
    echo "</body>";
    echo "</html>";
    die();
}

//In $name den Wert der Session speichern
$name = $_SESSION['username'];
$password = $_SESSION['password'];

//Text ausgeben
echo "Your name: '$name'<br>";
echo "Your password: '$password'";

echo "<h4><a href=\"logout.php\">Logout</a></h4>";

echo "</body>";
echo "</html>";

?>
