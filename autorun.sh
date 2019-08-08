#!/bin/sh
##################################
###### INITIAL SETUP VARS ########
##################################

#name of this script
SCRIPT_NAME=autorun.sh
#path to server config and executable
SERVER_PATH=Server/Linux
#name of server executable
SERVER_EXECUTABLE=FlyingAntLinux
#name of server config file
SERVER_CONFIG=flyingant.conf

##################################
##################################






#Get the directory the script was run from (launching from shell doesn't use that as the working dir)
SCRIPT_PATH=`echo $0 | sed 's/'$SCRIPT_NAME'//'`

#Combine the path´s to get the actual relative path to the serve dir from our CWD.
SERVER_PATH=$SCRIPT_PATH$SERVER_PATH

#path to FlyingaAntLinux executable
SERVER_EXECUTABLE=$SERVER_PATH/$SERVER_EXECUTABLE

#path to flyingant.conf
CONFIG_FILE=$SERVER_PATH/flyingant.conf

#Get in the open_url line
OPEN_URL=`grep ^open_url "$CONFIG_FILE"`

#get the url out of that line
OPEN_URL=`echo $OPEN_URL | sed 's/open_url //'`


# Find the absolute path of a browser installed on the system, takes the first one it finds
# which might not be the users preference. May needed to add more options / change the ordering 
# over time
BROWSER=`which firefox 2>&1`

if [ ! -x "$BROWSER" ]; then

	BROWSER=`which netscape 2>&1`
	
	if [ ! -x "$BROWSER" ]; then
	
		BROWSER=`which opera 2>&1`
		
		if [ ! -x "$BROWSER" ]; then
		
			BROWSER=`which konqueror 2>&1`
					
			if [ ! -x "$BROWSER" ]; then
			
				BROWSER=`which mozilla 2>&1`
				
				if [ ! -x "$BROWSER" ]; then
				
				BROWSER=`which safari 2>&1`
					
				fi
			
			fi
		
		fi
	
	fi

fi






#################################
# We now have all the data we need


#launch the server but don't wait for it to close
"$SERVER_EXECUTABLE" &
#wait a second to make sure the server has had time to load
sleep 1

#check if we found a browser
if [ ! -x "$BROWSER" ]; then
	echo No known browser available, please open your browser and go to
	echo $OPEN_URL	
else
	#launch the browser with the start page as an argument, don't wait for it to close
	$BROWSER $OPEN_URL &			
fi

