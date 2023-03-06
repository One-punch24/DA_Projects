## Make Cpp Great Again

Repo for EPFL CS-451 Distributed Algorithms.

* proj1 branch: Perfect Link.
* proj2 branch: FIFO Broadcast. In my submission, I didn't deal with the confilict problem (I forgot to add those locks in my submission), but it still performed well in my simulation and also achieved a reasonable grade (5.61/6) in the submission system. Adding those locks is safer : ).
* master branch: Lattice Agreement. Without the naive lock in my code, the running results are wrong and sometimes the program gets stuck. Thus, the lock is required.
