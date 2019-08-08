"""
Simple Dejunking Proxy configuration.
"""

#
# Time in seconds for a new connection to timeout.
CONNECTION_TIMEOUT = 20

#
# What port to listen on
LISTEN_PORT = 8080

# Do not modify these.
REJECT = 'REJECT'
ACCEPT = 'ACCEPT'

#
# All that really matters here are the COOKIE_POLICY, IMAGE_POLICY,
# and CONNECT_POLICY variables. GOOD, BAD, and UGLY are just for
# usefully arranging information
GOOD = ['*linuxjournal.com', '*zoto.com', 'slashdot.org']
BAD = ['ads.*']
UGLY = ['*microsoft.com']

COOKIE_POLICY = {
    'default': REJECT,
    'accept-list': GOOD,
    'reject-list': BAD
    }

IMAGE_POLICY = {
    'default': ACCEPT,
    'accept-list': GOOD,
    'reject-list': BAD
    }

CONNECT_POLICY = {
    'default': ACCEPT,
    'accept-list': GOOD,
    'reject-list': UGLY
    }
