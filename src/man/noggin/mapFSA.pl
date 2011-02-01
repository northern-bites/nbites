#!/usr/bin/perl -w
#
# Small utility script to parse a behavior file (ex: pBrunswick.py) and
# extract a mapping of all FSA states and which states they transition to.
#
# Generates this data as a DOT language file file (ex: pBrunswick.py.dot),
# and builds a .png image of the directed graph. 
#
# Date: January 21, 2011
# Author: Nathan Merritt

use strict;
use warnings;
use File::Find;
use Env '@PATH';

my $DEBUG = "";

my $DOT_OPTS = '-Tpng -O'; # outputs in png format

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

# Creates a graph description using the DOT language
# Basically once we've got the layout into a .dot file,
# we can visualize it any way that we want to (xml, png...)
# see: www.graphviz.org
# and http://www.graphviz.org/doc/info/lang.html
sub buildDOT {
    my $graphFile = shift;
    $graphFile = $graphFile . ".dot"; # fixes ambiguous order of ops
    my $graphName = shift;

    open(DOT, ">$graphFile") or die $!;

    print DOT "digraph $graphName {\n\n";

    # make all of the nodes
    foreach my $file ( keys %stateFiles ) {
	print DOT "subgraph $file {\n";

	foreach my $state ( keys %{$stateFiles{$file}}) {
	    #print "  $state\n";
	    print DOT "  $state [label=\"$state\"];\n";
	}

	print DOT "\n}\n";
    }

    # and add connections between nodes
    foreach my $file ( keys %stateFiles ) {
	foreach my $state ( keys %{$stateFiles{$file}}) {
	    TRANSITION: foreach my $toState (@{$stateFiles{$file}{$state}}) {
		if ($toState !~ /player\.stay/) {
		    print DOT "$state -> $toState\;\n";
		}
	    }
	}
    }

    print DOT "\n}\n";
    close (DOT);

    # now make a PNG file out of the .dot file we just built
    my $dot = 'dot';
    my $exec_exists = grep -x "$_/$dot", @PATH;
    if ($exec_exists) {
	`$dot $DOT_OPTS $graphFile`;
    }
    else {
	print "You need to install dot to build the .dot file\n";
	print "get it at http://www.graphviz.org/\n";
    }
}

# execution starts here
foreach my $behaviorFile (@ARGV) {

    print ":::${behaviorFile}:::";

    readBehavior($behaviorFile, "");

    if ($DEBUG) {
	print "foundSubState Files\n";
	foreach my $stateFile (@addedStateFiles) {
	    print "  $stateFile\n";
	}
    }

    # for each sub-behavior found (state file)
    # read it, add its states and further traverse if necessary
    die "not a top level behavior file!" if (not $lastBehavior);

    my $initialBehavior = $lastBehavior;

    print "\n:::reading sub state files:::";

    foreach my $new_file (@addedStateFiles) {
	my $new_file_loc = findFile($new_file);
	#print "opening [${new_file}] at $new_file_loc\n";
	readBehavior($new_file_loc, $new_file);
    }

    print "\n:::building .dot and .png files:::\n";
    buildDOT($behaviorFile, $initialBehavior);
}
