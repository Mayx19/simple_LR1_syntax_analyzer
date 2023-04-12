CC := gcc -g
OBJ := queue hashmap stack datrie
TOOL := tool
PARENT_DIR :=../
TARGET_DIR := ../../target/

ALL:${TARGET_DIR}queue.o ${TARGET_DIR}hashmap.o ${TARGET_DIR}stack.o ${TARGET_DIR}datrie.o

${TARGET_DIR}%.o : %.c %.h ../../target/tool.o
	${CC} -c $< -o $@ -I ${TARGET_DIR} -I ${PARENT_DIR}

../../target/tool.o : ${PARENT_DIR}tool.c ${PARENT_DIR}tool.h
	${CC} -c $< -o $@ -I ${TARGET_DIR} 

clean:
	rm -rf ${TARGET_DIR}*.o
