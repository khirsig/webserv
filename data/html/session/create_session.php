<?php

if (isset($_REQUEST['username']) && isset($_REQUEST['password'])) {
	$username = $_REQUEST['username'];
	$password = $_REQUEST['password'];
	if (empty($username) || empty($password)) {
		header("Location: login.php");
	} else {
		// ini_set("session.cookie_secure", 1);
		session_save_path('/Users/khirsig/Documents');
		session_start();
		$_SESSION['username'] = $username;
		$_SESSION['password'] = $password;
		echo "You have been logged in ";
		echo $_SESSION['username'];

		echo ". Click <a href='logout.php'>here</a> to log out.";
	}
} else {
	echo "Click <a href='login.php'>here</a> to log in.";
}

?>
