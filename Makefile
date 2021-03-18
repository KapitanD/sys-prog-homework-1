TESTS_DIR=test
FILES_DIR=test/files
BIN_DIR=bin
SRC_DIR=src/solution

build: ${SRC_DIR}/sort.c ${SRC_DIR}/vector.c
	gcc ${SRC_DIR}/sort.c ${SRC_DIR}/vector.c -o ${BIN_DIR}/sort

run: build
	./${BIN_DIR}/sort ${FILES_DIR}/out.txt ${FILES_DIR}/test1.txt 

build_debug: ${SRC_DIR}/sort.c ${SRC_DIR}/vector.c
	gcc -g ${SRC_DIR}/sort.c ${SRC_DIR}/vector.c -o ${BIN_DIR}/sort_debug

debug: build_debug
	gdb --args ${BIN_DIR}/sort_debug ${FILES_DIR}/out_debug.txt ${FILES_DIR}/test_debug.txt

test: run
	./${TESTS_DIR}/checker.py -f ${FILES_DIR}/out.txt