# In-memory indexing with availability list

### by Harshit Patel(@hpatel24)

This program will build and maintain a simple file of student records. Each record will have four fields: SID (student ID), last (last name), first, (first name) and major (program of study). Fields within a record will be variable-length, and will be delimited by the | character. For example

		712412913|Ford|Rob|Phi

represents a student with an SID of 712412913, a last of Ford, a first of Rob, and a major of Phi (Rob Ford is minoring in Ethics).

SID is the primary key for a student record. This means every individual student record will have a unique SID.

Records will be variable-length, and will be stored one after another in a binary data file. Each record will be preceded by an integer that defines the size of its corresponding record.

compiling the program on linux environment:

	gcc assn_2.c assn_2

Running the compiled program

	./assn2 strategy filename

(where, strategy can be one of --first-fit, --best-fit, --worst-fit and file should be a db file.)

This code works fine on windows and linux but with one exception. It some garbage values which ultimately rigs the positions of hole count WHEN I USE FOLLOWING command to execute it

	./assn2 strategy filename < input.txt > output.txt
