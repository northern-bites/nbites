#!/usr/bin/perl -w
#
# Small utility script to parse the NBites Playbook and
# extract a mapping of the cases within strategies, formations, roles,
# and subroles, and the transitions between them.
#
# Generates this data as a DOT language file file (ex: Strategies.py.dot),
# and builds a .png image of the directed graph.
#
# Cases from each file are assigned a unique color.
#
# Note: the syntax error that pops up (usually near line 4) appears to
# be harmless. Not sure why dot is complaining.
#
# Adapted from mapFSA.pl by Nathan Merritt
#
# Date: February 28, 2012
# Author: Wils Dawson

use strict;
use warnings;
use File::Find;
use Env '@PATH';

# Constants, colors, debug, etc.
my $DEBUG = "";
my $BUILD = "yes";
my $DOT_OPTS = '-Tpng -O'; # outputs in png format

# (colors chosen in no particular order)
my @caseFileColors = qw ( blue Brown Crimson Green Indigo
cyan4 Peru deeppink goldenrod dodgerblue firebrick chocolate4 yellow4 );

# Per FSA data structures
my @addedCategoryFiles;
my $lastCategory;

# access each categoryFile by its categoryName (eg strategies)
# for example, @{$caseFiles{Strategies}{sTwoField}}
# is the array of transitions from the sTwoField strategy to related formations.
our %caseFiles;
our %caseFileUsedColors;

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

# Reads the initial category file in
# sets categoryName, finds caseFiles
# usage: readCategory(fileName, {categoryName|""})
sub readCategory {
    my $currentCategory;
    my $currentCase;
    my $file;

    ($file, $currentCategory) = @_;

    my @foundFiles;

    # transitions inside the state we're reading currently
    my @currentTransitions;

    unless ($currentCategory) {
	# Find currentCategory name
	if ($file =~ /Strategies/) {
	    $currentCategory = "Strategies";
	}
	elsif ($file =~ /Formations/) {
	    $currentCategory = "Formations";
	}
	elsif ($file =~ /Roles/) {
	    $currentCategory = "Roles";
	}
	elsif ($file =~ /SubRoles/) {
	    $currentCategory = "SubRoles";
	}
	die "file: $file has unacceptable name" unless ($currentCategory);
    }

    if  (-f $file)  {
        open(CATEGORY, "$file") or die "cant open $file";

      LINE: while (my $line = <CATEGORY>) {
          chomp $line;

          $_ = $line;

	  if ($currentCategory eq "Strategies") {
	      # extract transition
	      if (/(Formations|Roles|SubRoles)\.(\w+)/) {
		  # add the file for the next case layer.
		  uniqueAdd(\@foundFiles, $1);

		  # add the transition.
		  push @currentTransitions, $2;
	      }
	  }

	  elsif ($currentCategory eq "Formations") {
	      # extract transition
	      if (/(Roles|SubRoles)\.(\w+)/) {
		  # add the file for the next case layer.
		  uniqueAdd(\@foundFiles, $1);

		  # add the transition.
		  push @currentTransitions, $2;
	      }
	  }

	  elsif ($currentCategory eq "Roles") {
	      # extract transition
	      if (/(SubRoles)\.(\w+)/) {
		  # add the file for the next case layer.
		  uniqueAdd(\@foundFiles, $1);

		  # add the transition.
		  push @currentTransitions, $2;
	      }
	  }

	  # No category for SubRoles because that's the end of tree.

	  if (/def\ (\w+)/ || eof) {
	      # we were already parsing a function, it must have ended
	      # "|| eof" guarantees that we add the last case in each file
	      if ($currentCase) {
		  # Since SubRoles are end cases, we use a trick to make sure they
		  # get added by creating a transition to themselves.
		  if ($currentCategory eq "SubRoles")
		  {
		      uniqueAdd(\@{$caseFiles{$currentCategory}{$currentCase}},
				$currentCase);
		  }
		  # add the transitions
		  foreach my $transition (@currentTransitions) {
		      uniqueAdd(\@{$caseFiles{$currentCategory}{$currentCase}},
				$transition);
		  }

		  # clear the transitions so we don't add them in wrong places
		  @currentTransitions = ();
	      }
	      $currentCase = $1;
	  }
      } # end of while <CATEGORY>

        # update the category we're on
        $lastCategory = $currentCategory;

        # add any new files we found that need parsing
        foreach my $new (@foundFiles) {
            uniqueAdd(\@addedCategoryFiles, $new);
        }

        # DEBUG: print out all the @foundFiles, @foundSubCategories
        if ($DEBUG) {
            print "currentCategory = $currentCategory\n";

            print "foundStates\n";
            foreach my $case ( keys %{$caseFiles{$currentCategory}}) {
                print "  $case\n";

                foreach my $transition (@{$caseFiles{$currentCategory}{$case}}) {
                    print "     to $transition\n";
                }
            }
        }
        close (CATEGORY);
    }
}

