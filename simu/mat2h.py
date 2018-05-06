#!/usr/bin/env python3

import sys

inDimensions = False

for line in sys.stdin:
    l = line.rstrip('\n')

    if inDimensions:
        if l == '%END DIMENSIONS':
            inDimensions = False
            continue

        if l.startswith('global'):
            continue

        if l.startswith('%'):
            print(l.replace('%', '//'))
            continue

        if l.strip() == "":
            print()
            continue

        out = "#define "
        step = 0;
        wasUpper = l[0].isupper()
        for c in l:
            if step == 0:
                if c.isalpha():
                    if c.isupper() and not wasUpper:
                        out += '_' + c
                    else:
                        out += c.upper()
                    wasUpper = c.isupper()
                else:
                    step = 1
            elif step == 1:
                if c == '=':
                    step = 2
            elif step == 2:
                if c.isalpha():
                    if c.isupper() and not wasUpper:
                        out += '_' + c
                    else:
                        out += c.upper()
                    wasUpper = c.isupper()
                else:
                    if c == ';':
                        step = 3
                    else:
                        out += c
            elif step == 3:
                if c == '%':
                    step = 4
                    out += '//'
                else:
                    out += c
            elif step == 4:
                out += c

        print(out.replace('PI', 'M_PI'))

    else:
        if l == '%BEGIN DIMENSIONS':
            inDimensions = True
            continue
