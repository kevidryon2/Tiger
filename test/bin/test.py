#Tiger, a program for creating servers and customizing them with Network Scripts.
#Copyright (C) 2023 kevidryon2
#
#This program is free software: you can redistribute it and/or modify
#it under the terms of the GNU Affero General Public License as
#published by the Free Software Foundation, either version 3 of the
#License, or (at your option) any later version.
#
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU Affero General Public License for more details.
#
#You should have received a copy of the GNU Affero General Public License
#along with this program.  If not, see <https://www.gnu.org/licenses/>.

import requests
import hashlib
import sys

def hashd(d):
    d = bytes(d.encode("windows-1252"))
    h = hashlib.sha1()
    chunk = 0
    i = 0
    while (chunk != b''):
        chunk = d[i:(i+1024)]
        h.update(chunk)
        i += 1024
    return h.hexdigest()

testhash = "1adc95bebe9eea8c112d40cd04ab7a8d75c4f961"

numtests = 100

successes = 0
fails = 0
for i in range(numtests):
    print("Test", i, end = ": ")
    r = requests.get("http://localhost:"+sys.argv[1]+"/TESTFILE")
    if (hashd(r.text) == testhash):
        print("SUCCESS")
        successes += 1;
    else:
        print("FAIL\nExpected:",testhash,"\nGot:",hashd(r.text))
        fails += 1;

print("Score:", successes-fails)
