#!/usr/bin/perl
#
# Nagios status.dat file parser.
# - csh
#
use strict;
use Data::Dumper;

my($STATUS_FILE)  = 'status.dat';
my($tmpPath)      = '/tmp';
my($tmpFile)      = $tmpPath."/$$".generate_random_string(13).'.tmp';

my(%data)            = ();
my($entryType)       = 0;
my($notifications)   = 1;
my($current_state)   = 0;
my($active_chks)     = 0;
my($svc_acknowleged) = 0;
my($service_desc)    = '';
my($plugin_output)   = '';
my($svc_type)        = 0;
my($hostname)        = '';
my($chk_cmd)         = '';
my($author)          = '';
my($entry_time)      = 0;
my($comment_data)    = '';

system("/bin/cp $STATUS_FILE $tmpFile");

if(-e $tmpFile) {
    open(FILE, $tmpFile);
    while(<FILE>) {
           my $line = $_;
           chomp($line);
           $line =~ s/^\s+(.*)/$1/;
           next if $line =~ /^$/;

           if($line =~ /^(info|programstatus|hoststatus|servicestatus|contactstatus|hostcomment|servicecomment)\s\{/)  { $entryType = $1; }
           next if $entryType =~ /(info|programstatus|contactstatus)/;


           if($line =~ /^host_name=(.*)$/)                      { $hostname        = $1;  }
           if($line =~ /^check_command=(.*)$/)                  { $chk_cmd         = $1;  }
           if($line =~ /^notifications_enabled=(.*)$/)          { $notifications   = $1;  }
           if($line =~ /^current_state=(.*)$/)                  { $current_state   = $1;  }
           if($line =~ /^active_checks_enabled=(.*)$/)          { $active_chks     = $1;  }
           if($line =~ /^problem_has_been_acknowledged=(.*)$/)  { $svc_acknowleged = $1;  }
           if($line =~ /^service_description=(.*)$/)            { $service_desc    = $1;  }
           if($line =~ /^plugin_output=(.*)$/)                  { $plugin_output   = $1;  }
           if($line =~ /^author=(.*)$/)                         { $author          = $1;  }
           if($line =~ /^entry_time=(.*)$/)                     { $entry_time      = $1;  }
           if($line =~ /^comment_data=(.*)$/)                   { $comment_data    = $1;  }


           if($line =~ /^\}$/) {
               if($entryType eq 'hoststatus') {
                  $data{$hostname}{$entryType}{'command'}                 = $chk_cmd;
                  $data{$hostname}{$entryType}{'notifications_enabled'}   = $notifications;
                  $data{$hostname}{$entryType}{'current_state'}           = $current_state;
                  $data{$hostname}{$entryType}{'active_checks_enabled'}   = $active_chks;
                  $data{$hostname}{$entryType}{'acknowledged'}            = $svc_acknowleged;
                  $data{$hostname}{$entryType}{'plugin_output'}           = $plugin_output;

               } elsif($entryType eq 'hostcomment') {
                  $data{$hostname}{$entryType}{$entry_time}{'author'}      = $author;
                  $data{$hostname}{$entryType}{$entry_time}{'comment'}     = $comment_data;

               } elsif($entryType eq 'servicestatus') {
                  $data{$hostname}{$entryType}{$service_desc}{'command'}                = $chk_cmd;
                  $data{$hostname}{$entryType}{$service_desc}{'plugin_output'}          = $plugin_output;
                  $data{$hostname}{$entryType}{$service_desc}{'current_state'}          = $current_state;
                  $data{$hostname}{$entryType}{$service_desc}{'notifications_enabled'}  = $notifications;
                  $data{$hostname}{$entryType}{$service_desc}{'active_checks_enabled'}  = $active_chks;
                  $data{$hostname}{$entryType}{$service_desc}{'acknowledged'}           = $svc_acknowleged;
               }

          }

    }
    close(FILE);
    unlink($tmpFile);

   print Dumper(%data);

}


# for tmp file creation
sub generate_random_string {
 my $length_of_randomstring=shift; # the length of the random string to generate
 my @chars=('a'..'z','A'..'Z','0'..'9','_');
 my $random_string;
 foreach (1..$length_of_randomstring) {
   # rand @chars will generate a random
   # number between 0 and scalar @chars
   $random_string.=$chars[rand @chars];
 }
 return $random_string;
}

