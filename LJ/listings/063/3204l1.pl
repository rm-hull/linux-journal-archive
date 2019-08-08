@lay:3204l1.pl 
 
 1 #!/usr/local/bin/perl
 2
 3 use Sybase::CTlib;
 4
 5 $dbh =
Sybase::CTlib->ct_connect('sa','password','linux_dev');
 6
 7 $sql = "select name from sysdatabases";
 8
 9 $qref = $dbh->ct_sql($sql);
10 @rows = @$qref;
11
12 foreach $row (@rows) {
13 	print "$$row[0]\n";
14 }
