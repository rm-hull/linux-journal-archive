<?
include("config.inc");
$conn=pg_connect("dbname=addrbook host=$PG_HOST port=$PG_PORT user=$PG_USER password=$PG_PASS") 
	or die("<b>Database connection error</b>".
	        "</p>please notify ".
	        "<a href=\"mailto:$SERVER_ADMIN_EMAIL\">$SERVER_ADMIN_EMAIL</a>");
?>
