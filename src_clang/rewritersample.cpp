#include <cstdio>
#include <memory>
#include <sstream>
#include <string>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/Rewriters.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
	MyASTVisitor(Rewriter &R, CompilerInstance *CI) : TheRewriter(R), astContext(&(CI->getASTContext())) {}


//#### check if the variable declaration is preceeded by a for stmt. If yes then genvar will take car of the variable declaration
bool checkfor(const Stmt *D)
{
        //get parents
        const auto& parents = astContext->getParents(*D);
        if ( parents.empty() ) {
            llvm::errs() << "Can not find parent\n";
            return false;
        }
        //llvm::errs() << "find parent size=" << parents.size() << "\n";
        const Stmt *ST = parents[0].get<Stmt>();
        if (isa<ForStmt>(ST))
	{
		//llvm::errs() << "declaration in for loop " << "\n";
		return true;
	}
	else
		return false;
}

//#### InstrumentStmt - Add braces to line of code
void InstrumentStmt(Stmt *s)
{
	//instrument if or else body start
	SourceLocation ST = s->getLocStart();
	int offset1 = Lexer::MeasureTokenLength(ST, TheRewriter.getSourceMgr(), TheRewriter.getLangOpts());
	SourceLocation END = s->getLocEnd();
	int offset2 = Lexer::MeasureTokenLength(END, TheRewriter.getSourceMgr(), TheRewriter.getLangOpts());
	if(isa<CompoundStmt>(s))
	{
		TheRewriter.RemoveText(ST,offset1);
		TheRewriter.InsertText(ST, "begin", true, true);
		TheRewriter.RemoveText(END,offset2);
		TheRewriter.InsertText(END, "end", true, true);
	}
	else
	{
		TheRewriter.InsertText(ST, "begin\n", true, true);
		END = END.getLocWithOffset(offset2+1);
		TheRewriter.InsertText(END, "\nend", true, true);
	}
	
}

//#### InstrumentFor - Add the for loop generator code
void InstrumentFor(Stmt *s, ForStmt *f)
{
	//get for body starting location
	SourceLocation ST = s->getLocStart();
	int offset1 = Lexer::MeasureTokenLength(ST, TheRewriter.getSourceMgr(), TheRewriter.getLangOpts());
	//instrument body start
	TheRewriter.RemoveText(ST,offset1);
	TheRewriter.InsertText(ST, "begin", true, true);
	//get for body end location
	SourceLocation END = s->getLocEnd();
	//int offset = Lexer::MeasureTokenLength(END, TheRewriter.getSourceMgr(), TheRewriter.getLangOpts()) + 1;
	int offset2 = Lexer::MeasureTokenLength(END, TheRewriter.getSourceMgr(), TheRewriter.getLangOpts());
	//instrument body end
	//SourceLocation END1 = END.getLocWithOffset(offset);
	TheRewriter.RemoveText(END,offset2);
	//TheRewriter.InsertText(END, "end \nendgenerate\n", true, true);
 	TheRewriter.InsertText(END, "end", true, true);

	//start the genvar attribute
	/*char gen[256];
	std::string var;
	const DeclStmt *DS = dyn_cast<DeclStmt>(f->getInit());

	for (auto *DI : DS->decls()) 
	{
		VarDecl *VD = dyn_cast<VarDecl>(DI);
		if (VD)
		{
			//llvm::errs() << "varrrrrrrr  " << VD->getDeclName().getAsString() << "\n";
			var = VD->getDeclName().getAsString();
		}
	}	
	
	sprintf(gen,"genvar %s;\ngenerate\n",var.c_str());

	SourceLocation FT = f->getLocStart();
	TheRewriter.InsertText(FT, gen,true,true);*/

}

//#### check all children of the if stmt to look for variables to add to the always @ block
void checkif(Stmt *If)
{
	for (auto *DI : If->children())
	{
		//Stmt *st = dyn_cast<Stmt>(DI);
		if(DI!=NULL){
			if(isa<DeclRefExpr>(DI))
			{
				DeclRefExpr *DRE = dyn_cast<DeclRefExpr>(DI);
				//llvm::errs() << "if statment variable  " << DRE->getNameInfo().getName().getAsString() <<"\n";
				if(!DRE->getType()->isArrayType())
				{
					add_always(DRE->getNameInfo().getName().getAsString());
				}
			}
			checkif(DI);
		}
	}
}

