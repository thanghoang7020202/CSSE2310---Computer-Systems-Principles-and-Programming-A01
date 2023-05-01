#!/bin/bash
# Usage: run_uqwordiply_with_valgrind.sh [-timeout secs] input-file \
#	[-dict dictionary-file] [-word word] [args] 
# Runs uqwordiply with the given word as the starter word (if supplied) with 
# stdin redirected from the given input-file (one guess per line).
# The given dictionary file will be used as the source of a named pipe 
# added to the uqwordiply arguments as a --dictionary argument.
# Additional args are given as command line arguments to uqwordiply.

if [ "$1" = "-timeout" -a -n "$2" ] ; then
    timeout=$2
    shift 2
else
    timeout=100
fi

if [ "$#" -lt 1 ] ; then
    echo "Insufficient arguments to $0" >&2
    exit 1
fi
if [ ! -r "$1" ] ; then
    echo "Can't read guesses file \"$1\"" >&2
    exit 2
fi
guesses="$1"
shift 1

if [ "$1" = "-dict" -a -r "$2" ] ; then
    dictionary="$2"
    shift 2;
    # Set up named pipe for our dictionary
    mkfifo /tmp/$$.dictionary
    dictionary_args="--dictionary /tmp/$$.dictionary"
else 
    dictionary=""
    dictionary_args=""
fi
if [ "$1" = "-word" -a -n "$2" ] ; then
    export WORD2310="$2"
    shift 2;
fi

# Remove temporary file
rm -f /tmp/valgrind.$$.out

# After a few seconds, send the contents of the required dictionary to the
# named pipe with a background process
if [ "$dictionary_args" ] ; then 
    (sleep 3; cat "${dictionary}") > /tmp/$$.dictionary 2>/dev/null &
    bg_pid=$!
fi

# Run uqwordiply within valgrind - with a timeout
timeout "$timeout" valgrind --log-file=/tmp/valgrind.$$.out ${uqwordiply:=./uqwordiply} ${dictionary_args} "$@" \
	< ${guesses}
status=$?

# Kill off the background process
if [ "$dictionary_args" ] ; then 
    kill $bg_pid >&/dev/null
    wait $bg_pid >&/dev/null
fi

# Check for memory leaks in the valgrind log
msg="All heap blocks were freed"
if grep "$msg" /tmp/valgrind.$$.out &>/dev/null ; then
    echo "$msg" >&2
else
    echo "Memory leak found" >&2
fi

# Remove valgrind output file and named fifo
rm -f /tmp/valgrind.$$.out /tmp/$$.dictionary

exit $status
