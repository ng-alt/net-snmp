# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl test.pl'

#########################

# change 'tests => 1' to 'tests => last_test_to_print';

use Test;
BEGIN { plan tests => 23 };
use NetSNMP::OID;
ok(1); # If we made it this far, we're ok.

#########################

# Insert your test code below, the Test module is use()ed here so read
# its man page ( perldoc Test ) for help writing this test script.

my $oid = new NetSNMP::OID(".1.3.6.1");
ok(ref($oid) eq "NetSNMP::OID");
ok(ref($oid->{oidptr}) eq "netsnmp_oidPtr");
#print STDERR ref($oid),"\n";

my $tostring = "$oid";
#print STDERR "$tostring\n";
ok($tostring eq "internet");

my $oid2 = new NetSNMP::OID(".1.3.6.1.2");
$tostring = "$oid2";
#print STDERR "$tostring\n";
ok($tostring eq "mgmt");

my $oid3 = new NetSNMP::OID(".1.3.6.1");

my $val = NetSNMP::OID::snmp_oid_compare($oid, $oid2);
#print STDERR "compare result: $val\n";
ok($val == -1);

$val = $oid2->snmp_oid_compare($oid);
#print STDERR "compare result: $val\n";
ok($val == 1);

$val = NetSNMP::OID::compare($oid, $oid);
#print STDERR "compare result: $val\n";
ok($val == 0);

$val = $oid->compare($oid3);
#print STDERR "compare result: $val\n";
ok($val == 0);

ok(($oid <=> $oid2) == -1);
ok(($oid2 <=> $oid) == 1);
ok(($oid <=> $oid3) == 0);

ok($oid < $oid2);
ok($oid <= $oid2);
ok($oid2 > $oid);
ok($oid2 >= $oid);
ok($oid == $oid3);
ok($oid <= $oid3);
ok($oid >= $oid3);

ok(new NetSNMP::OID('system') < new NetSNMP::OID('interfaces'));
ok(new NetSNMP::OID('interfaces') > new NetSNMP::OID('system'));
ok(new NetSNMP::OID('sysORTable') > new NetSNMP::OID('system'));

my @a = $oid->to_array();
ok($a[0] == 1 && $a[1] == 3 && $a[2] == 6 && $a[3] == 1 && $#a == 3);
