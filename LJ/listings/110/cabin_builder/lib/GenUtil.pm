#!/usr/bin/perl
package GenUtil;

BEGIN { push @INC, "/usr/local/ray/ulib" }

use Gen;
use Getopt::Std;
use RenderMan;

sub genlogwall
{
   @ARGV = @_;
   getopts("r");

   $genlog = \&Gen::genlog;
   $genlog = \&rmanlog if $opt_r;

   die "Usage: genlogwall material name width height logdiam holedata\n" if $#ARGV < 4;

   $material = $ARGV[0];
   $name = $ARGV[1];
   $width = $ARGV[2] * 12;
   $height = $ARGV[3] * 12;
   $diam = $ARGV[4];
   $rad = $diam / 2;

   $holefile = $ARGV[5];
   open HOLES, "<$holefile" or warn "No hole data file: $holefile.\n";

   $nholes = 0;
   while($data = <HOLES>) {
      next if $data =~ /^[ \t]*#/;
      next if $data !~ /[0-9]/;
      chomp $data;
      @vals = split /[ \t]/, $data;
      $vals{bottom} = $vals[0] * 12;
      $vals{top} = $vals[1] * 12;
      $vals{start} = $vals[2] * 12;
      $vals{width} = $vals[3] * 12;
      $holes[$nholes++] = { %vals };
   }

   close HOLES;

   # Sort the holes in increasing x.
   @holes = sort { $a->{start} <=> $b->{start} } @holes;

   $z = $rad;

   $n = $height / (2*$rad);

   if($n != int($n)) {
      warn "Height not divisible by log size for $name ($holefile). Wall will be slightly tall.\n";
      $n = int($n) + 1;
   }

   {
   my($bottom, $top);

   for($log = 1; $log <= $n; $log++) {
      # Find intersecting holes
      $x = 0;
      for($hole = 0; $hole < $nholes; $hole++) {

         $bottomhole = $holes[$hole]{bottom};
         $tophole = $holes[$hole]{top};
         $bottomlog = $z - $rad;
         $toplog = $z + $rad;

         if( ($bottomhole < $toplog && $tophole > $bottomlog) ||
        ($bottomhole < $toplog && $bottomhole > $bottomlog) ||
        ($tophole < $toplog && $tophole > $bottomlog))
         { # Hole breaks log
       &{$genlog}($material, "$name.$log", 
         [ $x, 0, $z, $holes[$hole]{start}, 0, $z ],
         $rad) if($holes[$hole]{start} - .2 > $x);
       if($bottomhole > $bottomlog && $bottomhole < $toplog) {
          &{$genlog}($material, "$name.tcut.$log", 
         [ $holes[$hole]{start}, 0, $z,
           $holes[$hole]{start} + $holes[$hole]{width}, 0, $z ],
         $rad, 0, $bottomhole-$z);
       } 
       if($tophole > $bottomlog && $tophole < $toplog) {
          &{$genlog}($material, "$name.bcut.$log", 
         [ $holes[$hole]{start}, 0, $z,
           $holes[$hole]{start} + $holes[$hole]{width}, 0, $z ],
         $rad, 0, $tophole-$z, "bottom");
       }
       $x = $holes[$hole]{start} + $holes[$hole]{width};
         }
      }
      &{$genlog}($material, "$name.$log", [ $x, 0, $z, $width, 0, $z ], 
        $rad) if $x < $width;
      $z += $diam;
   }

   }
}

sub rmanlog
{
   my($sx, $ex, $z, $rad) = ($_[2]->[0], $_[2]->[3], $_[2]->[2], $_[3]);

   TransformBegin;
      Translate($sx,0,$z);
      Rotate(90,0,1,0);
      Cylinder($rad, 0, $ex - $sx, 360.0);
   TransformEnd;
}

1;
