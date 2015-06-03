#!/usr/bin/python

import sys
from smb.SMBConnection import SMBConnection

host = sys.argv[1]
domain = ""
conn = SMBConnection("guest", "guest", "workgroup", host, domain, True, 2)
conn.connect(host, 139)

shares = conn.listShares()
for share in shares:
	print("Share: %s" % share.name)

conn.close()
