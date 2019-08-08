#!/usr/bin/perl -w

package Gen;

use POSIX qw(math_h);

require Exporter;

@ISA = qw(Exporter);

@EXPORT = ();
@EXPORT_OK = qw(genwindow genlog genframe genknob gendoor);
%EXPORT_TAGS = ( subs => [ qw(genlog genframe) ] );

# material, name, [ sx,sy,sz, ex,ey,ez ], radius, [cap_flag (def=1)], 
# [cut_height (def none)] [cut_orient top|bottom (def: "top")]
sub genlog($$$$;$$$) 
{
   my($mat, $name, $pts, $r, $cap, $cut, $cut_orient) = @_;
   my(%p) = ();

   ($p{sx}, $p{sy}, $p{sz}, $p{ex}, $p{ey}, $p{ez}) = @$pts;

   @svec = ( $p{ex} - $p{sx},
             $p{ey} - $p{sy},
             $p{ez} - $p{sz} );
   @evec = ( $p{sx} - $p{ex},
             $p{sy} - $p{ey},
             $p{sz} - $p{ez} );

   $cap = 1 if !defined($cap);

   if(defined($cut) && ($p{sy} != $p{ey} || $p{sz} != $p{ez})) {
         warn "LOG $name MUST BE ALONG X AXIS TO CUT. Log NOT cut.\n";
         undef $cut;
   }
   if(defined($cut)) {
      if($cut > $r || $cut < -$r) {
         warn "Cut height is not within the log $name.\n";
         undef($cut);
      }
   }

   if(!defined($cut)) {
      # Generate a standard log (no cut)

      print <<LOG;

         $mat cylinder $name
         0
         0
         7   
                 $p{sx} $p{sy} $p{sz}
                 $p{ex} $p{ey} $p{ez}
                 $r
LOG
      if($cap) {

         print <<CAPS;

         $mat ring ${name}_cap1
         0
         0
         8
            $p{sx} $p{sy} $p{sz}
            @svec
            0 $r
         
         $mat ring ${name}_cap2
         0
         0
         8
            $p{ex} $p{ey} $p{ez}
            @evec
            0 $r
CAPS
      }
   } else {
      # Generate a trimmed log (ALONG X AXIS)
      # gensurf mat name 'len * s' 'sin((2*end+PI)*t - (PI+end))' 'cos(mess)'
      #         1 10 -s

      my($len, $end_angle, $PI);
      $PI = 3.1415927;

      $len = $p{ex} - $p{sx};
      $end_angle = asin($cut / $r);
      $start_angle = -$PI - $end_angle;
      $orient = 0;

      $cut_orient = "top" if !defined($cut_orient);
      $orient = 180 if $cut_orient eq "bottom";
      $flip = 1;
      $flip = -1 if $cut_orient eq "bottom";

      @geom = qx{gensurf $mat $name '$len*s' '$r * cos((2*$end_angle+$PI)*t + $start_angle)' '$r * sin((2*$end_angle+$PI)*t + $start_angle)' 1 10 -s | xform -rx $orient -t $p{sx} $p{sy} $p{sz}};
      print @geom;

      # Cap top of log
      my @pts = ( 
         $p{sx}, $p{sy}+$r * cos($start_angle), $p{sz}+$r * $flip * sin($start_angle),
         $p{sx}, $p{sy}+$r * cos($end_angle), $p{sz}+$r * $flip * sin($end_angle),
         $p{ex}, $p{sy}+$r * cos($end_angle), $p{sz}+$r * $flip * sin($end_angle),
         $p{ex}, $p{sy}+$r * cos($start_angle), $p{sz}+$r * $flip * sin($start_angle)
         );
      print "$mat polygon ${name}_top 0 0 12 @pts\n";
      print "\n";

      # Cap ends of log if needed
      if($cap) {
         my ($t, $x, $z, $y);
         $x = $p{sx};
         print "\n$mat polygon ${name}_cap1\n0\n0\n33";
         for($t = 0; $t <= 1.0; $t += 0.1) {
            $y = $r * cos((2*$end_angle + $PI) * $t + $start_angle);
            $z = $r * sin((2*$end_angle + $PI) * $t + $start_angle);
            print "   $x $y $z\n";
         }
         $x = $p{ex};
         print "\n$mat polygon ${name}_cap2\n0\n0\n33";
         for($t = 1.0; $t >= 0; $t -= 0.1) {
            $y = $r * cos((2*$end_angle + $PI) * $t + $start_angle);
            $z = $r * sin((2*$end_angle + $PI) * $t + $start_angle);
            print "   $x $y $z\n";
         }
      }
   }
}

# genframe material name width_in height_in depth_in plank_thick_in bottom_flag
# Generate a frame at 0,0,0
#   If you are standing in -y, looking towards +y, +x is right, and +z is up.
#   The frame expands into positive x (right of frame) positive y (back of frame) and z (top of frame)
sub genframe($$$$$$$)
{
   my($mat, $name, $width, $height, $depth, $plankt, $bflag) = @_;

if($bflag > 0) {
   # Left side
   $ls = "genbox ". $mat." ". $name. " ". $plankt. " ". $depth. " ". ($height-2*$plankt). "| xform -t 0 0 $plankt";
   # Right side
   $rs = "genbox ". $mat." ". $name. " ". $plankt. " ". $depth. " ". ($height-2*$plankt). "| xform -t ". ($width - $plankt) . " 0 $plankt";
   # Top
   $ts = "genbox ". $mat." ". $name. " ". $width. " ". $depth. " ". $plankt . "| xform -t 0 0 ". ($height - $plankt);
   # Bottom
   $bs = "genbox ". $mat." ". $name. " ". $width. " ". $depth. " ". $plankt;
   system($ls);
   system($rs);
   system($ts);
   system($bs);
} else {
   # Left side
   $ls = "genbox ". $mat." ". $name. " ". $plankt. " ". $depth. " ". ($height-$plankt);
   # Right side
   $rs = "genbox ". $mat." ". $name. " ". $plankt. " ". $depth. " ". ($height-$plankt). "| xform -t ". ($width - $plankt) . " 0 0";
   # Top
   $ts = "genbox ". $mat." ". $name. " ". $width. " ". $depth. " ". $plankt . "| xform -t 0 0 ". ($height - $plankt);
   system($ls);
   system($rs);
   system($ts);
}

}

