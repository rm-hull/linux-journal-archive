#!/usr/bin/perl

use strict;
use Crypt::SmbHash qw( nthash );
use Term::ReadKey;

ReadMode 2;
print "Password: ";
my $password1 = ReadLine( 0 );
print "\nAgain: ";
my $password2 = ReadLine( 0 );
ReadMode 0;

chomp $password1;
chomp $password2;
print "\n";

if ( $password1 ne $password2 )
{
	print "Passwords do not match\n";
	undef $password1;
	undef $password2;
	exit 1;
}

# We generate a password for the machine
my $ntpassword = nthash( $password1 );
undef $password1;
undef $password2;

print "$ntpassword";
undef $ntpassword;

my @command = ( "chmod", "0400", "$ENV{'HOME'}/foohash" );
system( @command );

exit 0;
