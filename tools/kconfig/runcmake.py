#!/usr/bin/env python3
import re
import subprocess
import os

cmake_definition = ['cmake -S . -B build -G "Unix Makefiles"']

def gen_cmake(filename):
    try:
        config = open(filename, 'r')
    except:
        print('open config:%s failed' % filename)
        return

    empty_line = 1

    for line in config:
        line = line.lstrip(' ').replace('\n', '').replace('\r', '').replace('"','')

        if len(line) == 0:
            continue
        if line[0] == '#':
            if len(line) == 1:
                if empty_line:
                    continue

                #print('\n')
                empty_line = 1
                continue

            if line.startswith('# CONFIG_'):
                line = ' ' + line[9:]
            else:
                line = line[1:]
                #print('/*%s */\n' % line)

            empty_line = 0
        else:
            empty_line = 0
            setting = line.split('=')

            if len(setting) >= 2:
                if setting[0].startswith('CONFIG_'):
                    setting[0] = setting[0][7:]

                    cmake_definition.append('-D'+line)

fn = '.config'
gen_cmake(fn)
cmake_definition.append('..')
print(cmake_definition)

os.chdir('build')
subprocess.call(cmake_definition)
subprocess.call(['make','-j4'])
