.SUFFIXES: .c .o

CC = gcc
CCFLAGS = -g -std=c99 -Wall -pedantic 
EXEC = main
OBJS = main.o gameOfLifeOutput.o functions.o 
ARGS =20 glider-10x10 0 0
RM_FILES = main.o gameOfLifeOutput.o functions.o leaksLog *_* valgrind-report.txt
FINAL_RM = main.o gameOfLifeOutput.o functions.o 

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -o ${EXEC} ${OBJS} -lm 

.c.o:
	${CC} ${CCFLAGS} -c $<

run: ${EXEC}
	./${EXEC}

clean:
	rm -f ${EXEC} ${FINAL_RM} 

leaks: $(EXEC)
	MallocStackLogging=1 leaks --atExit -- ./$(EXEC)

leaksLog: $(EXEC)
	MallocStackLogging=1 leaks --atExit -- \
	./$(EXEC) > leaksLog 2>&1

valgrind: ${EXEC}
	valgrind --leak-check=full \
			--show-leak-kinds=all \
			--track-origins=yes \
			--verbose \
			--log-file=valgrind-report.txt \
			./${EXEC} ${ARGS}


main.o: main.c gameOfLifeOutput.h functions.h
gameOfLifeOutput.o: gameOfLifeOutput.c gameOfLifeOutput.h
functions.o: functions.c functions.h