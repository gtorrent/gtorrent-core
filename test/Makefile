BIN=gt-test
LIBS=../build/src/libgtorrent.a googletest/build/libgtest.a googletest/build/libgtest_main.a
CFLAGS=$(shell pkg-config --cflags libtorrent-rasterbar) -Igoogletest -Igoogletest/include -I../include -std=c++11
LDFLAGS=$(shell pkg-config --libs libtorrent-rasterbar) -pthread -o $(BIN)

all:
	$(CXX) $(CFLAGS) *.cpp $(LIBS) $(LDFLAGS)

clean:
	rm gt-test
