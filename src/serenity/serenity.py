#!/usr/bin/env python

import argparse
import subprocess
import ast

def acquire_cache_info():
    print "Please provide information to get your code."
    cache['git_name']= raw_input("Enter your github username: ")
    cache['git_url'] = raw_input("Enter your github url "+
                                 "(i.e. git://github.com/user/nbites.git): ")

    print "You entered:\n\tUsername:\t{0}\n\tURL:\t\t{1}".format(cache['git_name'],
                                                                 cache['git_url'])

    if (args.cache == None):
        ack = None
        while ack == None:
            ack = raw_input("Would you like to cache this data for next time?(Y/n) ")
            if (ack == "" or ack == "y" or ack == "Y"):
                write_cache()
            elif (ack != "n" and ack != "N"):
                print "Did not recognize input..."
                ack = None
    elif (args.cache == True):
        write_cache()

def write_cache():
    with open('cache.txt', 'w') as f:
        f.write(str(cache))

def read_cache():
    with open('cache.txt') as f:
        cache = ast.literal_eval(f.read())


parser = argparse.ArgumentParser(description="Sends a job to Serenity.")

job_group = parser.add_mutually_exclusive_group()
job_group.add_argument('-c', '--compile', action='store_true', default=False,
                       help='Only compiles. No test or pull request.')
job_group.add_argument('-m', '--mock', action='store_true', default=False,
                       help='Compiles and tests with no pull request.')

cache_group = parser.add_mutually_exclusive_group()
cache_group.add_argument('--cache', action='store_true', default=None,
                         help='Overwrite cache information for future use.')
cache_group.add_argument('--no-cache', action='store_false', dest='cache',
                         help='Do not cache information this time.')

parser.add_argument('-b', '--branch',
                    help='Branch to test on. Defaults to current branch.')

args = parser.parse_args()

branch = subprocess.check_output("git-branch.sh").rstrip()

if (args.branch == None):
    args.branch = branch

cache = {}

if (args.cache == None):
    print "Attempting to use cached data."
    try:
        read_cache()
        print "Using Cache:\n\tUsername:\t{0}\n\tURL:\t\t{1}".format(cache['git_name'],
                                                                         cache['git_url'])
    except IOError:
        print "Could not find a cache file."
        acquire_cache_info()

else:
    acquire_cache_info()


print args
