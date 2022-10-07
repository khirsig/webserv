
<?php

// while (list($var,$value) = each($_ENV)) {
//     echo "$var $value <br />";
// }

fwrite(STDERR, “hello, world\n”);
$stderr = fopen( 'php://stdout', 'w' );
fwrite($stderr, "Written through the PHP error stream" );
fclose($stderr)

// $myfile = fopen("newfile2.txt", "w") or die("Unable to open file!");

// $i = 1;
// while ($i <= 120) {
//     sleep(1);
//     fwrite($myfile, "$i\n");
//     $i++;
// }

// fclose($myfile);

?>
