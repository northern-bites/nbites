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

my $DEBUG = "yes";

my @addedStateFiles;
my $lastBehavior;

# access each StateFile by its behaviorName (eg pBrunswick)
# 
our %stateFiles;

# pushes an element to an array iff the element doesn't exist already
sub uniqueAdd {
    my $arrayRef = shift;
    my $toAdd = shift;

    foreach my $i (@$arrayRef) {
	if ($toAdd eq $i) {
	    return;
	}
    }
    push @$arrayRef, $toAdd;
}

# Reads the initial behavior file in
# sets behaviorName, finds stateFiles
# usage: readBehavior(fileName, {behaviorName|""})
sub readBehavior {
    my $currentBehavior;
    my $file;

    ($file, $currentBehavior) = @_;

    my @foundFiles;

    # transitions inside the state we're reading currently
    my @currentTransitions;

    # state variables used to avoid adding helper methods
    my $past_def;
    my $found_legal_return;
    my $currentFunction;
    my $current_can_loop;

    if  (-f $file)  {
	open(BEHAVIOR, "$file") or die "cant open $file";

      LINE: while (my $line = <BEHAVIOR>) {
	  chomp $line;

	  $_ = $line;

	  # found the behavior name
	  if (/setName\(\'(\w+)\'/) {
	      $currentBehavior = $1;
	      next LINE;
	  }

	  # this line adds states from other files so note it
	  if (/addStates\((\w+)\)/) {
	      uniqueAdd(\@foundFiles, $1);
	      next LINE;
	  }

	  # this line defines a function, set state variables accordingly
	  if (/def\ (\w+)/) {
	      # we were already parsing a function, it must have ended
	      # currentFunction is a state? if so add its transitions
	      if ($currentFunction && $found_legal_return)  {
		  # add the transitions
		  foreach my $transition (@currentTransitions) {
		      uniqueAdd(\@{$stateFiles{$currentBehavior}{$currentFunction}}, $transition);
		  }

		  # and note if the state contains a loop
		  if ($current_can_loop) {
		      push @{$stateFiles{$currentBehavior}{$currentFunction}}, "player.stay()";
		      $current_can_loop = "";
		  }
		  $found_legal_return = "";
	     }

	      $currentFunction = $1;
	      next LINE;
	  }

	  # legal ways to exit a state
	  if (/return\ player\.go(Now|Later)\(\'(\w+)/ ) {
	      $found_legal_return = 1;
	      push @currentTransitions, $2;
	      next LINE;
	  }

	  # marks a state that can loop on itself
	  if (/return\ player\.stay\(\)/) {
	      $found_legal_return = 1;
	      $current_can_loop = 1;
	      next LINE;
	  }
      }

	die "currentBehavior not set, this is bad" if (not $currentBehavior);
	# update the behavior we're on
	$lastBehavior = $currentBehavior;

	# add any new files we found that need parsing
	foreach my $new (@foundFiles) {
	    uniqueAdd(\@addedStateFiles, $new);
	}

	# DEBUG: print out all the @foundFiles, @foundSubStates
	if ($DEBUG) {
	    print "currentBehavior = $currentBehavior\n";

	    print "foundStates\n";
	    foreach my $state ( keys %{$stateFiles{$currentBehavior}}) {
		print "  $state\n";

		foreach my $transition (@{$stateFiles{$currentBehavior}{$state}}) {
		    print "     to $transition\n";
		}
	    }
	}

	close (BEHAVIOR);
    }
}

# find a behavior state file given it's Python-friendly reference
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

# execution starts here
foreach my $behaviorFile (@ARGV) {

    print ":::${behaviorFile}:::\n\n";

    readBehavior($behaviorFile, "");

    print "foundSubState Files\n";
    foreach my $stateFile (@addedStateFiles) {
	print "  $stateFile\n";
    }

    # for each sub-behavior found (state file)
    # read it, add its states and further traverse if necessary
    die "not a top level behavior file!" if (not $lastBehavior);

    print "\n:::reading sub state files:::\n\n";

    foreach my $new_file (@addedStateFiles) {
	my $new_file_loc = findFile($new_file);
	#print "opening [${new_file}] at $new_file_loc\n";
	readBehavior($new_file_loc, $new_file);
    }
}
