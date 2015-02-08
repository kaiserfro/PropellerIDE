#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, sys, re
import argparse
import subprocess
import xml.etree.ElementTree as etree
import logging

class Repo:
    def __init__(self, repofile):
        self.name = repofile
        self.root = etree.parse(self.name).getroot()

    def run_git(self, gitargs, repo):
        oldpath = os.getcwd()
        if os.path.exists(repo.attrib['path']):
            os.chdir(repo.attrib['path'])
    
        if gitargs[0] == 'clone':
            os.chdir(oldpath)
    
        gitargs.insert(0,"git")
        process = subprocess.Popen(gitargs, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = process.communicate()

        logging.debug([gitargs, out, err])
        os.chdir(oldpath)

        return out, err

    def gfx(self):
        gfx = self.root.find('gfx')
        if gfx == None:
            raise Exception("Graphics path not found!")

        return gfx.attrib['path']

    def info(self):
        return self.root.find('info')

    def node(self, node):
        return self.root.find(node)

    def version(self):
        version = self.root.find('version')

        out, err = self.run_git(['describe','--tags','--long'], version)
    
        if out == '':
            return "0.0.0"
        else:
            return out.split('-')[0]

    def git_clone(self, repo):
        gitargs = ['clone']
        gitargs.append(repo.attrib['url'])
        gitargs.append(repo.attrib['path'])
        self.run_git(gitargs, repo)
    
    def git_pull(self, repo):
        gitargs = ['pull']
        self.run_git(gitargs, repo)

    def build_tree(self):
        for child in self.root.findall('repo'):
        
            if not 'path' in child.attrib:
                raise KeyError("No path specified in repo")
        
            if not 'url' in child.attrib:
                raise KeyError("No URL specified in repo")
        
            print "--- "+child.attrib['path']+" ---"
        
            if os.path.exists(child.attrib['path']):
                if child.attrib['path'] == '.':
                    self.git_clone(child)
                else:
                    self.git_pull(child)
            else:
                self.git_clone(child)
        
            self.run_git(['submodule','init'], child)
            self.run_git(['submodule','update'], child)
        
            if 'branch' in child.attrib:
                self.run_git(['checkout',child.attrib['branch']], child)
            elif 'tag' in child.attrib:
                self.run_git(['checkout',child.attrib['tag']], child)
            else:
                self.run_git(['checkout','master'], child)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='repo.py - make checking out your Git project more complicated\n\nNo parameters builds the project repository')
    parser.add_argument('project', nargs=1, metavar="PROJECT", help="Project XML config file to check out")
    parser.add_argument('-l','--log', nargs=1, metavar="LEVEL", help="Log level of debug output (DEBUG, INFO, WARNING, ERROR, CRITICAL)")
    parser.add_argument('-v','--version', action='store_true', help="Get the project version")
    parser.add_argument('-g','--gfx', action='store_true', help="Get the graphics path")

    args = parser.parse_args()

    if args.log:
        logging.basicConfig(level=args.log[0])

    repo = Repo(args.project[0])

    if args.version:
        print repo.version()
    elif args.gfx:
        print repo.gfx()
    else:
        repo.build_tree()

