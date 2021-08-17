ddmon: ddmon.c
	gcc -shared -fPIC -o ddmon.so ddmon.c -ldl
	gcc -o ddchck ddchck.c
	gcc -pthread -g  -o abba abba.c
	gcc -pthread -g  -o eungang eungang.c
	gcc -pthread -g  -o falsepos1 falsepos1.c
	gcc -pthread -g  -o falsepos2 falsepos2.c
	gcc -pthread -g  -o falsepos3 falsepos3.c
	gcc -pthread -g  -o lock7 lock7.c
	gcc -pthread -g  -o m123 m123.c
	gcc -pthread -g  -o self self.c
	gcc -pthread -g  -o truepos1 truepos1.c
	gcc -pthread -g  -o truepos2 truepos2.c
	gcc -pthread -g  -o yeahn yeahn.c
	gcc -pthread -g  -o test9 test9.c
	gcc -pthread -g  -o test10 test10.c
	gcc -pthread -g  -o test11 test11.c
	echo LD_PRELOAD="./ddmon.so" ./[target_file]

