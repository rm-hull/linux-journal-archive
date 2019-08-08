<?
include("checkcookie.inc");
include("dbopen.inc");
if (!isset($add)) {
  echo("<title>Adding $addtype</title>\n");
  echo("<body bgcolor=\"#ffffff\" link=\"#000000\" alink=\"#151515\" vlink=\"#000000\">\n");
  echo("<form action=\"$PHP_SELF\" method=post>\n<input type=hidden name=pid value=$pid>\n<input type=hidden name=add value=1>");
}
?>
