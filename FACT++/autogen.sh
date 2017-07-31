#!/bin/sh
# Consider all files obsolete
#autoreconf --force --install -I .macro_dir

# Keep existing files - that doesn't overwrite ltmain.sh!
autoreconf --install -I .macro_dir
