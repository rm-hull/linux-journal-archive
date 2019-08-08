#!/usr/bin/perl

use Crypt::SmbHash;
use Net::LDAP;
use Net::LDAP::Entry;
use Net::LDAP::LDIF;
use File::Temp;
use TDB_File;

# The LDAP DN to bind to the directory as
my $LDAP_DN="uid=samba_server,ou=people,o=ci,dc=example,dc=com";

# Where the Samba secrets file is kept
my $secrets_file="/var/lib/samba/private/secrets.tdb";

# This is the SID for our domain
my $SID="S-1-5-21-2162541494-3670296480-3949091320";

# LDAP server's hostname
my $LDAP_hostname="ldap://ldap.example.com";

# Our LDAP search criteria
my $base = "ou=hosts,ou=samba,o=ci,dc=example,dc=com";
my $filter = "(objectClass=posixAccount)";
my $attrs = ["uidNumber"];

# First we need to extract the DN's password from the secrets file
my $tdb = TDB_File->open( $secrets_file ) or die $!;
my $LDAP_password = $tdb->fetch( "SECRETS/LDAP_BIND_PW/$LDAP_DN" );
chop( $LDAP_password );

# Gather the host information. Strip the tailing '$' on the end as well
my $host = $ARGV[0];
my $host_alias = $host;
$host_alias =~ s/\$$//;

# Connect, StartTLS, and bind to the LDAP dircetory
my $ldap = Net::LDAP->new( $LDAP_hostname ) or die $@;
my $mesg = $ldap->start_tls( verify => 'require', capath => '/etc/ssl/certs', ciphers => 'HIGH' );
$mesg->code && die $mesg->error_text;

$mesg = $ldap->bind( "$LDAP_DN", password => $LDAP_password);
$mesg->code && $ldap->disconnect( ) && die $mesg->error_text;

my $result = $ldap->search( base => "$base", scope => "sub", filter => "$filter", attrs => "$attrs" );
$result->code && $ldap->unbind( ) && $ldap->disconnect( ) && die $result->error;

# Next we push all the UID numbers we received into an array
my @entries = $result->entries;
my $entry, @uids;
foreach $entry ( @entries )
{
	my $attr = $entry->get_value( 'uidNumber' );
	push @uids, $attr;
}

# Next we sort the array and determine the next highest available UID number
my @sorted_uids = sort { $a <=> $b } @uids;
my $last_uid = pop @sorted_uids;
my $next_uid = $last_uid + 1;

# Next we need to calculate the RID for our machine
my $RID = ( $next_uid * 2 ) + 1000;

# We generate a password for the machine
my ( $host_lmpasswd, $host_ntpasswd ) = ntlmgen( $host_alias );

# Now we define the entry we'll add to the directory
$entry = Net::LDAP::Entry->new;
$entry->dn( "uid=$host,$base" );
$entry->add (
	'objectClass' => "account",
	'objectClass' => "posixAccount",
	'objectClass' => "sambaSamAccount",
	'cn' => "$host",
	'uid' => "$host",
	'uidNumber' => "$next_uid",
	'gidNumber' => "65534",
	'homeDirectory' => "/dev/null",
	'loginShell' => "/bin/false",
	'sambaSID' => "$SID-$RID",
	'sambaLMPassword' => "$host_lmpasswd",
	'sambaNTPassword' => "$host_ntpasswd",
	'sambaPwdMustChange' => "2147483647",
	'sambaAcctFlags' => "[W          ]",
	'sambaPwdCanChange' => "1115058233",
	'sambaPwdLastSet' => "1115058233"
);

# Last we write the contents to the directory
$ldap->add( $entry );

# To be good citizens, unbind from the directory
$ldap->unbind;
