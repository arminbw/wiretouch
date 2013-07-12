#!/usr/bin/env perl

my($targetPID, $targetVID) = (24577, 1027);
my($plistpath) = "/System/Library/Extensions/FTDIUSBSerialDriver.kext/Contents/Info.plist";

$> eq 0 or die "Use sudo to run this script.";
(-e $plistpath) or die "The FTDI driver configuration file $plistpath is missing!";

print "patching $plistpath...\n";

open (FILE, "<$plistpath") or die "Failed to open $plistpath.";

my(@out) = ();
my(@entry) = ();
my(@cfg) = ();
my($dictdepth, $inpersonalities, $pid, $vid, $curname, $ignconfig) = (0, 0, 0, 0, "", 0);

while (<FILE>) {
   (m/<\/dict>/) and $dictdepth--;

   if ($inpersonalities && $dictdepth > 2) {
      ($pid < 0 && m/<integer>([0-9]+)<\/integer>/) and $pid = int($1);
      ($vid < 0 && m/<integer>([0-9]+)<\/integer>/) and $vid = int($1);
      (m/idProduct/) and $pid = -1;
      (m/idVendor/) and $vid = -1;
      
      (m/<key>ConfigData<\/key>/) and $ignconfig = 1;
   
      push @entry, $_ unless $ignconfig;
      push @cfg, $_ if $ignconfig;
      
      ($ignconfig && $dictdepth == 3 && m/<\/dict>/) and $ignconfig = 0;
   } elsif ($inpersonalities) {
      if ($#entry > 0) {
         if ($targetPID == $pid && $targetVID == $vid) {
            print "Adding ConfigData for device \"$curname\"...\n";
            @cfg = ("\t\t<key>ConfigData</key>\n", "\t\t\t<dict>\n", "\t\t\t\t<key>BaudRates</key>\n",
               "\t\t\t\t<dict>\n", "\t\t\t\t\t<key>B300</key>\n", "\t\t\t\t\t<integer>500000</integer>\n",
               "\t\t\t\t</dict>\n", "\t\t\t\t<key>LatencyTimer</key>\n", "\t\t\t\t<integer>1</integer>\n",
               "\t\t\t</dict>\n");
         }
         
         $#cfg and push @entry, @cfg;
         push @out, @entry;
         
         @entry = (); @cfg = (); $pid = $vid = 0;
      }
      
      push @out, $_;
   
      ($dictdepth > 1 && m/<key>(.*)<\/key>/) and $curname = $1;   
   } else {
      push @out, $_;
   }
   
   (m/IOKitPersonalities/) and $inpersonalities = 1;
   (m/<dict>/) and $dictdepth++;
}

close FILE;

open (FILE, ">$plistpath") or die "Failed to open $plistpath for writing.";
print FILE @out;
close FILE;

print "finished writing Info.plist, re-loading kext now...\n";

system("kextunload -b com.FTDI.driver.FTDIUSBSerialDriver");
system("kextload -b com.FTDI.driver.FTDIUSBSerialDriver");

print "done.";
