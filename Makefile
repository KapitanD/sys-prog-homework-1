TESTS_DIR=test
BIN_DIR=bin
SRC_DIR=src/solution

build: ${SRC_DIR}/sort.c ${SRC_DIR}/vector.c
	gcc ${SRC_DIR}/sort.c ${SRC_DIR}/vector.c -o ${BIN_DIR}/sort

run: build
	./${BIN_DIR}/sort ${TESTS_DIR}/out.txt ${TESTS_DIR}/test1.txt 

build_debug: ${SRC_DIR}/sort.c ${SRC_DIR}/vector.c
	gcc -g ${SRC_DIR}/sort.c ${SRC_DIR}/vector.c -o ${BIN_DIR}/sort_debug

debug: build_debug
	gdb --args ${BIN_DIR}/sort_debug ${TESTS_DIR}/out.txt ${TESTS_DIR}/test1.txt

test: run
	./${TESTS_DIR}/checker.py -f ${TESTS_DIR}/out.txt