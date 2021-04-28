# DBManagementSystem
This project is the semester-long project required for CSCI-421 @ Rochester Institute of Technology.<br/> 
The primary objective of this project is to implement a Database Management System.

# Known Problems
The implementation of "where" clauses in our solution has some known problems that may not 
completely fulfill the requirements for Phase 3. One known problem is that when two tables
that have an attribute with an identical name, you may only reference that attribute if you
project it in the "select" clause. For example:
> select foo.baz, bar.baz from foo, bar where foo.baz = bar.baz;

works, where:
> select bar.baz from foo, bar where foo.baz = bar.baz;

will not.

Attributes that do not exist in tables may not be recognized as such in the "where" clause.

# Compilation
To create the executable move to the DBManagementSystem directory and enter on commandline: 
gcc -std=gnu99 src/drivers/database.c src/c/*.c -o database

# Run Program
Once the executable is created enter:
usage: ./database <db_loc> <page_size> <buffer_size>

## Example Run:
./database ~/Desktop/db/ 4096 10

# Project Details 
The project will be broken into 4 phases, and a group evaluation. Each phase will build
on the prior phase.<br />
A basic description of each phase is outlined here:<br />
&nbsp;- [Phase1](#phase1)<br />
&nbsp;- [Phase2](#phase2)<br />
&nbsp;- [Phase3](#phase3)<br />
&nbsp;- [Phase4](#phase4)<br />

# <a id="phase1"></a>Phase1
During this phase you will implement a basic storage manager. Details can be found in the
myCourses content section for the project in a pdf named phase1.pdf.
Your solution must be zipped, named phase1.zip, and placed in the Phase1 assignment box
in myCourses. No late or emailed submissions will be accepted. Only one group member is
required to submit. Only the last submission is graded.

# <a id="phase2"></a>Phase2
During this phase you will implement a basic DDL parser. Details can be found in the
myCourses content section for the project in a pdf named phase2.pdf.
Your solution must be zipped, named phase2.zip, and placed in the Phase2 assignment box
in myCourses. No late or emailed submissions will be accepted. Only one group member is
required to submit. Only the last submission is graded.

# <a id="phase3"></a>Phase3
During this phase you will implement a DML parser. Details can be found in the myCourses
content section for the project in a pdf named phase3.pdf.
Your solution must be zipped, named phase3.zip, and placed in the Phase3 assignment box
in myCourses. No late or emailed submissions will be accepted. Only one group member is
required to submit. Only the last submission is graded.

# <a id="phase4"></a>Phase4
During this phase you will implement two types of indexing: hash-based and B+Trees. Details
can be found in the myCourses content section for the project in a pdf named phase4.pdf.
Your solution must be zipped, named phase5.zip, and placed in the Phase5 assignment box
in myCourses. No late or emailed submissions will be accepted. Only one group member is
required to submit. Only the last submission is graded.

# Authors
Kyle Collins<br/>
Geoffrey Moss<br/>
Samuel Tillinghast<br/>
Samuel Tregea<br/>
