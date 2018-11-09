LLVM_SRC_PATH := /home/mwasfy/Desktop/llvm
LLVM_BUILD_PATH := /home/mwasfy/Desktop/build
LLVM_BIN_PATH 	:= $(LLVM_BUILD_PATH)/bin

$(info -----------------------------------------------)
$(info Using LLVM_SRC_PATH = $(LLVM_SRC_PATH))
$(info Using LLVM_BUILD_PATH = $(LLVM_BUILD_PATH))
$(info Using LLVM_BIN_PATH = $(LLVM_BIN_PATH))
$(info -----------------------------------------------)

CXX := g++
CXXFLAGS := -fno-rtti -O0 -g
PLUGIN_CXXFLAGS := -fpic

LLVM_CXXFLAGS := `$(LLVM_BIN_PATH)/llvm-config --cxxflags`
LLVM_LDFLAGS := `$(LLVM_BIN_PATH)/llvm-config --ldflags --libs --system-libs`


LLVM_LDFLAGS_NOLIBS := `$(LLVM_BIN_PATH)/llvm-config --ldflags`
PLUGIN_LDFLAGS := -shared

CLANG_INCLUDES := \
	-I$(LLVM_SRC_PATH)/tools/clang/include \
	-I$(LLVM_BUILD_PATH)/tools/clang/include

CLANG_LIBS := \
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


SRC_CLANG_DIR := src_clang
BUILDDIR := build

.PHONY: all
all: make_builddir \
	$(BUILDDIR)/rewritersample 



.PHONY: make_builddir
make_builddir:
	@test -d $(BUILDDIR) || mkdir $(BUILDDIR)


$(BUILDDIR)/rewritersample: $(SRC_CLANG_DIR)/rewritersample.cpp
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) $(CLANG_INCLUDES) $^ \
		$(CLANG_LIBS) $(LLVM_LDFLAGS) -o $@


.PHONY: clean format

clean:
	rm -rf $(BUILDDIR)/* *.dot 