# gendoor door_mat knob_mat name width_in height_in door_thk_in 
# Generate a door with a frame at 0,0,0
#   If you are standing in -y, looking towards +y, then +x is right, and +z is up.
#   The door expands into positive x (right of door) positive y (backside of door) and z (top of door)
# Knob is on the "right"
sub gendoor($$$$$$)
{
   my($doormat, $knobmat, $name, $w, $h, $doorthk) = @_;

   # Generate door
   system("genbox $doormat $name $w $doorthk $h");
   # Generate the doorknobs
   genknob($knobmat, $name . "_knob1", $w - 4, 0 ,36, -1);
   genknob($knobmat, $name . "_knob2", $w - 4, $doorthk ,36, 1);
}

# Generate a doorknob along the y-axis
# Args: mat, name, x, y, z (base of shaft), facey (-1=gen toward -y +1=gen toward +y)
sub genknob($$$$$$)
{
   my($mat, $name, $x, $y, $z, $facey) = @_;
   my $ty;

   if($facey > 0) {
      $ty = $y+.01;
      print <<EOF;
$mat ring ${name}_plate
0
0
8
   $x $ty $z
   0 1 0
   .8 1.5
EOF
      $ty = $y + .5;
      print <<EOF2;
$mat cone ${name}_stem
0
0
8
   $x $y $z
   $x $ty $z
   .8 .3
EOF2
      $ty = $y + 1.4;
      print <<EOF3;
$mat sphere ${name}_knob
0
0
4
   $x $ty $z 1
EOF3

   } else {
      $ty = $y-.01;
      print <<EOF4;
$mat ring ${name}_plate
0
0
8
   $x $ty $z
   0 -1 0
   .8 1.5
EOF4
      $ty = $y - .5;
      print <<EOF5;
$mat cone ${name}_stem
0
0
8
   $x $y $z
   $x $ty $z
   .8 .3
EOF5
      $ty = $y - 1.4;
      print <<EOF6;
$mat sphere ${name}_knob
0
0
4
   $x $ty $z 1
EOF6
   }

}

# genwindow frmmat glassmat name width_in height_in frmthickness_in 
#           glassthickness_in npanes_x npanes_z
# Generate a multi-paned window. Width & weight must be > 10in
#  +----+----+
#  |    |    |  +z
#  +----+----+ /|\  This would be a 2x2 paned window width:height ratio of about 2:1
#  |    |    |  |
#  +----+----+  --> +x
#
# NOTE: "inside" of window is towards +y. If you don't orient it correctly, daylight sims
#       won't work right.
sub genwindow($$$$$$$$)
{
   my($fmat, $gmat, $name, $w, $ht, $frmt, $npx, $npz) = @_;
   my($panew, $paneh, $offsetx, $offsety, $msz, $i, $j);

   if($w < 10 || $ht < 10) {
      warn "Window $name: ($w x $ht) is too small; must be at least 10\"x10\"...no geometry output\n";
      return;
   }

   $offsetx = 1; # Offsets to account for border
   $offsetz = 1;
   $msz = $frmt / 2; # Size of mullions (half of frame thickness square)
   $mlenx = $w - 2*$offsetx; # length of mullion in x
   $mlenz = $ht - 2*$offsetz; # length of mullion in z
   $mposy = $frmt/2 - $msz/2;
   $panew = ($w - 2*$offsetx) / $npx - ($npx-1)*$msz;
   $paneh = ($ht - 2*$offsetz) / $npz - ($npz-1)*$msz;

   # Make the structural parts (border, mullions)
   genframe($fmat, $name . "_frm", $w, $ht, $frmt, 1, 1);
   # Vertical sticks
   my($xp);
   for($i = 1; $i < $npx; $i++) {
      $xp = $offsetx + $panew*$i + $msz*($i-1);
      system("genbox $fmat ${name}_vs_$i $msz $msz $mlenz | xform -t $xp $mposy $offsetz");
   }
   # Horz sticks
   my($zp);
   for($i = 1; $i < $npz; $i++) {
      $zp = $offsetz + $paneh*$i + $msz*($i-1);
      system("genbox $fmat ${name}_hs_$i $mlenx $msz $msz | xform -t $offsetx $mposy $zp");
   }

   # Add glass
   $, = " ";
   for($i = 0; $i < $npx; $i++) {
      for($j = 0; $j < $npz; $j++) {
         $xp = $offsetx + $panew*$i + $msz*$i;
         $zp = $offsetz + $paneh*$j + $msz*$j;
         $n = ($i+1)*($j+1);
         print "$gmat polygon ${name}_glass_$n\n";
         print "0\n";
         print "0\n";
         print "12    ", $xp, $mposy, $zp, "\n";
         print "      ", $xp, $mposy, $zp+$paneh,"\n";
         print "      ", $xp+$panew, $mposy, $zp+$paneh,"\n";
         print "      ", $xp+$panew, $mposy, $zp, "\n\n";
      }
   }
   $, = "";
}

1;
