#!perl -w

use strict;

use POSIX qw(setsid);

umask 0;
open STDIN, '/dev/null' or die "Can't read /dev/null: $!";
open(STDOUT, ">>run.log") or die;
open STDERR, '>>run.log' or die "Can't write to /dev/null: $!";
defined(my $pid = fork) or die "Can't fork: $!";
exit if $pid;
setsid or die "Can't start a new session: $!";

print "Starting server..\n";
while(1)
{
	#chdir "/home/gt_partha/shaperprobe/";
	#`/home/gt_partha/shaperprobe/shaperprobeserver >> run.log 2>&1`;
	`./shaperprobeserver >> run.log 2>&1`;
}

