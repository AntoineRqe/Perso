# -*- coding: utf-8 -*-
#
# (c) Copyright 2003-2015 HP Development Company, L.P.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# Author: Shunmugaraj.K
#

# StdLib
import time
import io
import binascii
import xml.parsers.expat
from string import *

try:
    import xml.parsers.expat as expat
    xml_expat_avail = True
except ImportError:
    xml_expat_avail = False

class XMLToDictParser:
    def __init__(self):
        self.stack = []
        self.data = {}
        self.last_start = ''

    def startElement(self, name, attrs):
        #print "START:", name, attrs
        self.stack.append(str(name).lower())
        self.last_start = str(name).lower()

        if len(attrs):
            for a in attrs:
                self.stack.append(str(a).lower())
                self.addData(attrs[a])
                self.stack.pop()

    def endElement(self, name):
        if name.lower() == self.last_start:
            self.addData('')

        #print "END:", name
        self.stack.pop()

    def charData(self, data):
        data = str(data).strip()

        if data and self.stack:
            self.addData(data)

    def addData(self, data):
        #print("DATA:%s" % data)
        self.last_start = ''
        try:
            data = int(data)
        except ValueError:
            data = str(data)

        stack_str = '-'.join(self.stack)
        stack_str_0 = '-'.join([stack_str, '0'])

        try:
            self.data[stack_str]
        except KeyError:
            try:
                self.data[stack_str_0]
            except KeyError:
                self.data[stack_str] = data
            else:
                j = 2
                while True:
                    try:
                        self.data['-'.join([stack_str, str(j)])]
                    except KeyError:
                        self.data['-'.join([stack_str, str(j)])] = data
                        break
                    j += 1

        else:
            self.data[stack_str_0] = self.data[stack_str]
            self.data['-'.join([stack_str, '1'])] = data
            del self.data[stack_str]


    def parseXML(self, text):
        if xml_expat_avail:
            parser = expat.ParserCreate()

            parser.StartElementHandler = self.startElement
            parser.EndElementHandler = self.endElement
            parser.CharacterDataHandler = self.charData

            parser.Parse(text, True)

        else:
            log.error("Failed to import expat module , check python-xml/python3-xml package installation.")

        return self.data

def extract_xml_chunk(data):
    while 1:
        index = data.find('\r\n\r\n')
        if index > 0:
            data = data[index+4:]
        else:
            break

    print data
    print '--------------------------------------------------------------------'

    if data[0:1] != '<':            # Check for source encoding chunked or content length in http respose header.
        size = -1
        temp = ""
        while data[0:1] != '<':
            print data
            index = data.find('\r\n')
            print 'index : ' + str(index)
            print 'data[0:index+1] : ' + data[0:index+1]
            size = int(data[0:index+1], 16)
            print 'size : ' + str(size)
            temp = temp + data[index+2:index+2+size]
            print 'temp : ' + temp
            data = data[index+2:index+2+size]
            print '--------------------------------------------------------------------'
        data = temp

        pos = data.find('<io:WifiNetworks',0,len(data))
        repstr = data[pos:].strip()
        print 'repstr: ' + repstr
        repstr = repstr.replace('\r',' ').replace('\t',' ').replace('\n',' ') # To remove formating characters from the received xml
        repstr = repstr.rstrip('0')   # To remove trailing zero from the received xml
        print 'repstr: ' + repstr
        elementCount = repstr.count('<io:WifiNetwork>')
        print 'count: ' + str(elementCount)

        params = XMLToDictParser().parseXML(repstr)

        print 'params:'
        print params.items()
    return data

test_string = u'HTTP/1.1 200 OK\r\nServer: nginx\r\nDate: Sun, 24 Nov 2019 18:23:40 GMT\r\nContent-Length: 0\r\nConnection: keep-alive\r\nCache-Control: must-revalidate, max-age=0\r\nPragma: no-cache\r\n\r\nHTTP/1.1 200 OK\r\nServer: nginx\r\nDate: Sun, 24 Nov 2019 18:23:46 GMT\r\nContent-Type: text/xml\r\nTransfer-Encoding: chunked\r\nConnection: keep-alive\r\nCache-Control: must-revalidate, max-age=0\r\nPragma: no-cache\r\n\r\n6e7\r\n<?xml version="1.0" encoding="UTF-8"?>\r\n<!-- THIS DATA SUBJECT TO DISCLAIMER(S) INCLUDED WITH THE PRODUCT OF ORIGIN. -->\r\n<io:WifiNetworks xmlns:io="http://www.hp.com/schemas/imaging/con/ledm/iomgmt/2008/11/30" xmlns:dd="http://www.hp.com/schemas/imaging/con/dictionaries/1.0/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:wifi="http://www.hp.com/schemas/imaging/con/wifi/2009/06/26" xsi:schemaLocation="http://www.hp.com/schemas/imaging/con/ledm/iomgmt/2008/11/30 ../../schemas/IoMgmt.xsd     http://www.hp.com/schemas/imaging/con/dictionaries/1.0/ ../../schemas/dd/DataDictionaryMasterLEDM.xsd">\r\n\t<io:WifiNetwork>\r\n\t\t<wifi:SSID>42626f782d31454434353231442d3547487a</wifi:SSID>\r\n\t\t<wifi:BSSID>b8d94dd36b94</wifi:BSSID>\r\n\t\t<wifi:CommunicationMode>infrastructure</wifi:CommunicationMode>\r\n\t</io:WifiNetwork>\r\n</io:WifiNetworks>'

def main():
    test_2= extract_xml_chunk(test_string)

if __name__ == "__main__":
    main()
