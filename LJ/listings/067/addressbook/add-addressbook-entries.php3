<?
include("checkcookie.inc");
include("dbopen.inc");
?>
<? 
if (!isset($add)) {
  echo("<title>");
  echo("Add a person");
  echo("</title>");
} else {
  pg_exec($conn, "insert into person (uid, fname, minit, lname) values ($addressbook_user, '$fname', '$minit', '$lname')"); 
  include("dbclose.inc");
  header("Location: my-addressbook.php3?mode=view\n\n");
  exit();
}
?>
<body bgcolor=#ffffff link=#000000 alink=#151515 vlink=#000000>
<?
if (!isset($add)) {
?>
<form action="add-addressbook-entries.php3" method=post>
<input type=hidden name=add value=1>
<?
echo("<input type=hidden name=uid value=$addressbook_user>\n");
?>
first name: <input type=text name=fname size=8>
middle initial: <input type=text name=minit size=5>
last name: <input type=text name=lname size=12>
<br>
<input type=submit value="add person">
</form>
<?
}
?>

</body>

<?
include("dbclose.inc");
?>
