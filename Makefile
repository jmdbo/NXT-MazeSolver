SRC=src
BIN=bin
SOURCE=maze
INCLUDE=src
TARGET=maze
TEST=tst
TESTSOURCE=testlib
TESTTARGET=testlib

.phony: all test clean

all:
	nbc -Z2 ${SRC}/${SOURCE}.nxc -I=${INCLUDE} -O=${BIN}/${TARGET}.rxe
	nxtcom ${BIN}/${TARGET}.rxe 

test:
	nbc -Z2 ${TEST}/${TESTSOURCE}.nxc -I=${INCLUDE} -O=${BIN}/${TESTTARGET}.rxe
	nxtcom ${BIN}/${TESTTARGET}.rxe 

clean:
	rm ${BIN}/*