<?
include("checkcookie.inc");
include("dbopen.inc");
$rname=pg_exec($conn, "select fname, minit, lname from person where pid=$pid");
$remail=pg_exec($conn, "select mailto_url from email_addresses where person_id=$pid");
$rphone=pg_exec($conn, "select phone_num from phone_numbers where person_id=$pid");
$rwww=pg_exec($conn, "select href from www_addresses where person_id=$pid");
$rpmain=pg_exec($conn, "select aid, pid, city, sname, cname, postcode from v_post_main where pid=$pid");
$name=pg_fetch_array($rname, 0);
echo("<title>addressbook::$name[0] $name[1] $name[2]</title>\n");
echo("<body bgcolor=\"#ffffff\" link=\"#000000\" alink=\"#151515\" vlink=\"#000000\">");
// echo("$PHP_SELF</p>");
echo("<p><h3>Here's what you have for $name[0] $name[1] $name[2]:</h3><br>");
$ecurrow=0; $emaxrow=pg_numrows($remail);

echo("<b><font size=3>email addresses:</b></font><br>\n<font size=2><a href=\"add-email.php3?pid=$pid\">add one</a></font><ul>\n");
echo("<table cols=1 width=40% bgcolor=#EEEEEE NOSAVE>\n");
while($ecurrow<$emaxrow) {
  if(($ecurrow%2)==1)
    { echo("<tr bgcolor=\"#DADADA\" nosave>\n<td nosave>\n"); }
  else
    { echo("<tr bgcolor=\"#F4F4F4\" nosave>\n<td nosave>\n"); }
  $erow=pg_fetch_array($remail, $ecurrow);
  echo("<li> $erow[0] </li>\n");
  $ecurrow++;
  echo("</td>\n</tr>\n");
}
echo("</ul>\n");
echo("</table><br><br><br>");


$pcurrow=0; $pmaxrow=pg_numrows($rphone);
echo("<b><font size=3>telephone numbers:</b></font><br><font size=2><a href=\"add-phone.php3?pid=$pid\">add one</a></font>\n<ul>\n");
echo("<table cols=1 width=40% bgcolor=#EEEEEE NOSAVE>\n");
while($pcurrow<$pmaxrow) {
  if(($pcurrow%2)==1)
    { echo("<tr bgcolor=\"#DADADA\" nosave>\n<td nosave>\n"); }
  else
    { echo("<tr bgcolor=\"#F4F4F4\" nosave>\n<td nosave>\n"); }
  $prow=pg_fetch_array($rphone, $pcurrow);
  echo("<li> $prow[0] </li>\n");
  $pcurrow++;
  echo("</td>\n</tr>\n");
}
echo("</ul>");
echo("\n</table><br><br><br>");

$wcurrow=0; $wmaxrow=pg_numrows($rwww);
echo("<b><font size=3>www addresses:</b></font><br><font size=2><a href=\"add-www.php3?pid=$pid\">add one</a></font>\n<ul>\n");
echo("<table cols=1 width=40% bgcolor=#EEEEEE NOSAVE>\n");
while($wcurrow<$wmaxrow) {
  if(($wcurrow%2)==1)
    { echo("<tr bgcolor=\"#DADADA\" nosave>\n<td nosave>\n"); }
  else
    { echo("<tr bgcolor=\"#F4F4F4\" nosave>\n<td nosave>\n"); }
  $wrow=pg_fetch_array($rwww, $wcurrow);
  echo("<li> $wrow[0] </li>\n");
  $wcurrow++;
  echo("</td>\n</tr>\n");
}
echo("</ul>");
echo("</table><br><br><br>");

$pcurrow=0; $pmaxrow=pg_numrows($rpmain);
echo("<b><font size=3>postal addresses:</b></font><br><font size=2><a href=\"add-post.php3?pid=$pid\">add one</a></font>\n<ul>\n");
echo("<table cols=1 width=40% bgcolor=#EEEEEE NOSAVE>\n");
while($pcurrow<$pmaxrow) {
  if(($pcurrow%2)==1)
    { echo("<tr bgcolor=\"#DADADA\" nosave>\n<td nosave>\n"); }
  else
    { echo("<tr bgcolor=\"#F4F4F4\" nosave>\n<td nosave>\n"); }
  $prow=pg_fetch_array($rpmain, $pcurrow);
  echo("<li>");
  $rline=pg_exec("select ltext from post_lines where pmid=$prow[0]");
  $rcurrow=0; $rmaxrows=pg_numrows($rline);
  while($rcurrow<$rmaxrows) {
    $rrow=pg_fetch_array($rline, $rcurrow);
    echo("$rrow[0]<br>\n");
    $rcurrow++;
  }
  echo("$prow[2], $prow[3]<br> $prow[4], $prow[5]"); 
  echo("\n</li>");
  $pcurrow++;
  echo("</td>\n</tr>\n");
}
echo("</ul>");
echo("</table>");


?>

<?
include("dbclose.inc");
?>
