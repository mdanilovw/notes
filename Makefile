IDIR =./include
BOOST_IDIR = ./lib/boost/boost_1_63_0
CRYPTOPP_IDIR =./lib/cryptopp
SRC_DIR=./src

CC=g++

BOOST_LIB_DIR=./lib/boost/linux/x64/stage/lib
BOOST_DATE_TIME_LIB = $(BOOST_LIB_DIR)/libboost_date_time.a
BOOST_SERIALIZATION_LIB = $(BOOST_LIB_DIR)/libboost_serialization.a

CRYPTOPP_LIB =./lib/cryptopp/linux/x64/libcryptopp.a

LINK_LIBS=$(BOOST_DATE_TIME_LIB) $(BOOST_SERIALIZATION_LIB) $(CRYPTOPP_LIB) -lpthread

CXXFLAGS=-I$(IDIR) -I$(BOOST_IDIR) -I$(CRYPTOPP_IDIR) -std=c++11 -g

ODIR=./build

_DEPS = cli.hpp core.hpp core_service.hpp crypto.hpp util.hpp record.hpp return_code.hpp core_action.hpp response.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = cli.o core.o core_service.o crypto.o main.o record.o core_action.o response.o util.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CXXFLAGS)

notes: $(OBJ)
	$(CC) -o $(ODIR)/$@ $^ $(LINK_LIBS)

.PHONY: clean

clean:
	rm $(ODIR)/*
