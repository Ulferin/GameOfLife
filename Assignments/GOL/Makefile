CXX = g++ -std=c++17
CXXFLAGS  	= -g

LDFLAGS_1 = -pthread
LDFLAGS_2 = -fopenmp
OPTFLAGS = -O3 #-DDEBUG

TARGETS = gol-seq.o gol-par.o gol-omp.o

.PHONY = clean all test-seq test-par test-omp
.SUFFIXES = .cpp

all: $(TARGETS)

gol-omp.o: gol-omp.cpp
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -o $@ $< $(LDFLAGS_2)

gol-par.o: gol-par.cpp
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -o $@ $< $(LDFLAGS_1)

gol-seq.o: gol-seq.cpp
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

test-seq:
	./test.sh seq $(ARGS)

test-par:
	./test.sh par $(ARGS)

test-omp:
	./test.sh omp $(ARGS)
