CXXFLAGS+=-std=c++17
CXX=g++

fgrammar=grammar/grammar.hpp grammar/grammar.cpp
fhash=parser/expression_hash.hpp parser/expression_hash.cpp
fparser=parser/parser.hpp parser/parser.cpp
fproof=proof/proof_utils.hpp proof/proof_utils.cpp

pack: $(fgrammar) $(fhash) $(fparser) $(fproof) minimize.cpp parse.cpp
	zip all.zip -r .

out:
	mkdir -p executable

clean:
	rm -rf executable
	rm -f all.zip

out/tparse: $(fgrammar) $(fhash) $(fparser) parse.cpp
	$(CXX) $^ $(CXXFLAGS) -o executable/parse

out/tminimize: $(fgrammar) $(fhash) $(fparser) $(fproof) minimize.cpp
	$(CXX) $^ $(CXXFLAGS) -o executable/minimize

out/tintuitionistic: $(fgrammar) $(fhash) $(fparser) $(fproof) proof/proof_translator.hpp intuitionistic.cpp
	$(CXX) $^ $(CXXFLAGS) -o executable/intuitionistic

out/tprove: $(fgrammar) $(fhash) $(fparser) $(fproof) proof/proof_translator.hpp proof/proof_creator.hpp prove.cpp
	$(CXX) $^ $(CXXFLAGS) -o executable/prove

all: out out/tprove

run:
	./executable/prove