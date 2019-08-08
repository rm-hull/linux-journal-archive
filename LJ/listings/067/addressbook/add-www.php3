<?
$addtype="www";
include("add-begin.inc");
// this opens up a form for us
if(!isset($add)) {
?>
www address: <input type=text name=url size=30 value="http://"></br>
<input type=submit value="add record">
</form>
<?
} else {
  pg_exec($conn, "insert into www_url (pid, url) values ($pid, '$url')");
  include("dbclose.inc");
  header("Location: view-person.php3?pid=$pid\n\n");
  exit();
}
include("add-end.inc");

?>
