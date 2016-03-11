#! /usr/bin/env python2

import argparse
import os
import subprocess
import shutil

def main():
    ''' Compile the cycle accurate simulator '''

    # Configure parameters parser
    parser = argparse.ArgumentParser(description='Cycle Accurate Simulator Compiler script')
    subparsers = parser.add_subparsers(title='valid subcommands', dest='command')
    parser_cp = subparsers.add_parser('build')
    parser_cp.add_argument("-c", '--clean', action='store_true', help='clean previously compiled files before compiling')
    parser_cp.add_argument('-v',  '--verbose', action='store_true', help='enable verbose output')
    parser_cl = subparsers.add_parser('clean')
    parser_cl.add_argument('-v',  '--verbose', action='store_true', help='enable verbose output')
    args = parser.parse_args()

    # Run cmake, clean, build
    work_dir = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'obj')

    if args.command == 'clean' or args.clean:
        print ('Cleaning the abstract simulator previous build ...')
        for root, dirs, files in os.walk(work_dir):
            for f in files:
                os.unlink(os.path.join(root, f))
            for d in dirs:
                shutil.rmtree(os.path.join(root, d))

    print ('Running cmake')
    cmd = ['cmake', '..']
    if not args.verbose:
        cmake = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=work_dir)
    else:
        print (cmd)
        cmake = subprocess.Popen(cmd, cwd=work_dir)
    if cmake.wait() != 0:
        print ('cmake FAILED')
        exit(-1)
    print ('cmake done')

    if args.command == 'build':
        cmd = ['make']
        if not args.verbose:
             build = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=work_dir)
        else:
            print (cmd)
            build = subprocess.Popen(cmd, cwd=work_dir)
        if build.wait() != 0:
            print ('Bulding FAILED')
            exit(-1)
        print ('Building done')

# This script runs the main
if __name__ == "__main__":
    main()
