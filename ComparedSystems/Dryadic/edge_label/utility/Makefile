CXX:=icpc
#CPPFLAGS:=-Wall -Werror=return-type -Wshadow -O0 -march=native -std=c++17 -g -traceback -debug all -D_GLIBCXX_DEBUG
CPPFLAGS:=-Wall -Werror=return-type -Wshadow -O3 -march=native -std=c++17 -qopenmp

all: dist stats

dist: degree_distribution.cpp
	$(CXX) $(CPPFLAGS) $< -o $@

stats: set_statistics.cpp
	$(CXX) $(CPPFLAGS) $< -o $@

clean:
	-@rm -f dist
