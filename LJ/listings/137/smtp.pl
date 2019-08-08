#!/usr/bin/perl -w
#
# Title  :   smtp.pl 
# Author :   John_Ouellette@yahoo.com
# Files  :   smtp.pl 
# Pupose :   Send email through SMTP server
#            Called from monitor_smtp.sh
#
# Submit as 

use Net::SMTP;

my $rcpt   = $ARGV[2] || 'mygroup@somewhere';
my $sender = $ARGV[1] || 'root@host01';
my $host   = $ARGV[0];


#Start Script 

my $smtp =Net::SMTP->new($host, Debug => 1);
my $input="test msg for server $host";

$smtp->mail("$sender");
$smtp->to("$rcpt");
$smtp->data();
$smtp->datasend("To: $rcpt\n") ; 
$smtp->datasend("From: $sender\n") ; 
$smtp->datasend("Subject: $host test\n") ; 
$smtp->datasend("$input");
$smtp->dataend();
$smtp->quit;


