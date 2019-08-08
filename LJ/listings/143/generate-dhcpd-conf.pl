#!/usr/bin/perl

use Net::LDAP;
use Net::LDAP::Entry;
use Net::LDAP::LDIF;

my $ldap = Net::LDAP->new( "ldap://ldap.example.com" ) or die $@;
my $mesg = $ldap->start_tls( verify => 'require', capath => '/etc/ssl/certs', ciphers => 'HIGH' );
$mesg->code && $mesg->error_test;

$mesg = $ldap->bind( );
$mesg->code && $ldap->disconnect( ) && dia $mesg->error_text;

my $base = "ou=hosts,o=ci,dc=uchicago,dc=edu";
my $filter = "(&(objectClass=ipHost)(macAddress=*)(ipHostNumber=*))";
my $atts = ["cn", "macAddress", "ipHostNumber"];
my $result = $ldap->search( base => "$base", scope => "sub", filter => "$filter", attrs => "$attrs" );
$result->code && $ldap->unbind( ) && $ldap->disconnect( ) && die $result->error;

open( DHCPD_CONF, ">/tmp/dhcpd.conf" ) or die $@;
print( DHCPD_CONF "option domain-name-servers 192.168.1.2;\n" );
print( DHCPD_CONF "ddns-update-style none;\n" );
print( DHCPD_CONF "\n" );
print( DHCPD_CONF "subnet 192.168.1.0 netmask 255.255.255.0 {\n" );
print( DHCPD_CONF "\trange dynamic-bootp 192.168.1.200 192.168.1.250;\n" );
print( DHCPD_CONF "\toption domain-name \"example.com\";\n" );
print( DHCPD_CONF "\toption routers 192.168.1.1;\n" );
print( DHCPD_CONF "\toption subnet-mask 255.255.255.0;\n" );
print( DHCPD_CONF "\toption broadcast-address 192.168.1.255;\n" );
print( DHCPD_CONF "\tdefault-lease-time 600;\n" );
print( DHCPD_CONF "\tmax-lease-time 7200;\n" );
print( DHCPD_CONF "\n" );

my @entries = $result->entries;
my $entry, %hosts;


foreach $entry ( @entries )
{
	my $host, $mac, $ip;
	$host = $entry->get_value( 'cn' );
	$mac = $entry->get_value( 'macAddress' );
	$ip = $entry->get_value( 'ipHostNumber' );
	if ( $ip =~ /^192.168.1/ )
	{
		print( DHCPD_CONF "\thost $host {\n" );
		print( DHCPD_CONF "\t\toption domain-name \"example.com\";\n" );
		print( DHCPD_CONF "\t\thardware ethernet $mac;\n" );
		print( DHCPD_CONF "\t\tfixed-address $ip;\n" );
		print( DHCPD_CONF "\t}\n" );
		print( DHCPD_CONF "\n" );
	}
}

print( DHCPD_CONF "}\n" );

close( DHCPD_CONF );

$ldap->unbind( );
$ldap->disconnect( );
