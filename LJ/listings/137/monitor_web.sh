#!/bin/bash
#
# Title      :   Monitor_web.sh 
# Author     :   John_Ouellette@yahoo.com
# Files      :   monitor_web.sh 
# (Optional) :   ok.gif, critical.gif
# Purpose    :   Check web servers,alert if down
#                and save a history
#
# NOTES: 
#       URLs MUST return a valid http "200 OK" 
        string to work. Ttest it w/ wget 
        "Your Url" at a unix prompt.
#                  
# Submit as 
#
#         
#Monitor_web
#0,5,10,15,20,25,30,35,40,45,50,55 * * * * 1-5  \
# /usr/local/admin/bin/monitor_web.sh
#
# in crontab to monitor every 5 minutes 
#


##################################
#
# 1. Set proper recipient(s).
#
##################################

RECIPIENT="mygroup@company"


##################################
#
# 2. Type in your Valid Url here:
#       
##################################
 

URLS="
http://server01.somewhere
http://server02.somewhere 
http://server03.somewhere 
http://server04.somewhere
http://server05.somewhere 
http://server06.somewhere 
http://server07.somewhere 
"
           
##################################
#
# 3. Set Web Server Root
#    and Script Home
# 
##################################

WROOT="/var/www/html"
SHOME="webmon"



################################################
 Do not touch below this line !!!
################################################
#
# System Variables
#
DF="/bin/df"
MOUNT="/bin/mount"
PING="/bin/ping"
GREP="/bin/grep"
AWK="/bin/awk"
HOSTNAME="/bin/hostname"
MAIL="/bin/mail"
NETSTAT="/bin/netstat"
TAIL="/usr/bin/tail"
CUT="/bin/cut"
TR="/usr/bin/tr"
RM="/bin/rm"
DATE="/bin/date"


#Wget Timouts
TIMEOUT=5
RETRIES=2

#Wget Command line
WGET="/usr/local/bin/wget -T $TIMEOUT -t $RETRIES"

WPAGE="$WROOT/$SHOME/webmon.html"
PFILE="$WROOT/$SHOME/ping.out"


HOUR=$($DATE +%H)
MIN=$($DATE +%M)
DAY=$($DATE +%d)
YEAR=$($DATE +%EY)
MNTH=$($DATE +%B)

#
# HTML Variables
#
WEBSRV="$($HOSTNAME |$CUT -d'.'-f1\ 
               |$AWK'{ print $1 }')"
LTAG="</A><BR>"
#Use 2 lines below for text 
RTAG="<IMG SRC="../images/critical.gif">" 
GTAG="<IMG SRC="../images/ok.gif">" 

#Use 2 lines below for images 
#GTAG="<font color="##4CC417">" 
#RTAG="<font color="#FF0000">" 

#End font
EF="</font>"


########################
#Are we on the network?
########################

mkdir -p /tmp

#OK to assume one default gateway
$PING -c 2 $($NETSTAT -nr |\
                    $GREP UG |$AWK '{ print $2 }') 

if (( $? != 0 )); then
    echo "Not on Network - Exiting" > $WPAGE
    $CAT $PFILE  >> $WPAGE
    $RM  $PFILE
    exit 1
fi


#######################
#Create basic HTML Page
#######################
cd $WROOT/$SHOME || exit 1

$DATE > $WPAGE 
cat >> $WPAGE <<EOF
- <A HREF="history">History</A>
<P>
EOF


 
for URL in $URLS 

do
  ENV=$(echo $URL| cut -d "/" -f3- | sed s#/#_#g \
    |sed s#?#_#g | sed s#=#_#g | sed s#:#_#g)
  WLOG="history/$YEAR/$MNTH/$DAY/$ENV/\
                   $ENV.$HOUR.$MIN.LOG" 
  LINK="<A HREF="http://$WEBSRV/$SHOME/$WLOG">"
  UP="$URL is  back up" 
  DOWN="\n $URL is  down. I connected $RETRIES \
         times with a timeout of $TIMEOUT seconds\
         twice w/o a valid response. Please review\
         the log: \n\n \
         http://$WEBSRV/$SHOME/history"

 mail_alert ()
 {
 STATE=$1
 SUBJECT="$URL is $STATE"

 case $STATE in
 up)    echo $UP   >> $WLOG; cat $WLOG | $MAIL -v \
     -s "$SUBJECT" "$RECIPIENT" "$OTHER" >> $WLOG;;
 down)  echo $DOWN >> $WLOG; cat $WLOG | $MAIL -v \
     -s "$SUBJECT" "$RECIPIENT" "$OTHER" >> $WLOG;;
    *)     echo "no match"               >> $WLOG;;
 
 esac
 
 }

 
mkdir -p $WROOT/$SHOME/history/$YEAR/$MNTH/$DAY/$ENV
                 
 $DATE >           $WLOG
 #Don't switch order... 
 $WGET $URL  2>>   $WLOG
 if (( $? != 0 ));then 

    echo >> $WLOG 
    echo "2nd try for $0"  >> $WLOG 
    echo >> $WLOG 
   
  $DATE >>          $WLOG
  #Don't switch order... 
  $WGET $URL  2>>   $WLOG
 
     if (( $? != 0 ));then 
       #echo  "2nd try failed" 
       echo  \<A HREF\="$URL"\>$URL\<\/A\> is down\
      $RTAG $EF.  $LINK Last Result $LTAG  >> $WPAGE
          if [[ ! -a down.$ENV ]];then
             touch /tmp/down.$ENV
             mail_alert down
           else 
             echo Alert already sent for $ENV  -\
                  waiting | $TEE -a $WLOG
          fi
     fi

 else
 
      echo  \<A HREF\="$URL"\>$URL\<\/A\> is up \
             $GTAG $EF.  $LINK Last Result $LTAG\
             >> $WPAGE
        if [[  -a down.$ENV ]];then
          rm -e /tmp/down.$ENV | $TEE -a $WLOG
          echo              >> $WLOG
          mail_alert up 
        fi 
 fi 
        

done

$RM -f  index.html*