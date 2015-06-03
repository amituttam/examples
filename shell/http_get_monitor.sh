#!/bin/bash

URL=${1}

# Store http status header
curl -D /tmp/status ${URL}
ret=$?

# Note: [[ in bash takes arguments literally while [ splits arguments
# and expands '*'.
echo "cURL ret = $ret"
if [[ $ret -ne 0 ]]; then
	echo "Could not connect to ${URL}"
	exit 1
fi

# Get the first line of the header
HTTP_RESP_CODE=`head -n1 /tmp/status | awk '{ print $2 }'`
if [[ ${HTTP_RESP_CODE} != 200 ]]; then
	echo "Sending out email"
	echo "${HTTP_RESP_CODE} != 200 for ${URL}" | mutt -s "HTTP Error" -- amit.uttam@gmail.com
fi

exit 0
