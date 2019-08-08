#!/usr/bin/env python
"""
Simple Dejunking Proxy by Ken Kinder <ken@kenkinder.com>
Part of "Asynchronous Programming in Twisted"
By Ken Kinder <ken@kenkinder.com>
--------------------------------------------------------------------
Simple Dejunking Proxy by Ken Kinder <ken@kenkinder.com>

Copyright (C) 2004  Ken Kinder

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
--------------------------------------------------------------------
"""
__version__ = '1.0'
__author__ = 'Ken Kinder <ken@keninder.com>'

PROGRAM_NAME = "Simple Dejunking Proxy"
PROGRAM_VERSION = __version__
ERROR_MSG_TEMPLATE = """
<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML 2.0//EN">
<html><head><title>%(title)s</title></head><body>
<h1>%(title)s</h1>
<p>%(description)s</p>
<hr/>
%(program_name)s %(program_version)s / %(date)s
</body></html>
"""

#
# Configuration
from config import CONNECTION_TIMEOUT, LISTEN_PORT, COOKIE_POLICY, \
     IMAGE_POLICY, CONNECT_POLICY, ACCEPT, REJECT

# Python Modules
import urlparse, glob, time, gc
gc.enable()

# Twisted Modules
from twisted.internet import interfaces, reactor, protocol, address
from twisted.internet.defer import Deferred
from twisted.python.failure import Failure
from twisted.protocols import basic

def htmlEscape(value):
    """
    Does some basic escaping of HTML. Returns string with escaped
    <, >, and & chars.
    """
    return value.replace('&', '&amp;').replace('<', '&lt;').replace('>', '&gt;')

def genErrorMessage(title, description):
    """
    Generates suitable HTML error message.
    """
    return ERROR_MSG_TEMPLATE % {
        'title': htmlEscape(title),
        'description': htmlEscape(description),
        'date': time.strftime('%Y-%m-%d %H:%M:%S'),
        'program_name': PROGRAM_NAME,
        'program_version': PROGRAM_VERSION
        }

def getPolicy(policy, host):
    """
    Gets a accept/reject policy for a host. Returns True if it's okay,
    False if not. Policy should be one of ('cookie', 'image', 'connect')
    
    See the constants above.
    """
    if policy == 'cookie':
        rules = COOKIE_POLICY
    elif policy == 'image':
        rules = IMAGE_POLICY
    elif policy == 'connect':
        rules = CONNECT_POLICY
    else:
        raise ValueError, 'Invalid value for first argument: %s' % policy
    
    for wildcard in rules['reject-list']:
        if glob.fnmatch.fnmatch(host, wildcard):
            return False
    for wildcard in rules['accept-list']:
        if glob.fnmatch.fnmatch(host, wildcard):
            return True
    if rules['default'] == ACCEPT:
        return True
    else:
        return False

# ABOUT TWISTED'S HTTP CLASSES
# ----------------------------
#
# These classes are somewhat similar to those available in
# twisted.protocols.http. The reason I'm not using those classes
# is that they make a number of assumptions about how you want
# to handle HTTP traffic. For instance, they expect you to only
# begin processing HTTP requesta after they are fully finished.
#
# These methods are simpler in that they do not attend to every
# aspect of HTTP -- only enough to manage proxying requests and
# doing a few slight manipulations of the data.
#

class ProxyError(Exception):
    pass

class ProxyConnectError(ProxyError):
    pass

