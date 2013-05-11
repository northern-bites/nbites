#!/usr/bin/env python

import sys, os, time, subprocess
from github import Github, GithubException

username = os.environ['GITHUB_USERNAME']
password = os.environ['GITHUB_PASSWORD']
cl_id = os.environ['GITHUB_CLIENT_ID']
cl_secret = os.environ['GITHUB_CLIENT_SECRET']


repo_name = 'northern-bites/nbites'
org_name, repo_short_name = repo_name.split('/')


def handle(commit, head):
    # Get the Pull Request.

    fail = subprocess.call(["get-pull-req.sh", "-b"+head.ref,
                            "-n"+head.user.login, "-u"+head.repo.git_url])

    if fail:
        commit.create_status(state='error', description="What the hell's goin' on in the engine room? Were there monkeys? Some terrifying space monkeys maybe got loose?")

    # Run the Build Script.
    fail = subprocess.call("build-script.sh")

    if fail:
        commit.create_status(state='failure', description="This food is problematic.")

    else:
        commit.create_status(state='success', description="If you can't do something smart, do something right.")


#########################
#                       #
# Execution Starts Here #
#                       #
#########################

g = Github(username, password, client_id = cl_id, client_secret = cl_secret, user_agent = username)

org = g.get_organization(org_name)
repo = org.get_repo(repo_short_name)

checked = False

while(True):
    commit = None

    try:
        # Check GitHub once a minute
        if (checked):
            if (int(time.time()) % 60 != 0):
                checked = False
                continue

        # Two ifs so that we don't repeat the check more than once in the
        # first second.
        if (int(time.time()) % 60 != 0):
            continue

        for pull in repo.get_pulls():
            checked = True
            handled = False
            print "Pull #{0} found: {1}".format(pull.number, pull.title)

            commit = repo.get_commit(pull.head.sha)

            for status in commit.get_statuses():
                # If this loop is executing, then this PR has been
                # previously handled and should be skipped.
                handled = True
                break

            if (not handled):
                print "\tHandling new data..."

                commit.create_status(state='pend', description='Time for some thrilling heroics.')

                handle(commit, pull.head)

    except GithubException:
        if commit == None:
            print "Serenity Encountered An Unknown Error"
        else:
            commit.create_status(state='error', description="What the hell's goin' on in the engine room? Were there monkeys? Some terrifying space monkeys maybe got loose?")
