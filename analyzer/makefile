
PARENT_DIR := ../
TARGET_DIR := ${PARENT_DIR}target/
LIB := ${TARGET_DIR}hashmap.o ${TARGET_DIR}queue.o ${TARGET_DIR}stack.o ${TARGET_DIR}tool.o
CONTAINERDIR := ${PARENT_DIR}container

CC := gcc -g 


${TARGET_DIR}syntax.o : syntax.c
	${CC} -c $< -o $@ -I ${CONTAINERDIR} -I ${PARENT_DIR}

${LIB}: 
	cd ${CONTAINERDIR} && make

clean :
	rm -rf ${TARGET_DIR}*.o
	rm -rf ./*.exe