class SimpleHTTP(basic.LineReceiver):
    """
    Handling HTTP is similar for both incoming and outgoing, so this class
    provides the protocol framework for both.
    """
    def __init__(self):
        pass
    
    def startNewRequest(self):
        """
        This method is called every time there is a new HTTP request being
        handled by the object.

        Because of keep-alive, one of these objects can handle many HTTP
        requests.
        """
        self.request_started_def = Deferred()
        self.firstline_sent_def = Deferred()
        self.headers_finished_def = Deferred()
        self.request_finished_def = Deferred()
        
        # Indicates whether the first line (GET, 200, etc) is in
        self.firstline_received = False
        
        # Flag whether headers are done
        self.headers_received = False
        
        #
        # These vars are for HTTP "chunk mode" -- the first one flags whether
        # we are in chunk mode, the second indicates what part of chunk
        # mode.
        self.chunk_mode = False
        self.chunk_mode_part = 'prefix'   # (One of 'prefix', 'trailer')
        
        # Keep mostly intact list of headers and dictionary for easy lookup:
        self.header_lines = []
        self.headers = {}
        
        # Will be one of ('HTTP/1.1', 'HTTP/1.0')
        self.http_version = None
        
        self.request_started_def.callback(None)

    def connectionMade(self):
        self.startNewRequest()
        
    def lineReceived(self, line):
        """
        About lineReceived, rawDataReceived
        
        Because this class is a subclass of basic.LineReceiver, we implement
        lineReceived and rawDataReceived. LineReceiver is for handling
        protocols that involve reading line-delimited commands, and switching
        into binary/raw data mode.
        
        When we are processing HTTP headers or HTTP chunked data headers, we
        are in line mode, and this method is invoked on incoming data. Upon
        switching to "raw mode", the rawDataReceived will be invoked.
        """
        #print self.prefix, line
        if not self.firstline_received:
            self.fristline_value = line
            self.firstline_received = True
            self.handleFirstLine(line)
            
        elif self.chunk_mode:
            #
            # Chunk mode is a bit of HTTP nastiness we're forced
            # to deal with. For a good summary of it, see
            # http://www.jmarshall.com/easy/http/
            #
            self.handleData(line + '\r\n')
            
            if self.chunk_mode_part == 'trailer':
                if not line:
                    self.handleRequestFinished()                
                        
            elif self.chunk_mode_part == 'prefix':
                if line:
                    self.remaining_content = int(line, 16)
                    if self.remaining_content:
                        self.setRawMode()
                    else:
                        self.chunk_mode_part = 'trailer'
            else:
                raise ValueError, 'Illegal value for self.chunk_mode_part: %s' % self.chunk_mode_part
        
        elif not self.headers_received: # This is a header line
            if line.strip() == '':
                self.headers_received = True
                #
                # Headers are done -- build quick access dictionary
                # and set some useful 
                for header in self.header_lines:
                    if header.strip():
                        try:
                            k, v = header.split(':', 1)
                        except ValueError:
                            raise ValueError, 'Invalid header: %s' % header[:100]
                    self.headers[k.lower().strip()] = v.strip()
                
                # Subclass hook for this moment in time
                self.handleHeadersFinished()
                content_length = int(self.headers.get('content-length', 0))
                transfer_encoding = self.headers.get('transfer-encoding', '')
                if transfer_encoding.lower() == 'chunked':
                    self.chunk_mode = True
                    self.chunk_mode_part = 'prefix'
                elif content_length:
                    self.remaining_content = content_length
                    self.setRawMode()
                else:
                    # Neither chunk mode, nor limited content length
                    # specified -- request is done.
                    self.handleRequestFinished()
    
            elif line[0] in ' \t': # Continued header
                self.header_lines[-1] += '\n%s' % line
                
            else:
                self.header_lines.append(line)
                
        else:
            self.transport.loseConnection()
            raise AssertionError, "Don't know what to do with this line: %s" % line
    
    def rawDataReceived(self, data):
        """
        See lineReceived docstring
        """
        if len(data) < self.remaining_content:
            self.handleData(data)
            self.remaining_content -= len(data)
        else:
            self.handleData(data[:self.remaining_content])
            extraneous = data[self.remaining_content:]
            if self.chunk_mode:
                self.chunk_mode_part = 'prefix'
                self.setLineMode(extraneous)
            else:
                self.handleRequestFinished()
                self.setLineMode(extraneous)
    
    def handleData(self, data):
        pass
    
    def handleFirstLine(self, line):
        """
        This method is ran when first line of HTTP chat has been received. For
        outgoing hits, this is usually a GET or POST string. For incoming
        ones, it's an HTTP status code and message.
        
        This method can be subclassed, but should call the superclass's
        method, as it fires an event.
        """
        self.firstline_sent_def.callback(line)
    
    def handleHeadersFinished(self):
        """
        This method is ran when the HTTP headers have been fully sent. They
        are stored in self.headers in dictionary form, or self.header_lines
        as a list of original headers.
        
        This method can be subclassed, but should call the superclass's
        method, as it fires an event.
        """
        self.headers_finished_def.callback(self.header_lines)
    
    def handleRequestFinished(self):
        """
        This method is invoked when the HTTP request has been finished.
        
        This method can be subclassed, but should call the superclass's
        method, as it fires an event.
        """
        self.request_finished_def.callback(None)