//#### if variable not present in the list of parameters of the always @ add it to always_params array
void add_always(std::string strin)
{
	bool found = false;
	for (int i=0;i<counter;i++)
	{
		if(strin == always_params[i])
		{
			found = true;
			break;
		}
	}
	if(!found)
	{
		always_params[counter] = strin;
		counter++;
	}
}

//#### return the constructed always @ statement and reset always_params array
std::string get_always()
{
	std::string temp;
	temp.insert(0,"always @(");
	for(int i=0;i<counter;i++)
	{
		temp.insert(temp.length(),always_params[i]);
		if(i!=counter-1)
		{
			temp.insert(temp.length()," or ");
		}
	}
	temp.insert(temp.length(),")\nbegin\n");
	counter = 0;
	//llvm::errs() << "always block in string will be \n" << temp << "\n";
	return temp;
}
bool inalways(const Stmt *D)
{
	bool check = false;
        //get parents
        const auto& parents = astContext->getParents(*D);
        if ( parents.empty() ) {
            llvm::errs() << "Can not find parent\n";
            return false;
        }
        //llvm::errs() << "find parent size=" << parents.size() << "\n";
        const Stmt *ST = parents[0].get<Stmt>();
	if(ST!=NULL)
	{
		if (isa<IfStmt>(ST))
		{
			//llvm::errs() << "declaration in for loop " << "\n";
			check = true;
		}
		else
		{
			check = inalways(ST);
		}
	}
	return check;
}

//#### Override Binary Operator expressions
bool VisitBinaryOperator(BinaryOperator *E)
{
	/*const auto& parents = astContext->getParents(*E);
	const Stmt *ST = parents[0].get<Stmt>();
	if(!inalways(ST) && !isa<ForStmt>(ST))
	{
		if( E->getOpcode() == BO_Assign )
		{
			SourceLocation loc = E->getLocStart();
			TheRewriter.InsertText(loc, "assign ", true, true);
		}
	}*/
	std::string varname;
	char rep[256];
	if (DeclRefExpr *DRE = dyn_cast<DeclRefExpr>(E->getLHS())) 
	{
		if (VarDecl *VD = dyn_cast<VarDecl>(DRE->getDecl())) 
		{
			varname = VD->getQualifiedNameAsString();
		}
	}
	
	
	if( E->getOpcode() == BO_MulAssign )
	{
		sprintf(rep,"= %s *",varname.c_str());
		TheRewriter.ReplaceText(E->getOperatorLoc(), E->getOpcodeStr().size(),rep);
	}
	else if( E->getOpcode() == BO_DivAssign )
	{
		sprintf(rep,"= %s /",varname.c_str());
		TheRewriter.ReplaceText(E->getOperatorLoc(), E->getOpcodeStr().size(),rep);
	}
	else if( E->getOpcode() == BO_RemAssign )
	{
		sprintf(rep,"= %s %%",varname.c_str());
		TheRewriter.ReplaceText(E->getOperatorLoc(), E->getOpcodeStr().size(),rep);
	}
	else if( E->getOpcode() == BO_AddAssign )
	{
		sprintf(rep,"= %s +",varname.c_str());
		TheRewriter.ReplaceText(E->getOperatorLoc(), E->getOpcodeStr().size(),rep);
	}
	else if( E->getOpcode() == BO_SubAssign )
	{
		sprintf(rep,"= %s -",varname.c_str());
		TheRewriter.ReplaceText(E->getOperatorLoc(), E->getOpcodeStr().size(),rep);
	}
	else if( E->getOpcode() == BO_AndAssign )
	{
		sprintf(rep,"= %s &",varname.c_str());
		TheRewriter.ReplaceText(E->getOperatorLoc(), E->getOpcodeStr().size(),rep);
	}
	else if( E->getOpcode() == BO_XorAssign )
	{
		sprintf(rep,"= %s ^",varname.c_str());
		TheRewriter.ReplaceText(E->getOperatorLoc(), E->getOpcodeStr().size(),rep);
	}
	else if( E->getOpcode() == BO_OrAssign )
	{
		sprintf(rep,"= %s |",varname.c_str());
		TheRewriter.ReplaceText(E->getOperatorLoc(), E->getOpcodeStr().size(),rep);
	}




  return true;
}

