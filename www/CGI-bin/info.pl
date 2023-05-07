#!/usr/bin/perl
use strict;
use warnings;
use CGI;

# Create a CGI object
my $cgi = CGI->new;

# Get the form data
my $name = $cgi->param('name');
my $gender = $cgi->param('gender');
my $profession = $cgi->param('profession');
my $sport = $cgi->param('sport');

# Print the form data
print $cgi->header('text/html');
print "<html><head><title>Form Data</title></head><body>\n";
print "<h2>Here is the infomation you sent!</h2>\n";
print "<p>Name: $name</p>\n";
print "<p>Gender: $gender</p>\n";
print "<p>Profession: $profession</p>\n";
print "<p>Sport: $sport</p>\n";
print "</body></html>\n";

