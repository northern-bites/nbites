#!/usr/bin/env python

"""
usage: copy_diff.py <source_git_repo_base> <tgt_dir_base>

only works on git tracked files!
"""

import os, argparse, subprocess, sys, shutil

parser = argparse.ArgumentParser()

parser.add_argument('source')
parser.add_argument('dest')

def _l(fmt, *args, **kwargs):
    print '>>', fmt.format(*args, **kwargs)

def main():
    _l('start')

    args = parser.parse_args()

    source_base = args.source
    dest_base = args.dest

    for path in [source_base, dest_base]:
        if not os.path.isdir(path):
            _l('its not a directory: {}', path)
            return 1

    output = subprocess.check_output([
        'git', '-C', source_base, 'status', '-s'
    ])

    for line in output.splitlines():
        status = line[0:2]
        path = line[2:].strip()

        _l('{} : {}', status, path)

        full_source_path = os.path.join(source_base, path)
        full_dest_path = os.path.join(dest_base, path)

        if 'D' in status:
            if os.path.isfile(full_dest_path):
                _l('deleting: {}', full_dest_path)
                os.remove(full_dest_path)
            else:
                _l('ignoring...')
        else:
            if not os.path.isfile(full_source_path):
                _l('ahhhhh! this should be a valid path: {}', full_source_path)
                return 2

            _l('copy: {} ---> {}', full_source_path, full_dest_path)
            shutil.copy(full_source_path, full_dest_path)

    _l('done!')



if __name__ == '__main__':
    sys.exit(main())
else:
    _l('yo why you importing me you goose!')