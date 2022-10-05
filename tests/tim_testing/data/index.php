
<?php

$myfile = fopen("newfile2.txt", "w") or die("Unable to open file!");

$i = 1;
while ($i <= 120) {
    sleep(1);
    fwrite($myfile, "$i\n");
    $i++;
}

fclose($myfile);

?>
