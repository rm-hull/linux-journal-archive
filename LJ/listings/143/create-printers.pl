#!/usr/bin/perl

use Net::LDAP;
use Net::LDAP::Entry;
use Net::LDAP::LDIF;
use IO::Socket::SSL;
use FileHandle;

# LDAP information we'll need
my $host_uri="ldap://ldap.example.com";
my $base_dn="o=ci,dc=example,dc=com";
my $printer_suffix="ou=printers";
my $printer_filter="(objectClass=printerIPP)";
my $host_attrs="[printer-name printer-location printer-info printer-make-and-model printer-uri]";
my $use_starttls="1";

# CUPS information we'll need
my $lpadmin_path="/usr/sbin/lpadmin";

# Connect, StartTLS, and bind to the LDAP dircetory
my $ldap = Net::LDAP->new( $host_uri ) or die $@;
if ( $use_starttls )
{
	my $mesg = $ldap->start_tls( verify => 'require', capath => '/etc/ssl/certs', ciphers => 'HIGH' );
	$mesg->code && die $mesg->error_text;
}

$mesg = $ldap->bind( );
$mesg->code && $ldap->disconnect( ) && die $mesg->error_text;

my $base="$printer_suffix,$base_dn";
my $result = $ldap->search( base => "$base", scope => "one", filter => "$printer_filter", attrs => "$printer_attrs" );
$result->code && $ldap->unbind( ) && $ldap->disconnect( ) && die $result->error;


my $printer_fh = new FileHandle "lpadmin_script.sh", O_WRONLY|O_CREAT;

# Next we get the services associated with each host
my @printers = $result->entries;
my $printer;
foreach $printer ( @printers )
{
	my $printer_name = $printer->get_value( 'printer-name' );
	my $printer_location = $printer->get_value( 'printer-location' );
	my $printer_driver = $printer->get_value( 'printer-info' );
	my $printer_description = $printer->get_value( 'printer-make-and-model' );
	my $printer_uri = $printer->get_value( 'printer-uri' );

	print $printer_fh "$lpadmin_path -p $printer_name -E -v $printer_uri -m $printer_driver -D \"$printer_description\" -L \"$printer_location\"\n";
}

$printer_fh->close;

$ldap->unbind( );
$ldap->disconnect( );

exit 0;
