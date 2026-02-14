# Deadlocks Demonstration

The purpose of this repository is to demonstrate how a deadlock can occur
between two threads. While the actions causing the deadlock are meaningless
(locking and then unlocking a thread, with no action in between), it still shows
how dependant resources need to be carefully managed to avoid circular
dependencies.

Made for my university's Operating System Fundamentals class.

## Compiling

Use your C compiler of choice to compile and then run the `deadlocks.c` file.
The line present in the `Makefile` was tested on Arch and verified to work.

> [!CAUTION]
> Do not compile this program with the `-O3` flag. This causes the program to
> stall after joining Thread A. Instead, it is recommended to use `-Os` and
> compile for size due to the `sleep(1)` calls in `deadlocks.c`.
