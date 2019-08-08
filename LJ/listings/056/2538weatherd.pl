#!/usr/bin/perl
#
# weatherd.pl
#

use FileHandle;

$DATADIR="/home/weather/data";

if ($#ARGV < 0) { die "Usage: weatherd tty\n"; }
$TTY = $ARGV[0];

system("stty `cat /home/weather/bin/tty` < $TTY");
print "stty returned $?\n";

# Filename for today's data
($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) =
	localtime(time);
$TODAY= sprintf "%02d%02d%02d", $mon+1, $mday, $year;

print "today is $TODAY\n";

$OUTFILE = "$DATADIR/$TODAY";
open(LINE, "< $TTY") or die "Unable to open tty: $TTY";
open(OUT, ">> $OUTFILE") or die "Unable to open output file: $OUTFILE";

LINE->autoflush(1);
OUT->autoflush(1);

while(<LINE>)
{
	#
	# parse out date, make sure it matches $TODAY
	#
	($time, $date, $wdir, $wspeed, $aux, $intemp,
		$outtemp, $hum, $bp, $raind, $rainm, $rain_rate) = split;

	$date =~ s/\///g;

	if ( $time =~ /Min/ ) {
		#system(" echo \'$_\' > $DATADIR/$date.min ");

		open(MIN, ">> $DATADIR/$date.min")
		  or die "Unable to open output file: $DATADIR/$date.min";
		printf  MIN "%s", $_;
		close MIN;
	}
	elsif ( $time =~ /Max/ ) {
		#system(" echo \'$_\' > $DATADIR/$date.max ");

		open(MAX, ">> $DATADIR/$date.max")
		  or die "Unable to open output file: $DATADIR/$date.max";
		printf  MAX "%s", $_;
		close MAX;
	}
	else
	{
		$date =~ s/\///g;
#print "TODAY is $TODAY,  today is $date\n";

		if( $TODAY eq $date )
		{
			printf OUT "%s",$_; 
		}
		else
		{
			$TODAY = $date;

			# if not, close this file and open the next one
			close OUT;
			$OUTFILE = "$DATADIR/$TODAY";
			open(OUT, ">> $OUTFILE")
				or die "Unable to open output file: $OUTFILE";
	
			OUT->autoflush(1);
			printf OUT "%s",$_; 
		}
	}
}
