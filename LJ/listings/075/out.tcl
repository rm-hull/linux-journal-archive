frame .gui
frame .sta1
frame .sta2
frame .dynf
frame .dynb

label .labgui  -text "Independent  parameters"
label .labsta1 -text "Statistical  parameters"
label .labsta2 -text "Distribution parameters"
label .labdynf -text "Dynamics     forward"
label .labdynb -text "Dynamics     backward"

text .dummy1   -height 1.0 -width 20
text .dummy3   -height 1.0 -width 20
text .dummy4   -height 1.0 -width 20
text .spr      -height 1.0 -width 20
text .max      -height 1.0 -width 20
text .min      -height 1.0 -width 20
text .mea      -height 1.0 -width 20
text .med      -height 1.0 -width 20
text .mod      -height 1.0 -width 20
text .fs       -height 1.0 -width 20
text .bl       -height 1.0 -width 20
text .tau      -height 1.0 -width 20
text .de       -height 1.0 -width 20
text .steps    -height 1.0 -width 20
text .inf      -height 1.0 -width 20
text .mutinf   -height 1.0 -width 20
text .autocorr -height 1.0 -width 20
text .prerhy   -height 1.0 -width 20
text .fnearnf  -height 1.0 -width 20
text .fnearnb  -height 1.0 -width 20
text .prerlif  -height 1.0 -width 20
text .prerlib  -height 1.0 -width 20
text .prernof  -height 1.0 -width 20
text .prernob  -height 1.0 -width 20
text .maxlyapf -height 1.0 -width 20
text .maxlyapb -height 1.0 -width 20

pack .gui .sta1 .sta2 .dynb .dynf -side left
pack .labgui  .fs      .bl      .tau     .de      .steps   -in .gui
pack .labsta1 .max     .min     .mea     .med      .mod    -in .sta1
pack .labsta2 .spr     .inf     .mutinf  .autocorr .prerhy -in .sta2
pack .labdynf .fnearnf .prerlif .prernof .maxlyapf .dummy3 -in .dynf
pack .labdynb .fnearnb .prerlib .prernob .maxlyapb .dummy4 -in .dynb

proc Indicator { v y r } {
  if { ($v < $r) || ($v > (256-$r)) } { return "#FF0000" }
  if { ($v > $y) && ($v < (256-$y)) } { return "#00FF00" }
  return "#FFFF00"
}

set OK 1     

