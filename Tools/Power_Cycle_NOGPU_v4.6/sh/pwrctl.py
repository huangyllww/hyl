#!/usr/bin/env python
# -*- coding: utf-8 -*-
import argparse
import datetime
import re
import socket
import sys
import time
import urllib
import urllib2


TIMEOUT = 10
DELAY = '90'
SOCKET_COUNT = 2
MAX_SOCKET = 8
MAX_BIT = 24
AUTH = "c25tcDoxMjM0"
#AUTH = "YWRtaW46c2lnODhzaWc="


class Error(Exception):
    def __init__(self, msg):
        self.msg = msg

    def __str__(self):
        return self.msg

    def __repr__(self):
            return self.msg


def http_request(url):
    try:
        request = urllib2.Request(url, headers={'Authorization': 'Basic '+AUTH})
        response = urllib2.urlopen(request, timeout=TIMEOUT)
        output = response.read().strip()
        if output != 'Success!':
            raise Error('remote control failed <%s>' % output)
    except socket.timeout:
        raise Error('can not connect to URL <%s>' % url)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("ip", 
                        help="IP address of remote power switch, default 192.168.0.200")
    parser.add_argument("socket", 
                        nargs='?',
                        default='11',
                        help="which socket to be power OFF/ON, format is 'xx', x is 1 or 0, 1 means apply, 0 means ignore. Default '11', powering OFF/ON two sockets")
    parser.add_argument("delay",
                        nargs='?',
                        default=','.join([DELAY]*SOCKET_COUNT),
                        help="power ON delay time(seconds) for two sockets, format is 'x,x'. Default '%s', two sockets will be power OFF/ON with %s seconds delay" % (','.join([str(DELAY)]*SOCKET_COUNT), DELAY))
    parser.add_argument("-v", "--verbose", action="store_true", help="increase output verbosity")
    args = parser.parse_args()

    # socket argument must be like pattern 'xx', x is 1 or 0
    socket_regex = re.compile('[01]{2}')
    if not socket_regex.match(args.socket):
        parser.print_help()
        raise Error("socket argument is not correct, accepted pattern is like 'xx', x is 1 or 0")

    # delay argument must be like pattern 'x,x', x is a number
    delay_regex = re.compile("\d+,\d+")
    if not delay_regex.match(args.delay):
        parser.print_help()
        raise Error("delay argument is not correct, accepted pattern is like 'x,x', x is a number")

    # Firstly, set power OFF delay time of each socket to 0 seconds
    # max eight sockets, e.g. http://192.168.1.87/delayf1.cgi?led=0,0,0,0,0,0,0,0,0,
    offdelay_url = "http://%s/delayf1.cgi?led=0,60,60,4,5,6,7,8,9," % args.ip
    if args.verbose:
        print offdelay_url
    http_request(offdelay_url)

    # use delay argument to set power ON delay time for each socket
    # e.g. http://192.168.1.87/delay1.cgi?led=0,90,90,90,90,90,90,90,90,
    ondelay_url = "http://%s/delay1.cgi?led=0," % args.ip
    # two sockets: http://192.168.1.87/delay1.cgi?led=0,90,90,0,0,0,0,0,0,
    ondelay_url += args.delay + ',' + ','.join(['0']*(MAX_SOCKET-SOCKET_COUNT)) + ','
    if args.verbose:
        print ondelay_url
    http_request(ondelay_url)

    # control which sockets to turn off/on, max 24 bits  
    # e.g. turn off/on two sockets: http://192.168.1.87/offon.cgi?led=110000000000000000000000
    offon_url = "http://%s/offon.cgi?led=%s" % (args.ip, args.socket+'0'*(MAX_BIT-SOCKET_COUNT))

    now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    print "%s: remote power switch <%s> will power OFF <%s> sockets and power ON after <%s> seconds" % (now, args.ip, args.socket.count('1'), args.delay)
    if args.verbose:
        print offon_url
    http_request(offon_url)


if __name__ == "__main__":
    try:
        if sys.version[:3] != '2.7':
            raise Error('Python2.7 is required')
        main()
        print 'Success'
    except Error as e:
        print "Error: %s" % e
        sys.exit(1)
