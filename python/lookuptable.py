#!/usr/bin/env python

lookup = {}
with open('zipcodes.txt', 'r') as f:
    for _ in f.readlines():
        items = _.split(',')
        zip = items[1]
        lat = items[5]
        lon = items[4]
        lookup[zip[1:-1]] = "%s, -%s" % (lat, lon)

print lookup['91326']
