<title>Add a new account</title>
<body bgcolor=#ffffff>
<?php
include("dbopen.inc");
if(!isset($add_name)) {
?>
<form action="new-account.php3" method=post>
login <input type=text name=add_name size=8>
email <input type=text name=add_email size=8>
password <input type=password name=add_pass size=8>
<input type=submit value="Add account">
</form>
<?
} else {
$luser_r=pg_exec($conn, "select * from luser where ulogin='$add_name'");
$luser_exists=pg_numrows($luser_r);
 if($luser_exists) {
   echo("<h3>Sorry!</h3>\n<p>\nWe already have a user here named $add_name.  You are more than welcome to <a href=\"new-account.php3\">pick a new login name</a>, though.\n</body>");
 } else {
   $nluser=pg_exec($conn, "insert into luser (ulogin, upass, email, fname, lname) values ('$add_name', '$add_pass', '$add_email', 'web', 'web');");
   mail("$add_email", "new addressbook user at $SERVER_NAME", "Welcome!\nYou now have an addressbook account at $SERVER_NAME.\nHere's what you need to know:\n  login: '$add_name'\n   pass: '$add_pass'\n\n", "Reply-to: webmaster@$SERVER_NAME\nX-Mailer: Addressbook service at $SERVER_NAME\nFrom: Addressbook Service <webmaster@$SERVER_NAME>");
   echo("Welcome to the addressbook service, $add_name!<p>We have sent an email confirming your account to $add_email.<p>You may now <a href=\"login.php3\">log in</a>.</body>");
 }
?>

<?
}
include("dbclose.inc");
?>
