#####################################
# Build the C-to-Verilog translator #
#####################################

LLVM_CXXFLAGS = $(shell llvm-config --cxxflags)
LLVM_LDFLAGS = $(shell llvm-config --ldflags --libs --system-libs)

CLANG_LIBS = \
	-Wl,--start-group \
	-lclangAST \
	-lclangASTMatchers \
	-lclangAnalysis \
	-lclangBasic \
	-lclangDriver \
	-lclangEdit \
	-lclangFrontend \
	-lclangFrontendTool \
	-lclangLex \
	-lclangParse \
	-lclangSema \
	-lclangEdit \
	-lclangRewrite \
	-lclangRewriteFrontend \
	-lclangStaticAnalyzerFrontend \
	-lclangStaticAnalyzerCheckers \
	-lclangStaticAnalyzerCore \
	-lclangCrossTU \
	-lclangIndex \
	-lclangSerialization \
	-lclangToolingCore \
	-lclangTooling \
	-lclangFormat \
	-Wl,--end-group

SRCDIR = src
BUILDDIR = build

.PHONY: all

all: $(BUILDDIR)/rewritersample 

.PHONY: make_builddir

make_builddir:
	@test -d $(BUILDDIR) || mkdir $(BUILDDIR)

$(BUILDDIR)/rewritersample: make_builddir $(SRCDIR)/rewritersample.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) \
	  $(SRCDIR)/rewritersample.cpp \
	  $(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILDDIR)
