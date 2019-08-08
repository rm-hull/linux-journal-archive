#!/usr/local/bin/perl -- -*- C -*-

# Perl Routines to Manipulate CGI input
# S.E.Brenner@bioc.cam.ac.uk
# $Header: /people/seb1005/http/cgi-bin/RCS/cgi-lib.pl,v 1.2 1994/01/10 15:05:40 seb1005 Exp $
#
# Copyright 1993 Steven E. Brenner  
# Unpublished work.
# Permission granted to use and modify this library so long as the
# copyright above is maintained, modifications are documented, and
# credit is given for any use of the library.

# ReadParse
# Reads in GET or POST data, converts it to unescaped text, and puts
# one key=value in each member of the list "@in"
# Also creates key/value pairs in %in, using '\0' to separate multiple
# selections

# If a variable-glob parameter (e.g., *cgi_input) is passed to ReadParse,
# information is stored there, rather than in $in, @in, and %in.

sub ReadParse {
  if (@_) {
    local (*in) = @_;
  }

  local ($i, $loc, $key, $val);

  # Read in text
  if ($ENV{'REQUEST_METHOD'} eq "GET") {
    $in = $ENV{'QUERY_STRING'};
  } elsif ($ENV{'REQUEST_METHOD'} eq "POST") {
    for ($i = 0; $i < $ENV{'CONTENT_LENGTH'}; $i++) {
      $in .= getc;
    }
  } 

  @in = split(/&/,$in);

  foreach $i (0 .. $#in) {
    # Convert plus's to spaces
    $in[$i] =~ s/\+/ /g;

    # Convert %XX from hex numbers to alphanumeric
    $in[$i] =~ s/%(..)/pack("c",hex($1))/ge;

    # Split into key and value.
    $loc = index($in[$i],"=");
    $key = substr($in[$i],0,$loc);
    $val = substr($in[$i],$loc+1);
    $in{$key} .= '\0' if (defined($in{$key})); # \0 is the multiple separator
    $in{$key} .= $val;
  }

  return 1; # just for fun
}

# PrintHeader
# Returns the magic line which tells WWW that we're an HTML document

sub PrintHeader {
  return "Content-type: text/html\n\n";
}

# PrintVariables
# Nicely formats variables in an associative array passed as a parameter
# And returns the HTML string.

sub PrintVariables {
  local (%in) = @_;
  local ($old, $out);
  $old = $*;  $* =1;
  $output .=  "<DL COMPACT>";
  foreach $key (sort keys(%in)) {
    ($out = $in{$key}) =~ s/\n/<BR>/g;
    $output .=  "<DT><B>$key</B><DD><i>$out</I><BR>";
  }
  $output .=  "</DL>";
  $* = $old;

  return $output;
}

# PrintVariablesShort
# Nicely formats variables in an associative array passed as a parameter
# Using one line per pair (unless value is multiline)
# And returns the HTML string.

sub PrintVariablesShort {
  local (%in) = @_;
  local ($old, $out);
  $old = $*;  $* =1;
  foreach $key (sort keys(%in)) {
    if (($out = $in{$key}) =~ s/\n/<BR>/g) {
      $output .= "<DL COMPACT><DT><B>$key</B> is <DD><i>$out</I></DL>";
    } else {
      $output .= "<B>$key</B> is <i>$out</I><BR>";
    }
  }
  $* = $old;

  return $output;
}

1; #return true





