#!/usr/bin/perl
#
# ref:
# https://answers.launchpad.net/graphite/+question/192159
# http://stackoverflow.com/questions/9829539/perl-equivlent-to-this-python-code
#
# ====================================================================
# perl's pack function
# "N/a*"
#
# N = An unsigned long (32-bit) in "network" (big-endian) order.
# a = A string with arbitrary binary data, will be null padded.
#
# The / template character allows packing and unpacking of a sequence of items where
# the packed structure contains a packed item count followed by the packed items themselves.
#
# For pack, sequence-item may have a repeat count, in which case the minimum of that and the
# number of available items is used as the argument for length-item. If it has no repeat
# count or uses a '*', the number of available items is used.
#
# - csh
#
use IO::Socket::INET;
use Python::Serialise::Pickle qw();

my($carbon_server) = '127.0.0.1';
my($carbon_port) = 2004;

my($data) = [
             ["path.mytest", [1332444075,27893687]],
             ["path.mytest", [1332444076,938.435]],
            ];

my($message) = pack("N/a*", pickle_dumps($data));

my($sock) = IO::Socket::INET->new (
                PeerAddr => $carbon_server,
                PeerPort => $carbon_port,
                Proto => 'tcp'
            );
            die "Unable to connect: $!\n" unless ($sock->connected);

$sock->send($message);
$sock->shutdown(2);

# Work around P::S::Pickle 0.01's extremely limiting interface.
sub pickle_dumps {
   open(my $fh, '>', \my $s) or die $!;
   my $pickle = bless({ _fh => $fh }, 'Python::Serialise::Pickle');
   $pickle->dump($_[0]);
   $pickle->close();
   return $s;
}
