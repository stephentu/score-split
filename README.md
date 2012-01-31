score-split
===========
This is a simple unix utility, similar to a subset of functionality in `split(1)`.

The idea is quite simple- you have a text file, and you want to split it (by lines)
into some N files. Simple problem, which you would usually use `split` to solve.
However, what if you do not want an even split, but want to split based on some
score/ranking per file. This is what `score-split` accomplishes.

Synopsis
--------

    [USAGE] score-split (-f|--filename) filename (-s|--scores) score0 score1 ... scoreN 

Compiling
---------

    $ make   

Ranking 
-------
The number of lines which go into each file is determined as follows.  All N
scores are summed up (you must provide at least one score, and all scores must
be positive).  Each i-th file gets `scoreI / sum(score0, ..., scoreN)` lines,
with the result rounded to an integer using the usual rules of integer
rounding. 

If a particular file's line count is zero, an empty file is created.

The filenames are created in the same directory, with the naming convention
`filename_NNN`, where `NNN` is the position (0-based index) of the score which
resulted in that file.

For example, suppose `foo.txt` has 8 lines in it. Then the following invocation:

    score-split -f foo.txt -s 1 1

Produces two files `foo.txt_0` and `foo.txt_1`, each with 4 lines of text.
