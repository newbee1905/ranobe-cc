#!/bin/sh

find src -iname '*.cc' | xargs clang-format -i
find include -iname '*.hh' -o -name '*.tt' | xargs clang-format -i
