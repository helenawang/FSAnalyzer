../bin/main: main.o
	g++ -o ../bin/main main.o get_bytes.o type_judge.o

main.o: main.c get_bytes.o type_judge.o includes.h
	g++ -c main.c get_bytes.o type_judge.o includes.h

get_bytes.o: get_bytes.c includes.h
	g++ -c get_bytes.c includes.h

type_judge.o: type_judge.c includes.h
	g++ -c type_judge.c includes.h
