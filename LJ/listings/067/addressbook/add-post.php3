<?
$addtype="a postal address";
include("add-begin.inc");
// this opens up a form for us
if(!isset($add)) {
?>
postal address:<br><blockquote> <input type=text name=line1 size=30></br>
  <input type=text name=line2 size=30></br>
  <input type=text name=line3 size=30></br>
</blockquote>
  city: <input type=text name=city size=15>
  <select name=sid> 
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

  <select name=cid value=2> 
<?
$countries=pg_exec($conn, "select cid, cname from country order by cname");
$ccurrow=0;
$cmaxrow=pg_numrows($countries);
 while($ccurrow<$cmaxrow) {
   $row=pg_fetch_array($countries, $ccurrow);
   echo("<option value=\"$row[0]\">$row[1]</option>\n");
   $ccurrow++;
 }
?>
</select><br>
    postcode: <input type=text name=postcode size=10><br>

<input type=submit value="add record">
</form>
<?
} else {
  pg_exec($conn, "insert into post_main (pid, city, sid, cid, postcode) values ($pid, '$city', $sid, $cid, '$postcode')");
  $id=pg_exec($conn, "select aid from post_main where pid=$pid");
  $pmidrow=pg_fetch_array($id, pg_numrows($id)-1);
  $pmid=$pmidrow[0];
  if($line1!="") {
    pg_exec($conn, "insert into post_lines (pmid, ltext) values ($pmid, '$line1')");
    if($line2!="") {
      pg_exec($conn, "insert into post_lines (pmid, ltext) values ($pmid, '$line2')");
      if($line3!="") {
	pg_exec($conn, "insert into post_lines (pmid, ltext) values ($pmid, '$line3')");
      }
    }
  }
  include("dbclose.inc");
  header("Location: view-person.php3?pid=$pid\n\n");
  exit();
}
include("add-end.inc");

?>