proc draw { } {
  global OK
  if { ([gets stdin line] >= 0) } {
    set args [scan $line "%s %f" n v ]
    if { ($args >= 2) } {
      switch $n {
        "Spread"   { .spr      delete 1.0 end
                     .spr      insert end "Spread        $v"
                     .spr config -background [eval Indicator { $v 10 30 } ]
                     if { $v <  30 } { set OK 0 }
                   }
        "Maximum"  { .max      delete 1.0 end
                     .max      insert end "Maximum       $v"
                     .max config -background [eval Indicator { $v 3 10 } ]
                     if { $v >= 255 } { set OK 0 }
                   }
        "Minimum"  { .min      delete 1.0 end
                     .min      insert end "Minimum       $v"
                     .min config -background [eval Indicator { $v 3 10 } ]
                     if { $v <   1 } { set OK 0 }
                   }
        "Mean"     { .mea      delete 1.0 end
                     .mea      insert 1.0 "Mean          $v"
                     .mea config -background [eval Indicator { $v 3 10 } ] }
        "Median"   { .med      delete 1.0 end
                     .med      insert end "Median        $v"
                     .med config -background [eval Indicator { $v 3 10 } ] }
        "Modus"    { .mod      delete 1.0 end
                     .mod      insert end "Modus         $v"
                     .mod config -background [eval Indicator { $v 3 10 } ] }
        "fs"       { .fs       delete 1.0 end
                     .fs       insert end "fs            $v" }
        "bl"       { .bl       delete 1.0 end
                     .bl       insert end "block length  [expr $v/1000]" }
        "tau"      { .tau      delete 1.0 end
                     .tau      insert end "tau           $v" }
        "de"       { .de       delete 1.0 end
                     .de       insert end "de            $v" }
        "steps"    { .steps    delete 1.0 end
                     .steps    insert end "steps         $v" }
        "Inf"      { .inf      delete 1.0 end
                     .inf      insert end "Inf           $v"
                     .inf config -background [eval Indicator { $v 0.1 0.01 } ] }
        "MutInf"   { .mutinf   delete 1.0 end
                     .mutinf   insert end "MutInf        $v"
                     if { $v >=  0.00 && $v <= 0.4  } {
                       .mutinf config -background "#00FF00" }
                     if { $v >  0.4 } {
                       .mutinf config -background "#FFFF00" }
                     if { $v <   0.00 || $v >= 0.8  } {
                       .mutinf config -background "#FF0000" }
                     if { $v > 0.5 } { set OK 0 }
                   }
        "AutoCorr" { .autocorr delete 1.0 end
                     .autocorr insert end "AutoCorr      $v"
                     if { $v >= -0.99 && $v <= 0.99 } {
                       .autocorr config -background "#00FF00" }
                     if { $v <  -0.99 || $v >  0.99 } {  
                       .autocorr config -background "#FFFF00" }
                     if { $v <= -1.00 || $v >= 1.00 } {  
                       .autocorr config -background "#FF0000" }
                     if { $v > 0.9999 } { set OK 0 }
                   }
        "PrErHy"   { .prerhy delete 1.0 end
                     .prerhy insert end "PrErHy        $v"
                     if { $v >=  0.0  && $v <= 0.1  } {
                       .prerhy  config -background "#F0F0F0" }
                     if { $v >   0.1  } {
                       .prerhy  config -background "#D0D0D0" }
                     if { $v <   0.00 || $v >  0.3 } {
                       .prerhy  config -background "#A0A0A0" }
                     if { $v > 0.3 } { set OK 0 }
#                       .prerhy  config -background "#FFFFFF"
                   }
        "FNearN_f" { .fnearnf  delete 1.0 end
                     .fnearnf  insert end "FNearN_f      $v"
                     if { $v >=  0.00 && $v <= 0.15 } {
                       .fnearnf config -background "#00FF00" }
                     if { $v >   0.15 && $v <= 0.25 } {
                       .fnearnf config -background "#FFFF00" }
                     if { $v <   0.00 || $v >= 0.25 } {
                       .fnearnf config -background "#FF0000" }
                     if { $v >= 0.20 } { set OK 0 }
                   } 
        "FNearN_b" { .fnearnb  delete 1.0 end
                     .fnearnb  insert end "FNearN_b      $v"
                     if { $v >=  0.00 && $v <= 0.15 } {
                       .fnearnb config -background "#00FF00" }
                     if { $v >   0.15 && $v <= 0.25 } {
                       .fnearnb config -background "#FFFF00" }
                     if { $v <   0.00 || $v >= 0.25 } {
                       .fnearnb config -background "#FF0000" }
                     if { $v >= 0.20 } { set OK 0 }
                   }
        "PrErLi_f" { .prerlif delete 1.0 end
                     .prerlif insert end "PrErLi_f      $v"
                     if { $v >=  0.0  && $v <= 0.1  } {
                       .prerlif config -background "#00FF00" }
                     if { $v >   0.1  } {
                       .prerlif config -background "#FFFF00" }
                     if { $v <   0.00 || $v >  0.3 } {
                       .prerlif config -background "#FF0000" }
                     if { $v > 0.3 } { set OK 0 }
                   }
        "PrErLi_b" { .prerlib delete 1.0 end
                     .prerlib insert end "PrErLi_b      $v"
                     if { $v >=  0.0  && $v <= 0.1 } {
                       .prerlib config -background "#00FF00" }
                     if { $v >   0.1 } {
                       .prerlib config -background "#FFFF00" }
                     if { $v <   0.00 || $v >  0.3 } {
                       .prerlib config -background "#FF0000" }
                     if { $v > 0.3 } { set OK 0 }
                   }
        "PrErNo_f" { .prernof delete 1.0 end
                     .prernof insert end "PrErNo_f      $v"
                     if { $v >=  0.0  && $v <= 0.1  } {
                       .prernof config -background "#00FF00" }
                     if { $v >   0.1  } {
                       .prernof config -background "#FFFF00" }
                     if { $v <   0.00 || $v >  0.3 } {
                       .prernof config -background "#FF0000" }
                     if { $v > 0.3 } { set OK 0 }
                   }
        "PrErNo_b" { .prernob delete 1.0 end
                     .prernob insert end "PrErNo_b      $v"
                     if { $v >=  0.0  && $v <= 0.1 } {
                       .prernob config -background "#00FF00" }
                     if { $v >   0.1 } {
                       .prernob config -background "#FFFF00" }
                     if { $v <   0.00 || $v >  0.3 } {
                       .prernob config -background "#FF0000" }
                     if { $v > 0.3 } { set OK 0 }
                   } 
        "MaxLyap_f" { .maxlyapf delete 1.0 end
                     .maxlyapf insert end "MaxLyap_f     $v"
                     if { $OK == 1 } {
                       .maxlyapf config -background "#00FF00" }
                     if { $OK == 0 } {
                       .maxlyapf config -background "#FF0000" } 
                   }
        "MaxLyap_b" { .maxlyapb  delete 1.0 end
                     .maxlyapb  insert end "MaxLyap_b     $v"
                     if { $OK == 1 } {
                       .maxlyapb config -background "#00FF00" }
                     if { $OK == 0 } { 
                       .maxlyapb config -background "#FF0000" }
                     set OK 1
                   } 
      }
    }
    if {$args == 1} {
      if { $n == "Quit"   } { exit }
    }
  }
  after 1 { draw }
}

after 0 { draw  }
