TAR := test.exe

PARENT_DIR := ../../../../
TARGET_DIR := ${PARENT_DIR}target/
OBJ := ${TARGET_DIR}syntax.o ${TARGET_DIR}regular_expression.o 
LIB := ${TARGET_DIR}hashmap.o ${TARGET_DIR}queue.o ${TARGET_DIR}stack.o ${TARGET_DIR}tool.o
CONTAINER_DIR := ${PARENT_DIR}/tool/container


CC := gcc -g 

${TAR} : ${OBJ} ${CONTAINER} ${LIB}  
	${CC} $^ -o $@ -I ${TARGET_DIR}

${TARGET_DIR}regular_expression.o : regular_expression.c
	${CC} -c $< -o $@ -I ${CONTAINER_DIR} -I ../../ -I ${PARENT_DIR}/tool

${TARGET_DIR}syntax.o : ../../syntax.c
	cd ../../&&make
	
${LIB}: 
	cd ${CONTAINER_DIR} && make

clean :
	rm -rf ${TARGET_DIR}*.o
	rm -rf ./*.exe

