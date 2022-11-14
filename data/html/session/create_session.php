<?php
session_start();
echo "<html>";
echo "<head>";
echo "<title>create_session.php</title>";
echo "</head>";
echo "<body>";
echo "<h1>create_session.php</h1>";

if (isset($_REQUEST['username']) && isset($_REQUEST['password'])) {
    $username = $_REQUEST['username'];
	$password = $_REQUEST['password'];
	if (empty($username) || empty($password)) {
        header("Location: login.php");
	} else {
        $_SESSION['username'] = $username;
		$_SESSION['password'] = $password;
		echo "Logged in as '$username'. <br>
        Click <a href='page.php'>here</a> to continue.";
		echo "<h4><a href='logout.php'>Logout</a></h4>";
	}
} else {
    $URL="./login.php";
	echo "<script type='text/javascript'>document.location.href='{$URL}';</script>";
	echo '<META HTTP-EQUIV="refresh" content="0;URL=' . $URL . '">';
}

echo "</body>";
echo "</html>";

?>
