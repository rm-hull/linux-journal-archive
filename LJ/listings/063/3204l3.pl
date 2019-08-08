@lay:3204l3.pl

 1 #!/usr/local/bin/perl
 2
 3 use Sybase::CTlib;
 4
 5 $ENV{'SYBASE'} = '/opt/sybase';
 6 $ENV{'DSQUERY'} = 'linux_dev';
 7
 8 $dbh =
Sybase::CTlib->ct_connect('sa','password','linux_dev');
 9
10 $sql = "sp_who";
11 $qref = $dbh->ct_sql($sql);
12 <\@>rows = <\@>$qref;
13
14 print "Content-Type: text/html\n\n";
15 print "<HTML><HEAD><TITLE>Who's logged
in</TITLE></HEAD>\n";
16
17 print "<BODY>\n";
18 print "<TABLE BORDER>\n";
19 print
"<TR><TH>ID</TH><TH>Status</TH><TH>Login</TH>\n";
20 print
"<TH>Host</TH><TH>Block</TH><TH>Database</TH>\n";
21 print "<TH>Command</TH></TR>\n";
22
23 foreach $row (<\@>rows) {
24 	print "<TR>\n";
25	print "
<TD>$$row[0]</TD><TD>$$row[1]</TD>\n";
26	print "
<TD>$$row[2]</TD><TD>$$row[3]</TD>\n";
27	print "
<TD>$$row[4]</TD><TD>$$row[5]</TD>\n";
28	print " <TD>$$row[6]</TD>\n";
29	print "</TR>\n";
30 }
31 print "</TABLE>\n";
32 print "</BODY>\n";
33 print "</HTML>\n";
