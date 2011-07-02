#!/usr/bin/perl -w
#
# Small utility script to parse a behavior file (ex: pBrunswick.py) and
# extract a mapping of all FSA states and which states they transition to.
#
# Generates this data as a DOT language file file (ex: pBrunswick.py.dot),
# and builds a .png image of the directed graph.
#
# States from each file are assigned a unique color. "goNow"
# transitions are marked with solid lines while "goLater" transitions
# use dotted lines.
#
# Note: the syntax error that pops up (usually near line 4) appears to
# be harmless. Not sure why dot is complaining.
#
# Date: January 21, 2011
# Author: Nathan Merritt

use strict;
use warnings;
use File::Find;
use Env '@PATH';

# Constants, colors, debug, etc.
my $DEBUG = "";
my $BUILD = "yes";
my $DOT_OPTS = '-Tpng -O'; # outputs in png format

my $nowChar = '*';
my $laterChar = '^';

# (colors chosen in no particular order)
my @stateFileColors = qw ( blue Brown Crimson Green Indigo
cyan4 Peru deeppink goldenrod dodgerblue firebrick chocolate4 yellow4 );

# Per FSA data structures
my @addedStateFiles;
my $lastBehavior;

# access each StateFile by its behaviorName (eg pBrunswick)
# for example, @{$stateFiles{PenaltyKickStates}{penaltyBallInOppGoalbox}{chase}}
# is the array of transitions from the chase state to other states
our %stateFiles;
our %stateFileUsedColors;

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
          }

          # this line adds states from other files so note it
          elsif (/addStates\((\w+)\)/) {
              uniqueAdd(\@foundFiles, $1);
          }

          # legal ways to exit a state
          elsif (/return\ \w+\.go(Now|Later)\(\'(\w+)/ ) {
              $found_legal_return = 1;
              my $toState = $2;
              # mark the transition as "now" or "later"
              if ($1 =~ /Later/) {
                  push @currentTransitions, ($toState . $laterChar);
              }
              else {
                  push @currentTransitions, ($toState . $nowChar);
              }
          }

          # marks a state that can loop on itself
          elsif (/return\ \w+\.stay\(\)/) {
              $found_legal_return = 1;
              $current_can_loop = 1;
          }

          # this line defines a function, set state variables accordingly
	  # don't use elsif here incase eof is in one of the lines already found
          if (/def\ (\w+)/ || eof) {
              # we were already parsing a function, it must have ended
              # currentFunction is a state? if so add its transitions
              # "|| eof" guarantees that we add the last state in each file
              if ($currentFunction && $found_legal_return)  {
                  # add the transitions
                  foreach my $transition (@currentTransitions) {
                      uniqueAdd(\@{$stateFiles{$currentBehavior}{$currentFunction}}, $transition);
                  }

		  # clear the transitions so we don't add them in wrong places
		  @currentTransitions = ();

                  # and note if the state contains a loop
                  if ($current_can_loop) {
                      push @{$stateFiles{$currentBehavior}{$currentFunction}}, "player.stay()";
                      $current_can_loop = "";
                  }
                  $found_legal_return = "";
              }
              $currentFunction = $1;
          }
      } # end of while <BEHAVIOR>

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
                    my $timing;
                    if ($transition =~ /\*/) {
                        $timing = " (now)";
                    }
                    else {
                        $timing = " (later)";
                    }
                    print "     to $transition $timing\n";
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

    my $dir = $ENV{'NBITES_DIR'};

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

    print DOT "digraph $graphName {\n\n"; # directed graph

    my $subgraph_number = 0;

    # make all of the nodes
    foreach my $file ( keys %stateFiles ) {
        print DOT "subgraph ${file} {\n"; # states from each file
        # use this if you want to label cluster subgraphs
        #print DOT "[label=\"$file\", fontcolor=\"$stateFileColors[$subgraph_number]\"];\n";

        # remember which color goes with which file, for the legend
        $stateFileUsedColors{$file} = $stateFileColors[$subgraph_number];

        foreach my $state ( keys %{$stateFiles{$file}}) {
            print DOT "  $state [label=\"$state\", fontcolor=\"$stateFileColors[$subgraph_number]\"];\n";
        }

        $subgraph_number++;
        print DOT "\n}\n";
    }

    # and add connections between nodes
    foreach my $file ( keys %stateFiles ) {
        foreach my $state ( keys %{$stateFiles{$file}}) {
          TRANSITION: foreach my $toState (@{$stateFiles{$file}{$state}}) {
              if ($toState !~ /player\.stay/) {
                  if ($toState =~ /\*/) { # nowChar marker
                      $toState =~ s/\*//;
                      print DOT "$state -> $toState;\n";
                  }
                  else {
                      $toState =~ s/\^//; # goLater transitions are dotted
                      print DOT "$state -> $toState [style=\"dotted\"];\n";
                  }
              }
	      else { # add loops to ourselves
		  print DOT "$state -> $state;\n";
	      }
          }
        }
    }

    # add a legend to the graph: each file name in its color
    print DOT "\n\nedge [style=invis];\n";
    print DOT "Legend [shape=box,width=2];\n";

    my $last;
    foreach my $file (keys %stateFiles ) {
        print DOT "$file [shape=box,width=2,fontcolor=\"$stateFileUsedColors{$file}\"];\n";
        if ($last) {
            print DOT "$last -> $file;\n";
        }
        else {
            print DOT "Legend -> $file;\n";
        }
        $last = $file;
    }

    print DOT "\n}\n";
    close (DOT);

    # now make a PNG file out of the .dot file we just built
    my $dot = 'dot';
    my $exec_exists = grep -x "$_/$dot", @PATH;

    if ($exec_exists && $BUILD) {
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

    # and reset the data structures for the next FSA
    %stateFiles = ();
    %stateFileUsedColors = ();
    @addedStateFiles = ();
    undef $lastBehavior
}