class IncomingProxy(SimpleHTTP):
    """
    This is the incoming protocol. An instance of this class will be created
    for each socket opened to the server.
    """
    def __init__(self):
        SimpleHTTP.__init__(self)
        
        #
        # The outgoing proxy cache is in charge of getting us our new outbound
        # http proxies. If approopriate, it will re-use them.        
        self.outgoing_proxy_cache = OutgoingProxyCache()
    
    def connectionMade(self):
        SimpleHTTP.connectionMade(self)
        
        # These specify the outgoing http client. outgoing_buffer queues up data
        # while we wait for the outbound proxy.
        self.outgoing_proxy = None
        self.outgoing_buffer = ''
        
    def handleFirstLine(self, line):
        SimpleHTTP.handleFirstLine(self, line)
        
        #
        # We haven't received an HTTP command (this is usually either GET or POST)
        try:
            self.http_command, self.url, self.http_version = line.split()
        except ValueError:
            self.transport.write("HTTP/1.1 400 Bad Request\r\n\r\n")
            self.transport.loseConnection()
            return
        
        #
        # Decode HTTP command
        (scheme, netloc, path, params, query, fragment) = urlparse.urlparse(self.url)
        if ':' in netloc:
            host, port = netloc.split(':', 1)
        else:
            host = netloc
            port = 80
        self.host = host
        
        #
        # Check policy
        if getPolicy('connect', host):
            uri = urlparse.urlunparse(('', '', path, params, query, ''))
            
            #
            # Get a new outgoing proxy from the proxy cache. It might be the same
            # one, but either way we should ask.
            self.outgoing_proxy = None
            d = self.outgoing_proxy_cache.getOutgoing(host, int(port))
            
            #
            # Set callbacks for when the connection is made or an error has occured.
            d.addCallback(self.outgoingConnectionMade, uri)
            d.addErrback(self.outgoingProxyError, uri)
        else:
            self.rejectConnection('Remote server prohibited')
    
    def rejectConnection(self, reason):
        # This site is blocked.
        self.sendLine(self.http_version + ' 403 %s' % reason)
        self.sendLine('Content-type: text/html')
        self.sendLine('')
        self.transport.write(genErrorMessage('Forbidden', 'The proxy server rules do not allow access to this host under these circumstances.'))
        self.transport.loseConnection()
        if self.outgoing_proxy:
            self.outgoing_proxy.closeProxy()
    
    def outgoingProxyError(self, failure, uri):
        """
        This method is invoked as a Twisted "errback" method that is called
        when an error occurs inside a Deferred. This is like an Except: block,
        but works asynchronously.
        """
        #
        # Send HTTP code for a proxy failure.
        self.sendLine(self.http_version + ' 502 %s' % failure.value)
        
        #
        # Send error message
        self.sendLine('Content-type: text/html')
        self.sendLine('')
        self.transport.write(genErrorMessage('Connection error: %s' % failure.value, 'Error accepting request for %s: %s' % (uri, failure.value)))
        
        #
        # Close this connection and any outbound one that may have been created.
        self.transport.loseConnection()
        if self.outgoing_proxy:
            self.outgoing_proxy.closeProxy()
    
    def outgoingConnectionMade(self, outgoing_proxy, uri):
        """
        This occurs when our outbound proxy has connected. It's a Twisted
        callback method.
        """
        assert(outgoing_proxy, OutgoingProxy)
        self.outgoing_proxy = outgoing_proxy
        outgoing_proxy.incoming_proxy = self
        
        # Send HTTP command and echo back result
        outgoing_proxy.write('%s %s %s' % (self.http_command, uri, self.http_version) + self.delimiter)
        outgoing_proxy.firstline_sent_def.addCallback(self.outgoingFirstlineReceived)
        
        # Send anything we have queued.
        self.flushOutgoingBuffer()
        
        # Add callbacks for when headers are ready
        outgoing_proxy.headers_finished_def.addCallback(self.outgoingHeadersReceived)
        outgoing_proxy.request_finished_def.addCallback(self.handleOutgoingRequestFinished)
    
    def outgoingFirstlineReceived(self, line):
        """
        This method is invoked when the outgoing proxy has gotten an HTTP 
        response line back from the remote server.
        
        We just echo it to our client.
        """
        self.sendLine(line)
    
    def outgoingHeadersReceived(self, header_lines):
        """
        This method is invoked as a Twisted callback when the proxy
        has gotten headers back from the remote server.
        """
        #
        # If this host isn't allowed to serve images, this is where it's blocked.
        images_ok = getPolicy('image', self.host)
        cookies_ok = getPolicy('cookie', self.host)
        
        for header in header_lines:
            if (not images_ok) and header.lower().startswith('content-type:') and 'image' in header:
                # This is an image and images aren't ok.
                self.sendLine('')
                self.transport.loseConnection()
                self.outgoing_proxy.closeProxy()
                return
            elif (not cookies_ok) and header.lower().startswith('set-cookie:'):
                pass
            else:
                self.sendLine(header)
        self.sendLine('Proxy-agent: %s %s' % (PROGRAM_NAME, PROGRAM_VERSION))
        self.sendLine('')
    
    def handleHeadersFinished(self):
        """
        This method occurs when the web browser has finished sending us the
        HTTP headers. We'll just echo them to the server.
        """
        SimpleHTTP.handleHeadersFinished(self)
        
        for header in self.header_lines:
            #
            # Replace proxy-connection header.
            if header.lower().startswith('proxy-connection:'):
                self.sendOutgoingLine('Connection: %s' % header.split(':', 1)[1].strip())
            else:
                self.sendOutgoingLine(header)
        self.sendOutgoingLine('Proxy-agent: %s %s' % (PROGRAM_NAME, PROGRAM_VERSION))
        self.sendOutgoingLine('')
    
    def handleData(self, data):
        """
        This method is called when the web browser sends us data that isn't
        part of the HTTP headers. We just send it to the remote server.
        """
        SimpleHTTP.handleData(self, data)
        self.sendOutgoing(data)
        
    def sendOutgoingLine(self, line):
        """
        This method sends a single line to the remote server via the proxy.
        See sendOutgoing.
        """
        self.sendOutgoing('%s%s' % (line, self.delimiter))
        
    def sendOutgoing(self, data):
        """
        sendOutgoing and flushOutgoingBuffer just queue up data to send to
        the remote web server via the proxy.
        """
        self.outgoing_buffer += data
        self.flushOutgoingBuffer()
    
    def flushOutgoingBuffer(self):
        if self.outgoing_proxy:
            self.outgoing_proxy.write(self.outgoing_buffer)
            self.outgoing_buffer = ''
            
    def write(self, data):
        """
        Just pass data coming from the remote web server through.
        """
        self.transport.write(data)

    def handleOutgoingRequestFinished(self, void):
        """
        This method is called when the outgoing proxy has finished
        its request. We decide whether to stay alive and do another
        request.
        """
        self.flushOutgoingBuffer()
        self.keeping_alive = self.keepAlive()
        if self.keeping_alive:
            self.startNewRequest()
        else:
            self.transport.loseConnection()
    
    def keepAlive(self):
        """
        Inspects HTTP headers to determine whether another HTTP request
        is coming.
        """
        if self.http_version == 'HTTP/1.1':
            if self.headers.get('proxy-connection', '') == 'close':
                return False
            else:
                return True
        else:
            if self.headers.get('proxy-connection', '') == 'keep-alive':
                return True
            else:
                return False
            
