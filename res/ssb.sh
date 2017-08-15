#!/bin/sh
bin/dump-sql-text -i -g console < ssb.sql > ssb.asni
bin/dump-sql-text -i -g plaintext < ssb.sql > ssb.txt
bin/dump-sql-text -i -g latex < ssb.sql > ssb.tex
lualatex ssb.tex
