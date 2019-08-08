@lay:3204l2.pl

 1 #!/usr/local/bin/perl
 2
 3 use Sybase::CTlib;
 4
 5 $dbh =
Sybase::CTlib->ct_connect('sa','password','linux_dev');
 6
 7 $dbh->ct_sql("use pubs2");
 8
 9 $sql = "insert into discounts values ('Customer
Discount','5023',1000,2000,6)";
10 $dbh->ct_sql($sql);
