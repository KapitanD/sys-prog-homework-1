TESTS_DIR=test
FILES_DIR=test/files
BIN_DIR=bin
SRC_DIR=src/solution

build: ${SRC_DIR}/main_coroutine.c ${SRC_DIR}/vector.c
	gcc ${SRC_DIR}/main_coroutine.c ${SRC_DIR}/vector.c -o ${BIN_DIR}/sort

run: build
	./${BIN_DIR}/sort ${FILES_DIR}/test1.txt ${FILES_DIR}/test2.txt ${FILES_DIR}/test3.txt ${FILES_DIR}/test4.txt ${FILES_DIR}/test5.txt ${FILES_DIR}/test6.txt

build_debug: ${SRC_DIR}/main_coroutine.c ${SRC_DIR}/vector.c
	gcc -g ${SRC_DIR}/main_coroutine.c ${SRC_DIR}/vector.c -o ${BIN_DIR}/sort_debug

debug: build_debug
	gdb --args ${BIN_DIR}/sort_debug ${FILES_DIR}/test_debug1.txt ${FILES_DIR}/test_debug2.txt ${FILES_DIR}/test_debug3.txt

test: run
	./${TESTS_DIR}/checker.py -f ${FILES_DIR}/out.txt