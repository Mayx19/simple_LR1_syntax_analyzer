
TARGET_DIR := ../../target/
LIB := ${TARGET_DIR}hashmap.o ${TARGET_DIR}queue.o ${TARGET_DIR}stack.o ${TARGET_DIR}tool.o
CONTAINERDIR := ../container
PARENTDIR := ../

CC := gcc -g 


${TARGET_DIR}%.o : %.c
	cd ${CC} -c $< -o $@ -I ${CONTAINERDIR} -I ${PARENTDIR}

${LIB}: 
	cd ${CONTAINERDIR} && make

clean :
	rm -rf ${TARGET_DIR}*.o
	rm -rf ./*.exe

