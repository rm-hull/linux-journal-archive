<?
if(!isset($sid)) { Header("Location: login.php3"); }
include("dbopen.inc");
$uinfo=s_lookup($sid);
if($uinfo) {
  s_renew($sid);
  $auth_user=$uinfo[0];
  $auth_pw=$uinfo[1];
  include("data-close.inc");
} else {
  include("data-close.inc");
  Header("Location: login.php3?expired=1");
}
?>
