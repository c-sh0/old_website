<?php

$fd = fopen("/tmp/session.txt", "a+") or die("can`t open file!");
$fout = fwrite($fd, "$QUERY_STRING\n");
fclose($fd);

?>

