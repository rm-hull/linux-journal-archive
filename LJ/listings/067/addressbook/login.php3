<?
include("dbopen.inc");
if(isset($user)) {
  $verify=pg_exec($conn, "select * from luser where ulogin='$user' and upass='$pass'");
  if(pg_numrows($verify)>0) {
    $vrow=pg_fetch_array($verify, 0);
    $uid=$vrow[0];
    setcookie("addressbook_user", $uid, 0, "/", "", 0);
    //    $redir_to="index.php3";
    //    include("redir.php3");
    echo("<title>Welcome, $user</title>\n");
    include("index.php3");
    exit();
  } else {
    echo("<title>Invalid login or password</title>\n");
    echo("<body bgcolor=\"#ffffff\">\n");
    echo("You can create an account or retry.");
  }
}
?>
<title>Log in to addressbook</title>
<body bgcolor="#ffffff">
<form action="login.php3" method=post>
login: <input type=text name=user> <br>
password: <input type=password name=pass> <br>
<input type=submit value="log in">
</form>

<?
include("dbclose.inc");
?>

</body>