//#### Override Binary Operator expressions
bool VisitUnaryOperator(UnaryOperator *E)
{
	std::string varname;
	char rep[256];
	if (DeclRefExpr *DRE = dyn_cast<DeclRefExpr>(E->getSubExpr())) 
	{
		if (VarDecl *VD = dyn_cast<VarDecl>(DRE->getDecl())) 
		{
			varname = VD->getQualifiedNameAsString();
		}
	}
	
	
	if( E->getOpcode() == UO_PostInc )
	{
		sprintf(rep,"= %s + 1",varname.c_str());
		TheRewriter.ReplaceText(E->getOperatorLoc(), E->getOpcodeStr(UO_PostInc).size(),rep);
	}
	else if( E->getOpcode() == UO_PostDec )
	{
		sprintf(rep,"= %s - 1",varname.c_str());
		TheRewriter.ReplaceText(E->getOperatorLoc(), E->getOpcodeStr(UO_PostDec).size(),rep);
	}
	else if( E->getOpcode() == UO_PreInc )
	{
		sprintf(rep,"= %s + 1",varname.c_str());
		TheRewriter.ReplaceText(E->getOperatorLoc(), E->getOpcodeStr(UO_PreInc).size(),rep);
	}
	else if( E->getOpcode() == UO_PreDec )
	{
		sprintf(rep,"= %s - 1",varname.c_str());
		TheRewriter.ReplaceText(E->getOperatorLoc(), E->getOpcodeStr(UO_PreDec).size(),rep);
	}
	


  return true;
}



