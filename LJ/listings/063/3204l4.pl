@lay:3204l4

 1 #!/usr/bin/perl
 2 use Sybase::CTlib;
 3 ct_callback(CS_CLIENTMSG_CB, \&msg_cb);
 4 ct_callback(CS_SERVERMSG_CB, "srv_cb");
 5 $uid = 'sa'; $pwd = 'password'; $srv = 'linux_dev';
 6 $X = Sybase::CTlib-<\>>ct_connect($uid, $pwd, $srv);
 7 $X-<\>>ct_execute("select * from sysusers");
 8 while(($rc = $X-<\>>ct_results($restype)) == CS_SUCCEED) {
 9    next if($restype == CS_CMD_DONE || $restype == CS_CMD_FAIL ||
10            $restype == CS_CMD_SUCCEED);
11    if(@names = $X-<\>>ct_col_names()) {
12         print "@names\n";
13    }
14    if(@types = $X-<\>>ct_col_types()) {
15         print "@types\n";
16    }
17    while(@dat = $X-<\>>ct_fetch) {
18         print "@dat\n";
19    }
20 }
21 print "End of Results Sets\n" if($rc == CS_END_RESULTS);
22 print "Error!\n" if($rc == CS_FAIL);
23 sub msg_cb {
24     my($layer, $origin, $severity, $number, $msg, $osmsg) = @_;
25     printf STDERR "\nOpen Client Message: (In msg_cb)\n";
26     printf STDERR "Message number: LAYER = (%ld) ORIGIN = (%ld) ",
27            $layer, $origin;
28     printf STDERR "SEVERITY = (%ld) NUMBER = (%ld)\n",
29            $severity, $number;
30     printf STDERR "Message String: %s\n", $msg;
31     if (defined($osmsg)) {
32         printf STDERR "Operating System Error: %s\n", $osmsg;
33     }
34     CS_SUCCEED;
35 }
36 sub srv_cb {
37     my($cmd, $number, $severity, $state, $line, $server,
38        $proc, $msg) = @_;
39     printf STDERR "\nServer message: (In srv_cb)\n";
40     printf STDERR "Message number: %ld, Severity %ld, ",
41            $number, $severity;
42     printf STDERR "State %ld, Line %ld\n", $state, $line;
43     if (defined($server)) {
44         printf STDERR "Server '%s'\n", $server;
45     }
46     if (defined($proc)) {
47         printf STDERR " Procedure '%s'\n", $proc;
48     
49     printf STDERR "Message String: %s\n", $msg;  CS_SUCCEED;
50 }
