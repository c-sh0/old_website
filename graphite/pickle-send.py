#!/usr/bin/python
#
# - csh
#
import socket
import struct
import cPickle

sock = socket.socket()
sock.connect( ('localhost', 2004) ) # pickle protocol is done on port 2004

#data = gather_me_some_data()
data = (
         ("testing.mytest_metric", (1332444073,666.777)),
         ("testing.mytest_metric", (1332444074,888.999)),
       )

# where data = [ (metic, datapoints), ... ]
# and datapoints = [ (timestamp, value), ... ]

serialized_data = cPickle.dumps(data, protocol=-1)
length_prefix = struct.pack("!L", len(serialized_data))
message = length_prefix + serialized_data

sock.sendall(message)

