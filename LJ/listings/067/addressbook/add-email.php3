<?
$addtype="email";
include("add-begin.inc");
// this opens up a form for us
if(!isset($add)) {
?>
email: <input type=text name=email> <input type=submit value="add record">
</form>
<?
} else {
  pg_exec($conn, "insert into email (pid, address) values ($pid, '$email')");
  include("dbclose.inc");
  header("Location: view-person.php3?pid=$pid\n\n");
  exit();
}
include("add-end.inc");

?>
