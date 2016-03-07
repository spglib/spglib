#!/bin/sh

git describe --tags --dirty | sed -e 's/\([.0-9]*\)-\(.*\)-g.*/\2/' -e 's/^v//g' >__nanoversion__.txt
