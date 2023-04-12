TAR := test.exe

TARGET_DIR := ../../target/
OBJ := ${TARGET_DIR}syntax.o ${TARGET_DIR}test.o 
LIB := ${TARGET_DIR}hashmap.o ${TARGET_DIR}queue.o ${TARGET_DIR}stack.o ${TARGET_DIR}tool.o
CONTAINERDIR := ../container
PARENTDIR := ../

CC := gcc -g 

${TAR} : ${OBJ} ${CONTAINER} ${LIB}  
	${CC} $^ -o $@ -I ${TARGET_DIR}

${TARGET_DIR}%.o : %.c
	${CC} -c $< -o $@ -I ${CONTAINERDIR} -I ${PARENTDIR}

${LIB}: 
	cd ${CONTAINERDIR} && make

clean :
	rm -rf ${TARGET_DIR}*.o
	rm -rf ./*.exe

