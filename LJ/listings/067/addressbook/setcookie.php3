<?
SetCookie($cname, $uid, time()+14400, "/", "", 0);
Header("Location: index.php3\n\n")
include("index.php3");
?>
