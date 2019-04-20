CXXFLAGS+=-std=c++17 -O3
CXX=g++

fgrammar=grammar/grammar.hpp grammar/grammar.cpp
fhash=parser/expression_hash.hpp parser/expression_hash.cpp
fparser=parser/parser.hpp parser/parser.cpp
fproof=proof/proof_utils.hpp proof/proof_utils.cpp

pack: $(fgrammar) $(fhash) $(fparser) $(fproof) minimize.cpp parse.cpp
	zip minimize.zip -r .

out:
	mkdir -p executable

clean:
	rm -rf executable
	rm -f minimize.zip

out/tparse: $(fgrammar) $(fhash) $(fparser) parse.cpp
	$(CXX) $^ $(CXXFLAGS) -o executable/parse

out/tminimize: $(fgrammar) $(fhash) $(fparser) $(fproof) minimize.cpp
	$(CXX) $^ $(CXXFLAGS) -o executable/minimize

all: out out/tparse out/tminimize

run:
	./executable/minimize