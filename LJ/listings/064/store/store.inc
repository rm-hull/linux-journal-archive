# ###################################################
# Program:  store.inc
# Author :  Jay Sissom
# Date   :  11/25/98
# Purpose:  Important constants for the book store 
#           program
# ###################################################

# Paths and such
$ENV{'SERVER_ROOT'} = '/home/httpd';
$template_dir = $ENV{'SERVER_ROOT'} . '/cgi-bin/store/templates';
$script_dir = $ENV{'SERVER_ROOT'} . '/cgi-bin/store';

$sybase_user      = 'store_user';
$sybase_password  = 'a&emtvvh1tnt';
$sybase_server    = 'linux_dev';
$sybase_database  = 'book_d';

# Sybase things
$ENV{'SYBASE'} = '/opt/sybase';
$ENV{'DSQUERY'} = $sybase_server;

# ##################################################
# Constants
# ##################################################
$SCRIPT_NAME = $ENV{'SCRIPT_NAME'};

1;


