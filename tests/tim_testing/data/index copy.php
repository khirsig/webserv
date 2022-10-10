<!DOCTYPE html>
<html><head></head><body>
<?php

sleep(1);

while (list($var,$value) = each ($_ENV)) {
    echo "$var => $value <br />";
}

?>
</body></html>


<?php
$f = fopen( 'php://stdin', 'r' );
while( $line = fgets( $f ) ) {
  echo $line;
}
fclose( $f );
?>
