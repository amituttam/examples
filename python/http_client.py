#!/usr/bin/python3

import sys
import http.client
from pprint import pprint
import base64
import re
import hashlib
import random

path = sys.argv[1]

def get(headers=None):
    # Simple GET
    if not headers:
        headers = { 'User-Agent': 'http_client/0.1',
                    'Accept': '*/*',
                    'Accept-Encoding': 'gzip, deflate' }
    http_conn = http.client.HTTPConnection("localhost")
    http_conn.set_debuglevel(1)
    http_conn.request("GET", path, headers=headers)

    ## Response
    resp = http_conn.getresponse()
    print()
    print("Status:", resp.status, resp.reason)

    ## Cleanup
    http_conn.close()
    return resp

def get_auth_basic():
    # GET with authorization
    authstring = base64.b64encode(("%s:%s" % ("amit","amit")).encode())
    authheader = "Basic %s" % (authstring.decode())
    print("Authorization: %s" % authheader)

    headers = { 'User-Agent': 'http_client/0.1',
                'Accept': '*/*',
                'Authorization': authheader,
                'Accept-Encoding': 'gzip, deflate' }
    http_conn = http.client.HTTPConnection("localhost")
    http_conn.set_debuglevel(1)
    http_conn.request("GET", path, headers=headers)

    ## Response
    resp = http_conn.getresponse()
    print()
    print("Status:", resp.status, resp.reason)

    ## Cleanup
    http_conn.close()

def get_auth_digest():
    resp = get()

    # Get dictionary of headers
    headers = resp.getheader('WWW-Authenticate')
    h_list = [h.strip(' ') for h in headers.split(',')]
    #h_tuple = re.findall("(?P<name>.*?)=(?P<value>.*?)(?:,\s)", headers) 
    h_tuple = [tuple(h.split('=')) for h in h_list]
    f = lambda x: x.strip('"')
    h = {k:f(v) for k,v in h_tuple}
    print(h)

    # HA1 = md5(username:realm:password)
    ha1_str = "%s:%s:%s" % ("amit",h['realm'],"amit")
    ha1 = hashlib.md5(ha1_str.encode()).hexdigest()
    print("ha1:",ha1)

    # HA2 = md5(GET:uri) i.e. md5(GET:/files/)
    ha2_str = "%s:%s" % ('GET',path)
    ha2 = hashlib.md5(ha2_str.encode()).hexdigest()
    print("ha2:",ha2)

    # Generate cnonce
    cnonce = hashlib.sha1(str(random.random()).encode()).hexdigest()[:8]
    print("cnonce:",cnonce)

    # Generate response = md5(HA1:nonce:00000001:cnonce:qop:HA2)
    resp_str = "%s:%s:%s:%s:%s:%s" % (ha1,h['nonce'],"00000001",cnonce,h['qop'],ha2)
    resp_hash = hashlib.md5(resp_str.encode()).hexdigest()
    print("resp_hash:",resp_hash)

    # Do another get
    authheader = 'Digest username="%s", realm="%s", nonce="%s", ' \
                 'uri="%s", response="%s", algorithm="%s", qop="%s", nc=00000001, ' \
                 'cnonce="%s"' \
                 % ("amit", h['realm'], h['nonce'], path, resp_hash, h['Digest algorithm'], h['qop'], cnonce)
    print(authheader)
    headers = { 'User-Agent': 'http_client/0.1',
                'Accept': '*/*',
                'Accept-Encoding': 'gzip, deflate',
                'Authorization': authheader
              }
    get(headers)

def main():
    get_auth_digest()

if __name__ == '__main__':
    main()
