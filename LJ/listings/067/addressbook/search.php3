<?
include("dbopen.inc");
include("checkcookie.inc");
if (!isset($search)) {
  echo("<title>Search your addressbook</title>\n");
} else {
  echo("<title>Search results for $sstring</title>\n");
}
?>
<body bgcolor=#ffffff>
<?
   if(!isset($search)) {
     if(!isset($stype)) {
?>

<ul>
<li> search for <a href="search.php3?stype=name">a name</a>
<li> search for <a href="search.php3?stype=phone">a phone number</a>
<li> search for <a href="search.php3?stype=email">an email</a>
</ul>
<?
     } /*foo*/ else {

       switch($stype) {
	 case name:
?>
<h3>Search for a name:</h3><p>
<form action="search.php3">
<input type=hidden name=search value=1>
<input type=hidden name=stype value=name>
name (first or last): <input type=text name=sstring>
<input type=submit value="Search for name">
</form>
<hr width=50%>
<?
break;

	 case phone:
?>
<h3>Search for a phone number:</h3><p>
<form action="search.php3">
<input type=hidden name=search value=1>
<input type=hidden name=stype value=phone>
phone number: <input type=text name=sstring>
<input type=submit value="Search for phone">
</form>
<hr width=50%>
<?
break;

	 case email:
?>

<h3>Search for an email address or domain :</h3><p>
<form action="search.php3">
<input type=hidden name=search value=1>
<input type=hidden name=stype value=email>
email or domain: <input type=text name=sstring>
<input type=submit value="Search for email">
</form>
<hr width=50%>
<?
break;

	 case state:
?>

<h3>Search for a state:</h3><p>
<form action="search.php3">
<input type=hidden name=search value=1>
<input type=hidden name=stype value=state>
state: <select name=sstring>
  <option value=0>select a state</option>
  <option value=0>(none)</option>
<?
$states=pg_exec($conn, "select sid, sname from state order by sname");
$scurrow=0;
$smaxrow=pg_numrows($states);
 while($scurrow<$smaxrow) {
   $row=pg_fetch_array($states, $scurrow);
   echo("<option value=\"$row[0]\">$row[1]</option>\n");
   $scurrow++;
 }
?>
</select>
<input type=submit value="Search for state">
</form>
<hr width=50%>
<?
break;

	 case postcode:
?>

<h3>Search for a zip code:</h3><p>
<form action="search.php3">
<input type=hidden name=search value=1>
<input type=hidden name=stype value=postcode>
zip: <input type=text name=sstring>
<input type=submit value="Search for zip code">
</form>
<hr width=50%>
<?
break;
       }
     }
   
   } else {
  // doin' the search
  $slower=strtolower($sstring);
  switch ($stype) {
  case name:
    $people=pg_exec($conn, "select pid, fname, minit, lname from person where uid=$addressbook_user and ( lower(fname) like '%$slower%' or lower(lname) like '%$slower%')");
    break;
  case email:
    $people=pg_exec($conn, "select person_id, first_name, middle, last_name from email_addresses where user_id=$addressbook_user and lower(email) like '%$slower%'");
    break;
  case phone:
    $people=pg_exec($conn, "select person_id, first_name, middle, last_name from phone_numbers where user_id=$addressbook_user and phone_num like '%$sstring%'");
    break;
  case state:
    $people=pg_exec($conn, "select person_id, first_name, middle, last_name from post_addresses where user_id=$addressbook_user and sid=$sstring group by pid");
    break;
  case state:
    $people=pg_exec($conn, "select person_id, first_name, middle, last_name from post_addresses where user_id=$addressbook_user and lower(postal_code) like '%$slower%' group by pid");
    break;
  }

$pcurrow=0;     
$pnumrows=pg_numrows($people);
if ($pnumrows==0) {
  echo("\n<p><b>No matches to your query</b></p>\n");
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
      echo("<a href=\"view-person.php3?pid=$row[0]\">$row[1] $row[2] $row[3]</a></p>");
      echo("</td>\n</tr>\n");
      $pcurrow++;
    }
  echo("</table>");



?>
 
<?
   }
   }
?>

		   
