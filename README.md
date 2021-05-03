# KeyValue-Server
Kritik Patel: ksp127
Manav Kumar: mk1745

*Command codes (SET, GET, DEL) should be capital
We tested our program using the "nc" command, as well as the client send.c program given in class. We
used Valgrind to check for proper memory allocation and freeing. Some testing cases we used:
- Correct usages of SET, GET, and DEL
- Improper lengths of requests to get proper LEN errors
- Malformed messages to get the proper BAD errors
- SET keys and DEL same keys to make sure deletion worked
- Connecting multiple clients to the server and doing operations