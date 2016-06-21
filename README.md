# ShaperProbe

End-to-end traffic shaping detection and estimation tool.

The goal of DiffProbe is to detect if an ISP is classifying certain kinds of traffic as "low priority", providing different levels of service for them. DiffProbe actively (and non-intrusively) probes the network path and tries to diagnose the nature and extent of traffic discrimination. This page presents a module of DiffProbe, called ShaperProbe. ShaperProbe tries to answer the question:
Is the ISP shaping my traffic? In other words, is my "connection speed" dropped automatically to a low rate after some time?
We detect traffic shaping, which means that the customer gets a large rate for a certain number of bytes, and then the rate is dropped automatically to a lower value. If a user gets rate limited for certain time periods, he/she can detect that observing the capacity estimates given by ShaperProbe.

DiffProbe's ShaperProbe makes use of the Measurement Lab (M-Lab) research platform. To learn more, go here. In order to advance network research, all collected data will be made publicly accessible.



Status:
January '12: new version released.
November '09: beta version released.

We currently support clients on Windows, Mac OS X, and Linux platforms.

The Tool:
Download links:

Microsoft Windows (binary): http://netinfer.net/diffprobe/ShaperProbe.exe
MAC OS X (binary): http://netinfer.net/diffprobe/ShaperProbe.dmg
Linux (source): http://netinfer.net/diffprobe/shaperprobe.tgz
FreeBSD: Available in the ports collection (maintained by Josh Carroll).
Old versions: Windows (binary), MAC OS X (binary), Linux (source).

Running: On Windows, double-click the executable. On MAC OS X, double-click the disk-image to mount, and then double-click the binary.
Compiling and running: The client needs the following library to compile: glibc. On Ubuntu and Debian Linux, this library is available in the package libc6-dev. You probably already have this package if you have compiled code on your machine before. After extracting the archive, compile and run ShaperProbe: 
$ make
$ ./prober 
In order to compile on MAC OS X, do make -f Makefile.osx


Sample Output:
The following is an example output, not taken from any particular ISP:
$ ./prober
DiffProbe alpha release. April 2009.
Shaper Detection Module.

Connected to server 123.231.123.231.

Estimating capacity:
 Upstream: 10800.39 Kbps.
 Downstream: 37127.07 Kbps.

Checking for traffic shapers:
 Upstream: Burst size: 5402 KB; Shaping rate: 1008.00 Kbps.

 sending measurement data to server..done.
 Downstream: No shaper detected.

For more information, visit: http://www.cc.gatech.edu/~partha/diffprobe
The tool starts by estimating the path's capacity. This is followed by detection of traffic shaping on the path in both upstream and downstream directions.

Output messages:
- Burst size: ...; Shaping rate: ...: This quantifies the traffic shaping (if any) done by the ISP at the time of measurement. Shaping rate is the rate-limiting rate.
- All servers are busy or Cannot connect to server. Server may be busy; please try in a few minutes.: For measurement accuracy, our servers currently support only one client at a time. Please retry in a few minutes. Also verify that your firewall has ports TCP 55000 (outgoing), TCP 55005 (outgoing), and UDP 55005 (both directions) open. The tool works fine over a NAT (likely, if you are a home user).
- Incompatible server. Please download the latest version of DiffProbe client.: It is likely that your platform is not yet supported by ShaperProbe. Please try the version on this page, or if possible, please let us know by sending a mail.
- 