//#### Override variable declaration to insert verilog syntax
bool VisitVarDecl(VarDecl *V)
{
	//clang::Rewriter::RewriteOptions opts = clang::Rewriter::RewriteOptions();
	//opts.RemoveLineIfEmpty =1;

	SourceLocation end = V->getLocEnd();
	int offset = Lexer::MeasureTokenLength(end, TheRewriter.getSourceMgr(), TheRewriter.getLangOpts());
	SourceLocation semicol = end.getLocWithOffset(offset);
	SourceLocation start = V->getLocStart();

	if(!isa<ParmVarDecl>(V))
	{


		const auto& parents = astContext->getParents(*V);
		const Stmt *ST = parents[0].get<Stmt>();
		//const DeclStmt *DS = cast<DeclStmt>(ST);
		if (checkfor(ST))
		{
			QualType qin = V->getType();
			SourceLocation paramloc = V->getLocStart();
			TheRewriter.RemoveText(paramloc,qin.getAsString().size());
		}
		else
		{
			char rep[256];
			QualType vtype = V->getType();

			if (vtype->isArrayType())
			{
				ConstantArrayType const *ArrayTy = dyn_cast<ConstantArrayType>(V->getType().getTypePtr());
				if (ArrayTy->getElementType()->isBooleanType())
				{
					if(V->getStorageClass() == SC_Register)
					{
						sprintf(rep,"reg %s [%ld:0];\n",V->getDeclName().getAsString().data(),ArrayTy->getSize().getSExtValue()-1);
					}
					else
					{
						sprintf(rep,"wire %s [%ld:0];\n",V->getDeclName().getAsString().data(),ArrayTy->getSize().getSExtValue()-1);
					}
					SourceRange stmtrange = SourceRange(start,end);
					TheRewriter.RemoveText(semicol,1);
					TheRewriter.RemoveText(stmtrange);

					SourceLocation temp = moving_start;
					TheRewriter.InsertTextBefore(temp, rep);
				}
				else
				if (ArrayTy->getElementType()->isIntegerType())
				{
					if(V->getStorageClass() == SC_Register)
					{
						sprintf(rep,"reg [4:0] %s [%ld:0];\n",V->getDeclName().getAsString().data(),ArrayTy->getSize().getSExtValue()-1);
					}
					else
					{
						sprintf(rep,"wire [4:0] %s [%ld:0];\n",V->getDeclName().getAsString().data(),ArrayTy->getSize().getSExtValue()-1);
					}
					SourceRange stmtrange = SourceRange(start,end);
					TheRewriter.RemoveText(semicol,1);
					TheRewriter.RemoveText(stmtrange);

					SourceLocation temp = moving_start;
					TheRewriter.InsertTextBefore(temp, rep);
				}
				else
				if (ArrayTy->getElementType()->isArrayType())
				{
					ConstantArrayType const *Array2Ty = dyn_cast<ConstantArrayType>(ArrayTy->getElementType().getTypePtr());
					if (Array2Ty->getElementType()->isBooleanType())
					{
						if(V->getStorageClass() == SC_Register)
						{
							sprintf(rep,"reg %s [%ld:0][%ld:0];\n",V->getDeclName().getAsString().data(),ArrayTy->getSize().getSExtValue()-1,Array2Ty->getSize().getSExtValue()-1);
						}
						else
						{
							sprintf(rep,"wire %s [%ld:0][%ld:0];\n",V->getDeclName().getAsString().data(),ArrayTy->getSize().getSExtValue()-1,Array2Ty->getSize().getSExtValue()-1);
						}
						SourceRange stmtrange = SourceRange(start,end);
						TheRewriter.RemoveText(semicol,1);
						TheRewriter.RemoveText(stmtrange);

						SourceLocation temp = moving_start;
						TheRewriter.InsertTextBefore(temp, rep);
					}
					else
					if (Array2Ty->getElementType()->isIntegerType())
					{
						if(V->getStorageClass() == SC_Register)
						{
							sprintf(rep,"reg [4:0] %s [%ld:0][%ld:0];\n",V->getDeclName().getAsString().data(),ArrayTy->getSize().getSExtValue()-1,Array2Ty->getSize().getSExtValue()-1);
						}
						else
						{
							sprintf(rep,"wire [4:0] %s [%ld:0][%ld:0];\n",V->getDeclName().getAsString().data(),ArrayTy->getSize().getSExtValue()-1,Array2Ty->getSize().getSExtValue()-1);
						}
						SourceRange stmtrange = SourceRange(start,end);
						TheRewriter.RemoveText(semicol,1);
						TheRewriter.RemoveText(stmtrange);

						SourceLocation temp = moving_start;
						TheRewriter.InsertTextBefore(temp, rep);
					}
				}
				else
					llvm::errs() << "error in array type \n";


			}
			else
			{
				if (vtype->isBooleanType())
				{
					if(V->getStorageClass() == SC_Register)
					{
						sprintf(rep,"reg %s;\n",V->getDeclName().getAsString().data());
					}
					else
					{
						sprintf(rep,"wire %s;\n",V->getDeclName().getAsString().data());
					}
					SourceRange stmtrange = SourceRange(start,end);
					TheRewriter.RemoveText(semicol,1);
					TheRewriter.RemoveText(stmtrange);

					SourceLocation temp = moving_start;
					TheRewriter.InsertTextBefore(temp, rep);
				}
				else
				if (vtype->isIntegerType())
				{//std::string name = V->getDeclName().getAsString().data();
					if(V->getStorageClass() == SC_Register)
					{
						sprintf(rep,"reg [4:0] %s;\n",V->getDeclName().getAsString().data());
					}
					else
					{
						sprintf(rep,"wire [4:0] %s;\n",V->getDeclName().getAsString().data());
					}
					SourceRange stmtrange = SourceRange(start,end);
					TheRewriter.RemoveText(semicol,1);
					TheRewriter.RemoveText(stmtrange);

					SourceLocation temp = moving_start;
					TheRewriter.InsertTextBefore(temp, rep);

				}
				else
					llvm::errs() << "error in input param \n";

			}
		}
	}
	return true;
}


