# MACA 
Maca is a simple tool that is intended to provide most of the information you require to start a reverse engineering project in one go. 

Current version has the following features:

* Provides ELF details: Header, Sections, Program headers and Dynamic Section
* Dumps strings (use --strings Minumum_size or -s Minimum=_size). Shows offset in the file as well as the section it belongs to, in case sections are available
* Tries to identify the language used to write the program (C/Asm, C++, Rust, Go) **Experimental**

Just a simple tool to explore ELF binaries and give a little help on reverse challenges
