#!/usr/bin/perl -w
#
# Small utility script to parse a behavior file (ex: pBrunswick.py) and
# extract a mapping of all FSA states and which states they transition to.
#
# Generates this data as a text file (ex: pBrunswick.py.states) and as
# a PDF graph using TikZ.
#
# Date: January 21, 2011
# Author: Nathan Merritt

use strict;
use warnings;
use File::Find;
use Cwd;

my $DEBUG = "yes";

my @addedStateFiles;
my $lastBehavior;

# access each state by its behaviorName
my %states;
my %subStates;

our @temp_ToParse;

# Reads the initial behavior file in
# sets behaviorName, finds stateFiles
# usage: readBehavior(fileName, {behaviorName|""})
sub readBehavior {
    my $currentBehavior;
    my $file;

    ($file, $currentBehavior) = @_;

    my @foundStates;
    my @foundSubStates;

    # state variables used to avoid adding helper methods
    my $past_def;
    my $found_legal_return;
    my $currentFunction;


    if  (-f $file)  {
	open(BEHAVIOR, "$file") or die "cant open $file";

      LINE: while (my $line = <BEHAVIOR>) {
	  chomp $line;

	  $_ = $line;

	  # found the behavior name!
	  if (/setName\(\'(\w+)\'/) {
	      $currentBehavior = $1;
	      next LINE;
	  }

	  # this line adds states from other files so note it
	  if (/addStates\((\w+)\)/) {
	      push @foundSubStates, $1;
	      next LINE;
	  }

	  # this line defines a function, set state variables accordingly
	  if (/def\ (\w+)/) {
	      # we were already parsing a function, it must have ended
	      if ($currentFunction)  {
		  # currentFunction is a state, so add it
		  if ($found_legal_return) {
		      push @foundStates, $currentFunction;
		      $found_legal_return = "";
		  }
	      }

	      $currentFunction = $1;
	  }

	  # legal ways to exit a state
	  if (/return\ player\.go(Now|Later)\(\'(\w+)/ ) {
	      $found_legal_return = 1;
	  }

	  # marks a state that can loop on itself
	  if (/return\ player\.stay\(\)/) {
	      $found_legal_return = 1;
	  }
      }

	# assign back to the data structure
	if ($currentBehavior) {
	    $states{currentBehavior} = @foundStates;
	    $subStates{currentBehavior} = @foundSubStates;
	    $lastBehavior = $currentBehavior;

	    # TODO: remove this!
	    @temp_ToParse = @foundSubStates;
	}

	# DEBUG: print out all the @foundStates, @foundSubStates
	if ($DEBUG) {
	    print "currentBehavior = $currentBehavior\n";

	    print "foundStates\n";
	    foreach my $state (@foundStates) {
		print "  $state\n";
	    }

	    print "foundSubStates\n";
	    foreach my $state (@foundSubStates) {
		print "  $state\n";
	    }
	}

	close (BEHAVIOR);
    }
}

# finds a state file given it's Python-friendly reference
# usually this means just looking for a .py file somewhere downstream
sub findFile ($) {
    our $file_final;

    my $behavior = shift;
    our $behaviorFile = $behavior . '.py';

    my $dir = $ENV{'PWD'};

    # `find ./ -name $behaviorFile"
    sub matchesName {
	if ($behaviorFile eq $_) {
	    $file_final = $File::Find::name;
	}
    }

    File::Find::find({wanted => \&matchesName,}, "$dir");

    undef $behaviorFile;
    return $file_final;
}

foreach my $behaviorFile (@ARGV) {

    print ":::${behaviorFile}:::\n\n";

    readBehavior($behaviorFile, "");

    # for each sub-behavior found (state file)
    # read it, add its states and further traverse if necessary
    die "not a top level behavior file!" if (not $lastBehavior);

    print "reading sub state files\n";

    foreach my $subBehavior (@temp_ToParse) {
	print "  $subBehavior\n";

	my $subBehavior_file = findFile($subBehavior);

	if ($subBehavior_file) {
	    print "$subBehavior_file\n";
	}

	readBehavior($subBehavior_file, $subBehavior);
   }
}
