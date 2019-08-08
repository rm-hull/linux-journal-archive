<?
include("checkcookie.inc");
$uid=$addressbook_user;
include("dbopen.inc");
if($mode="view") {
  $people=pg_exec($conn, "select * from person where uid=$uid order by lname, fname, minit");
}
echo("<title>Address book</title>\n<body bgcolor=\"#ffffff\" link=\"#000000\" alink=\"#151515\" vlink=\"#000000\">\n");
?>
<p><h3>People you know:</h3></p>
<?
$pcurrow=0;     
$pnumrows=pg_numrows($people);
if ($pnumrows==0) {
  echo("\n<p><b>No entries in your addressbook</b></p>\n");
} else {
  echo("<table cols=1 width=40% bgcolor=#EEEEEE NOSAVE>\n");
  while($pcurrow<$pnumrows) 
    {
      if(($pcurrow%2)==1)
	{
	  echo("<tr bgcolor=\"#DADADA\" nosave>\n<td nosave>\n");
	}
      else
	{
	  echo("<tr bgcolor=\"#F4F4F4\" nosave>\n<td nosave>\n");
	}
      $row=pg_fetch_array($people, $pcurrow);
      echo("<a href=\"view-person.php3?pid=$row[0]\">$row[2] $row[3] $row[4]</a></p>");
      echo("</td>\n</tr>\n");
      $pcurrow++;
    }
  echo("</table>");
}
?>
<p><a href="add-addressbook-entries.php3">Add a person</a> to your addressbook.</p>
<?
include("dbclose.inc");
?>