class OutgoingProxy(SimpleHTTP):
    """
    Outgoing Proxy class -- sends hits out to the remote web server.
    """
    def __init__(self, connected_defer):
        """
        The connected_defer is an event we'll fire when the outgoing
        connection has been made.
        """
        SimpleHTTP.__init__(self)
        self.connected_defer = connected_defer
        
    def connectionMade(self):
        SimpleHTTP.connectionMade(self)
        self.connected_defer.callback(self)
    
    def write(self, data):
        self.transport.write(data)
    
    def handleData(self, data):
        SimpleHTTP.handleData(self, data)
        
        self.incoming_proxy.write(data)
        
    def handleRequestFinished(self):
        SimpleHTTP.handleRequestFinished(self)
        
        #
        # See if we'll be needed again.
        if self.incoming_proxy.keeping_alive:
            self.startNewRequest()
        else:
            self.transport.loseConnection()
    
    def closeProxy(self):
        self.transport.loseConnection()
        
class IncomingFactory(protocol.ServerFactory):
    """
    This is what Twisted uses to spawn the incoming proxy server. To accept
    TCP connections, you create an instance of a Factory class like this one,
    and send it to the reactor.listenTCP method.
    """
    protocol = IncomingProxy

class OutgoingFactory(protocol.ClientFactory):
    """
    This the outgoing proxy dispatcher. To create an outbound connection,
    you create an instance of a Factory like this, and send it to
    reactor.connectTCP
    """
    protocol = OutgoingProxy
    def __init__(self):
        #
        # This deferred object will be invoked when the connection is made
        # or an error occured.
        self.defer = Deferred()
    
    def buildProtocol(self, addr):
        """
        This method generally means we've made a new connection and we need a
        new Protocol class to manage the session.
        """
        p = self.protocol(self.defer)
        p.factory = self
        return p

    def clientConnectionFailed(self, connector, reason):
        self.defer.errback(Failure(reason, ProxyLostConnectionError))

class OutgoingProxyCache:
    """
    Manages outbound proxies. Re-uses them when apropriate.
    """
    def __init__(self):
        self.last_host = None
        self.last_port = None
        self.last_outgoing = None
        
    def getOutgoing(self, host, port):
        # If the port and host are the same, re-use the last proxy
        if (host, port) == (self.last_host, self.last_port):
            d = Deferred()
            d.callback(self.last_outgoing)
        else:
            outgoing_factory = OutgoingFactory()
            reactor.connectTCP(host, port, outgoing_factory, timeout=CONNECTION_TIMEOUT)
            d = outgoing_factory.defer
            d.addCallback(self.setLastOutgoing)
        
        self.last_host = host
        self.last_port = port
        return d
    
    def setLastOutgoing(self, outgoing_proxy):
        self.last_outgoing = outgoing_proxy
        return outgoing_proxy

if __name__ == '__main__':
    reactor.listenTCP(LISTEN_PORT, IncomingFactory())
    reactor.run()
