
compiling the program on linux environment:
	gcc assn_2.c assn_2

Running the compiled program
	./assn2 strategy filename

(where, strategy can be one of --first-fit, --best-fit, --worst-fit and file should be a db file.)

This code works fine on windows and linux but with one exception. It some garbage values which ultimately rigs the positions of hole count WHEN I USE FOLLOWING command to execute it
./assn2 strategy filename < input.txt > output.txt

However, if I use the following command line then I get expected output.
./assn2 strategy filename > output1.txt
(in this case I copy hole input.txt file and paste it on the command line once the program starts execution)

I hope this would be taken into consideration for grading.
 