bool VisitStmt(Stmt *s) 
{
	if (isa<IfStmt>(s))
	{
		// Cast s to IfStmt to access the then and else clauses
		IfStmt *If = cast<IfStmt>(s);
		//checkif(s);
		//TheRewriter.InsertText(s->getLocStart(),get_always(),true,true);
		//int offset = Lexer::MeasureTokenLength(s->getLocEnd(), TheRewriter.getSourceMgr(), TheRewriter.getLangOpts());
		//TheRewriter.InsertText(s->getLocEnd().getLocWithOffset(offset),"\nend",true,true);

		Stmt *TH = If->getThen();

		// Add braces if needed to then clause
		InstrumentStmt(TH);

		Stmt *EL = If->getElse();
		if (EL)
		{
			// Add braces if needed to else clause
			InstrumentStmt(EL);
		}
	}
	else
	if (isa<WhileStmt>(s))
	{
		WhileStmt *While = cast<WhileStmt>(s);
		Stmt *BODY = While->getBody();
		InstrumentStmt(BODY);
	}
	else
	if (isa<ForStmt>(s))
	{
		ForStmt *For = cast<ForStmt>(s);
		Stmt *BODY = For->getBody();
		InstrumentFor(BODY,For);
	}
	else
	if (isa<ReturnStmt>(s))
	{
		TheRewriter.ReplaceText(s->getLocStart(), 6,"result = ");
		//TheRewriter.ReplaceText(s->getLocStart(), 6,"result = ");
	}

	return true; 
}

bool VisitFunctionDecl(FunctionDecl *f) 
{

	if (f->hasBody())
	{

		SourceRange sr = f->getSourceRange();
		Stmt *s = f->getBody();

		QualType q = f->getReturnType();
		const Type *typ = q.getTypePtr();
		char fd[256];

		std::string args_in;
		char param_in[10][256];
		//write input ports definitoins 
		for(int i=0;i<f->getNumParams();i++)
		{
			args_in = f->parameters()[i]->getQualifiedNameAsString();
			//llvm::errs() << "params(" << i << ") is " << args[i] << "\n";
			QualType qin = f->parameters()[i]->getType();

			
			//const Type *typin = qin.getTypePtr();
			if (qin->isBooleanType())
			{
				sprintf(param_in[i],"\n input %s",args_in.data());
				SourceLocation paramloc = f->parameters()[i]->getLocStart();
				TheRewriter.RemoveText(paramloc,qin.getAsString().size()-1);
				TheRewriter.ReplaceText(paramloc, f->parameters()[i]->getQualifiedNameAsString().size()+1, param_in[i]);
			}
			else
			if (qin->isIntegerType())
			{
				sprintf(param_in[i],"\n input [4:0] %s",args_in.data());
				SourceLocation paramloc = f->parameters()[i]->getLocStart();
				TheRewriter.RemoveText(paramloc,qin.getAsString().size());
				TheRewriter.ReplaceText(paramloc, f->parameters()[i]->getQualifiedNameAsString().size()+1, param_in[i]);
			}
			else
				llvm::errs() << "error in input param--not supported yet \n";


		}
		SourceLocation param_out_loc = f->parameters()[f->getNumParams()-1]->getLocEnd().getLocWithOffset(f->parameters()[f->getNumParams()-1]->getQualifiedNameAsString().size());

	
		if (typ->isBooleanType())
		sprintf(fd, ",\n output result ");
		else
		if (typ->isIntegerType())
		sprintf(fd, ",\n output [4:0] result ");
		else
		sprintf(fd, "\n error in output param \n");
		SourceLocation ENDf = s->getLocStart();
		int offsett = Lexer::MeasureTokenLength(ENDf,TheRewriter.getSourceMgr(),TheRewriter.getLangOpts()) + 1;
		TheRewriter.ReplaceText(ENDf,offsett,";\n");
		moving_start = s->getLocStart().getLocWithOffset(offsett);
		SourceLocation temp = moving_start;
		TheRewriter.InsertText(temp,"always@*\nbegin\n");

		//write output port definition
		TheRewriter.InsertText(param_out_loc, fd, true, true);
		//TheRewriter.InsertText(ENDf, fd, true, true);

		// Get name of function
		DeclarationNameInfo dni = f->getNameInfo();
		DeclarationName dn = dni.getName();
		std::string fname = dn.getAsString();

		// Point to start of function declaration
		SourceLocation ST = sr.getBegin();

		// Add comment
		char fc[256];
		sprintf(fc, "// Begin conversion of '%s' to Verilog \n", fname.data());
		TheRewriter.InsertText(ST, fc, true, true);
		TheRewriter.ReplaceText(ST, q.getAsString().size(),"");
		TheRewriter.ReplaceText(f->getNameInfo().getLocStart(), fname.size(), "module "+fname);

		if (f->isMain())
		llvm::errs() << "Found main()\n";

		SourceLocation END1 = s->getLocEnd();
		//int offsett1 = Lexer::MeasureTokenLength(END1,TheRewriter.getSourceMgr(),TheRewriter.getLangOpts()) + 1;
		TheRewriter.ReplaceText(END1,offsett,"end\nendmodule\n");

		SourceLocation END = s->getLocEnd().getLocWithOffset(1);
		sprintf(fc, "\n// End of '%s'\n", fname.data());
		TheRewriter.InsertText(END, fc, true, true);
	}

	return true; 
}

