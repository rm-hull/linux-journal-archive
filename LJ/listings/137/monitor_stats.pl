#!/usr/bin/perl -w
#
# Title  :   Monitor_stats.pl 
# Author :   John_Ouellette@yahoo.com
# Files  :   Monitor_stats.pl 
# Pupose :   Check servers disk,cpu, swap, uptime
#            and save a history
#
######################################
#
# 1. Type in your Servers here:
#       
#######################################

@unxsrvs= ("myhostapp1","myhostapp2","myhostapp5",
"myhostapp6","myhostdb1","myhostdb3","myhostdb4",
myhostdb5","myhostdb6","myhostdbdev2","myhostweb1", 
"myhostweb2","myhostwebdev1","myhostwebdev2",
"myhostdba","myhost01","myhost02"); 



#Quote for execution: 
chomp($date  = qx#/bin/date    #); 
chomp($month = qx#/bin/date +%B#); 
chomp($year  = qx#/bin/date +%EY#); 
chomp($day   = qx#/bin/date +%d#); 
chomp($hour  = qx#/bin/date +%H#); 
chomp($min  = qx#/bin/date +%M#); 
chomp($webserver  = qx#/bin/hostname#); 

$file="/var/www/html/stats/history/$year/$month/\
 $day/stats.$hour.$min.html";
$current_stats="stats.$hour.$min.html";
$workdir="/var/www/html/stats";


###############################
#mkdir first
system ("mkdir -p  $workdir/history/$year/$month/\
 $day");

#open new file
open   (FH,   ">$workdir/$current_stats") || \
 die "cant open $current_stats:$!";
system ("ln -sf $workdir/$current_stats \ 
 $workdir/stats.html");




########################

sub printhtml  {
print FH "<TD>";
print FH "<A HREF\=\"$current_stats\#$server\"> \
 $server </A>";
print FH "\<\/TD>";
}


#################################################


print FH "$date<P>";
print FH "\<TABLE BORDER\=\"1\" WIDTH\=\"85%\" >";





###########1-5
print FH "<TR>";

foreach $server (@unxerp[0..4])  {
printhtml ();
}

print FH "\<\/TR>";





##########6-10
print FH "<TR>";

foreach $server (@unxerp[5..9])  {
printhtml ();
}

print FH "\<\/TR>";






#########11-15
print FH "<TR>";

foreach $server (@unxerp[10..14])  {
printhtml ();
}

print FH "\<\/TR>";




############16-20
print FH "<TR>";

foreach $server (@unxerp[15..17])  {
printhtml ();
}

print FH "\<\/TR>";


print FH "\<\/TABLE\>";


#################################################

print FH "<A HREF\=\"http://$webserver/stats/\
 history\">History</A>";
print FH "<P>";
print FH "<P>";

##################################################

foreach $server (@unxerp)  {
 
 print FH "<H3><A NAME\=\"$server\">$server\</A\>\
  </H3>";
 print FH "<P>";

 @cmdlist=("/usr/bin/df -k","/sbin/swapon -s",\
  "/bin/top | head -n 20", "/usr/bin/uptime") ;
          foreach $cmd (@cmdlist) {

             @result=`rsh $server $cmd`;
             print FH "<PRE>";
               foreach $line (@result) {
                  print FH "$line";
               }
                  print FH "</PRE>";
                  print FH "<P>";
                  print FH "<BR>";
          }

     print FH "<P>";
     print FH "<BR>";
     print FH "<P>";

}



#move file in to new loc with new name
system ("cp $workdir/stats.html $file") || \
 warn "$file not copied: $!";

