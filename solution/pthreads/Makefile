sources = solution_pthreads.c solution_join.c solution_barrier.c ../common/io.c ../common/iteration.c
lib = -lpthread
tester = ../common/tester.sh
runner = python3 ../common/runner.py
test_data = ../common/test.inputs.txt

out/join.o:
	gcc -o out/join.o ${lib} -DJOIN ${sources} -lm

out/speedtest.join.o:
	gcc -o out/speedtest.join.o ${lib} -DJOIN -DSPEED_TEST=1 ${sources} -lm

out/barrier.o:
	gcc -o out/barrier.o ${lib} ${sources} -lm

out/speedtest.barrier.o:
	gcc -o out/speedtest.barrier.o ${lib} -DSPEED_TEST=1 ${sources} -lm

test_join: out/join.o
	cat ${test_data} | ./out/join.o | ${tester}

speed_join: out/speedtest.join.o
	${runner} ./out/speedtest.join.o {NP} > res/join.csv

run_join: out/join.o
	./join.o ${NP}

test_barrier: out/barrier.o
	cat ${test_data} | ./out/barrier.o | ${tester}

speed_barrier: out/speedtest.barrier.o
	${runner} ./out/speedtest.barrier.o {NP} > res/barrier.csv

run_barrier: out/barrier.o
	./out/barrier.o ${NP}

rebuild: clean out/join.o out/barrier.o out/speedtest.join.o out/speedtest.barrier.o

clean:
	rm -f out/*.o