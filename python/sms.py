#!/usr/bin/python

import urllib, urllib2

def send_text(numstring, message):
    data = urllib.urlencode({'number': numstring, 'message': message})
    r = urllib2.urlopen("http://textbelt.com/text", data)
    return r.getcode(), r.read()

send_text("8184705599", "test from python")
