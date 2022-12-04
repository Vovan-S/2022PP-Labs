sources = solution_mpi.c ../common/io.c ../common/iteration.c
lib = -lm
tester = ../common/tester.sh 
test_data = ../common/test.inputs.txt
runner = python3 ../common/runner.py

out/solution.o:
	mpicc -o out/solution.o ${sources} ${lib} 

run: out/solution.o
	mpiexec -np $(NP) ./out/solution.o

rebuild: clean out/solution.o out/speedtest.o

test: out/solution.o
	cat ${test_data} | mpirun -np 8 ./out/solution.o | ${tester} 

out/speedtest.o:
	mpicc -o out/speedtest.o -D SPEED_TEST=1 ${sources} ${lib}

speedtest: out/speedtest.o
	${runner} mpirun -np {NP} ./out/speedtest.o > res/result.csv

clean:
	rm -f out/*.o