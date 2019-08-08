<?
$addtype="phone";
include("add-begin.inc");
// this opens up a form for us
if(!isset($add)) {
?>
phone: <input type=text name=countrycode size=3 value=1> <font size=10>(</font><input type=text name=areacode size=4><font size=10>)</font> <input type=text name=phone size=10> x<input type=text name=ext size=5></br><input type=submit value="add record">
</form>
<?
} else {
  pg_exec($conn, "insert into telephone (pid, countrycode, areacode, phone, ext) values ($pid, '$countrycode', '$areacode', '$phone', '$ext')");
  include("dbclose.inc");
  header("Location: view-person.php3?pid=$pid\n\n");
  exit();
}
include("add-end.inc");

?>
