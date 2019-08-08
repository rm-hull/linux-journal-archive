<title>Address book service</title>
<body bgcolor="#ffffff">
<p><?php
include("dbopen.inc");
if(isset($addressbook_user))
{
  $luser=pg_exec($conn, "select ulogin from luser where uid=$addressbook_user");
  $lname=pg_fetch_array($luser, 0);
  echo("Hello, $lname[0].  You have several options:<br>\n<font size=2>(If you're not $lname[0], <a href=\"login.php3\">click here</a> to log in as yourself.)</font>");
  echo("<ul>\n  <li>\n   You may <a href=\"my-addressbook.php3?mode=view\">view or edit your addressbook</a>.\n  </li>\n  <li>\n   You may <a href=\"add-addressbook-entries.php3\">add entries to your addressbook.</a>\n  </li>\n  <li>\n   Or you may <a href=\"search.php3\">search your addressbook</a>\n  </li>\n</ul>");
} else {
  echo("<table cols=1 width=\"60%\" nosave><tr><td><p>To use this service, you will need to <a href=\"login.php3\">log in</a>; if you don't have an account here, you'll need to <a href=\"new-account.php3\">create one</a>, but it's quick and painless.</tr></td></table>");
}
include("dbclose.inc");
?>
<hr>
If you have comments about this page, please contact <a href="mailto:<?echo $SERVER_ADMIN_EMAIL;?>"><? echo $SERVER_ADMIN_REALNAME; ?></a><address><?echo $SERVER_ADMIN_EMAIL;?></address>
