# CS3413-DiskRead2
A c program that simulates various disk reading algorithms.  The program can use first come first server, shortest seek time first, scan, c-scan, look, and c-look.  The user specifies which algorithm they want to use.  This program assumes that the disk layout is using a linked list, and will traverse the linked list to retrieve all data files based on the file requested, then return the memory locations that the file was retrieved from.