# find a playbook category file given it's Python-friendly reference
# usually this means just looking for a .py file somewhere downstream
sub findFile ($) {
    our $file_final;

    my $category = shift;
    our $categoryFile = $category . '.py';

    my $dir = $ENV{'NBITES_DIR'};

    # `find ./ -name $categoryFile"
    sub matchesName {
        if ($categoryFile eq $_) {
            $file_final = $File::Find::name;
        }
    }

    File::Find::find({wanted => \&matchesName,}, "$dir");

    undef $categoryFile;
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

    print DOT "digraph $graphName {\nnodesep=.5;ranksep=1.5;\n"; # directed graph

    my $subgraph_number = 0;

    # make all of the nodes
    foreach my $file ( keys %caseFiles ) {
        print DOT "subgraph ${file} {\n"; # states from each file
        # use this if you want to label cluster subgraphs
        #print DOT "[label=\"$file\", fontcolor=\"$caseFileColors[$subgraph_number]\"];\n";

        # remember which color goes with which file, for the legend
        $caseFileUsedColors{$file} = $caseFileColors[$subgraph_number];

        foreach my $case ( keys %{$caseFiles{$file}}) {
            print DOT "  $case [label=\"$case\", fontcolor=\"$caseFileColors[$subgraph_number]\"];\n";
        }
        $subgraph_number++;
        print DOT "\n}\n";
    }

    # and add connections between nodes
    # use 3 levels because there are 4 levels of the playbook
    # i.e. Strategies, Formations, Roles, & SubRoles
    # We have to look through each file in caseFiles to find the next leveln var
    foreach my $file ( keys %caseFiles ) {
        foreach my $case ( keys %{$caseFiles{$file}}) {
          TRANSITION: foreach my $transition (@{$caseFiles{$file}{$case}}) {
	      #print DOT "edge [samehead=\"$transition\"];\n";
	      print DOT "$case -> $transition;\n";
	  }
	}
    }


    # add a legend to the graph: each file name in its color
    print DOT "\n\nedge [style=invis];\n";
    print DOT "Legend [shape=box,width=2];\n";

    my $last;
    foreach my $file (keys %caseFiles ) {
        print DOT "$file [shape=box,width=2,fontcolor=\"$caseFileUsedColors{$file}\"];\n";
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
foreach my $categoryFile (@ARGV) {
    print ":::${categoryFile}:::";

    readCategory($categoryFile, "");

    if ($DEBUG) {
        print "foundSubState Files\n";
        foreach my $category (@addedCategoryFiles) {
            print "  $category\n";
        }
    }

    die "not a proper playbook file!" unless ($lastCategory);

    # for each sub-category found (category file)
    # read it, add its cases and further traverse if necessary


    my $initialCategory = $lastCategory;

    print "\n:::reading sub category files:::";

    foreach my $new_file (@addedCategoryFiles) {
        my $new_file_loc = findFile($new_file);
        #print "opening [${new_file}] at $new_file_loc\n";
        readCategory($new_file_loc, $new_file);
    }

    print "\n:::building .dot and .png files:::\n";
    buildDOT($categoryFile, $initialCategory);

    # and reset the data structures for the next FSA
    %caseFiles = ();
    %caseFileUsedColors = ();
    @addedCategoryFiles = ();
    undef $lastCategory
}