private:
	Rewriter &TheRewriter;
	ASTContext *astContext;
	std::string always_params[20];
	int counter = 0;
	SourceLocation moving_start;
};


class MyASTConsumer : public ASTConsumer {
public:
	MyASTConsumer(Rewriter &R, CompilerInstance *CI) : Visitor(R,CI) {}

virtual bool HandleTopLevelDecl(DeclGroupRef DR) {
	for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b)
		Visitor.TraverseDecl(*b);
	return true;
}

private:
	MyASTVisitor Visitor;
};

int main(int argc, char *argv[]) {
	if (argc != 2) {
		llvm::errs() << "Usage: rewritersample <filename>\n";
		return 1;
	}

	
	CompilerInstance TheCompInst;
	TheCompInst.createDiagnostics();

	LangOptions &lo = TheCompInst.getLangOpts();
	lo.GNUMode = 1; 
	lo.CXXExceptions = 1; 
	lo.RTTI = 1; 
	lo.Bool = 1; 
	lo.CPlusPlus = 1;

	
	auto TO = std::make_shared<TargetOptions>();
	TO->Triple = llvm::sys::getDefaultTargetTriple();
	TargetInfo *TI = TargetInfo::CreateTargetInfo(TheCompInst.getDiagnostics(), TO);
	TheCompInst.setTarget(TI);

	TheCompInst.createFileManager();
	FileManager &FileMgr = TheCompInst.getFileManager();
	TheCompInst.createSourceManager(FileMgr);
	SourceManager &SourceMgr = TheCompInst.getSourceManager();
	TheCompInst.createPreprocessor(TU_Module);
	TheCompInst.createASTContext();

	Rewriter TheRewriter;
	TheRewriter.setSourceMgr(SourceMgr, TheCompInst.getLangOpts());

	const FileEntry *FileIn = FileMgr.getFile(argv[1]);
	SourceMgr.setMainFileID(SourceMgr.createFileID(FileIn, SourceLocation(), SrcMgr::C_User));
	TheCompInst.getDiagnosticClient().BeginSourceFile(TheCompInst.getLangOpts(), &TheCompInst.getPreprocessor());



	MyASTConsumer TheConsumer(TheRewriter, &TheCompInst);

	ParseAST(TheCompInst.getPreprocessor(), &TheConsumer, TheCompInst.getASTContext());

	const RewriteBuffer *RewriteBuf = TheRewriter.getRewriteBufferFor(SourceMgr.getMainFileID());
	//llvm::outs() << std::string(RewriteBuf->begin(), RewriteBuf->end());

	std::string fileName(argv[1]);
	std::string outName (fileName);
	size_t ext = outName.rfind(".");
	if (ext == std::string::npos)
		ext = outName.length();
	outName = outName.substr(0,ext);
	outName.insert(ext, ".v");
	llvm::errs() << "Output to: " << outName << "\n";
	std::error_code OutErrorInfo;
	llvm::raw_fd_ostream outFile(llvm::StringRef(outName), OutErrorInfo, llvm::sys::fs::F_None);
	outFile << "/* DWave-c2v: source to source translation of C-code to verilog-code compliant with edif2qmasm using clang tools */\n\n";
	outFile << std::string(RewriteBuf->begin(), RewriteBuf->end());
	outFile.close();
	return 0;
}
