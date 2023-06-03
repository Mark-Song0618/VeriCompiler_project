# Verific to Netlist Compiler

## This is a prototype of "verific to netlist" compiler.For now it only support very little grammer and primitivies.

* It use regexp for lexical analysis and rdp for syntax analysis.
* It construct an ast, and then resolve the identifiers, build netlists, dump the results.This is achieved by visitor pattern.
* CmdLine and qt-gui are both supported.Uses can open a verific design to compile or editor in gui.
