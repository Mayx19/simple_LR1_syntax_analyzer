CC := gcc -g
TAR := test.exe
PARENT_DIR :=../
TARGET_DIR := ../../../target/
OBJ :=   ${TARGET_DIR}test.o 
LIB := ${TARGET_DIR}hashmap.o ${TARGET_DIR}tool.o ${TARGET_DIR}datrie.o
TOOL := tool



${TAR} : ${OBJ} ${LIB}  
	${CC} $^ -o $@ -I ${TARGET_DIR}

${TARGET_DIR}test.o : datrie_test.c 
	${CC} -c $< -o $@

${LIB}: ../datrie.c
	cd ${PARENT_DIR} && make

clean :
	rm -rf ${TARGET_DIR}*.o
	rm -rf ./*.exe