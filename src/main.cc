#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include "codegen/Visitor.h"

#include "parse/FileInput.h"
#include "parse/Tokenizer.h"
#include "parse/TokenBuffer.h"
#include "parse/Parser.h"

#include "Text.h"

#include <llvm/Support/raw_ostream.h>
#include <fcntl.h>

using namespace rsms;

int main(int argc, char **argv) {
  
  // Read input file
  Text textSource;
  if (!textSource.setFromUTF8FileContents(argc > 1 ? argv[1] : "examples/program1.txt")) {
    std::cerr << "Failed to read input file" << std::endl;
    return 1;
  }
  
  // A tokenizer produce tokens parsed from a ByteInput
  Tokenizer tokenizer(textSource);
  
  // A TokenBuffer reads tokens from a Tokenizer and maintains limited history
  TokenBuffer tokens(tokenizer);
  
  
  while (1) {
    const Token &token = tokens.next();
    
    if (token.type == Token::Unexpected) {
      std::cout << "Error: Unexpected token '" << token.textValue
                << "' at " << token.line << ':' << token.column << std::endl;
      break;
    }
    
    std::cout << "\e[34;1m>> " << token.toString() << "\e[0m";
    
    // list all available historical tokens
    // size_t n = 1;
    // while (n < tokens.count()) {
    //   std::cout << "\n  " << tokens[n++].toString() << ")";
    // }
    std::cout << std::endl;
    
    // An End token indicates the end of the token stream and we must
    // stop reading the stream.
    if (token.type == Token::End || token.type == Token::Error) break;
  }
  
  return 0;
  
  
  
  
  
  
  // A parser reads the token buffer and produce an AST
  Parser parser(tokens);
  
  // Parse the input into an AST
  ast::Function *moduleFunc = parser.parseModule();
  if (!moduleFunc) return 1;
  if (parser.errors().size() != 0) {
    std::cerr << parser.errors().size() << " parse error(s)." << std::endl;
    return 1;
  }
  std::cerr << "Parsed module: " << moduleFunc->body()->toString() << std::endl;
  return 0; // xxx only parser
  
  // Generate code
  codegen::Visitor codegenVisitor;
  llvm::Module *llvmModule = codegenVisitor.genModule(llvm::getGlobalContext(), "hello", moduleFunc);
  //std::cout << "moduleIR: " << llvmModule << std::endl;
  if (!llvmModule) {
    std::cerr << codegenVisitor.errors().size() << " error(s) during code generation." << std::endl;
    return 1;
  }
  
  // Write IR to stderr
  llvmModule->dump();
  
  // Write human-readable IR to file "out.ll"
  std::string errInfo;
  llvm::raw_fd_ostream os("out.ll", errInfo);
  if (os.has_error()) {
    std::cerr << "Failed to open 'out.ll' file for output. " << errInfo << std::endl;
    return 1;
  }
  llvmModule->print(os, 0);
  
  //
  // From here:
  //
  //   PATH=$PATH:$(pwd)/deps/llvm/bin/bin
  //
  //   Run the generated program:
  //     lli out.ll
  //
  //   Generate target assembler:
  //     llc -asm-verbose -o=- out.ll
  //
  //   Generate target image:
  //     llvm-as -o=- out.ll | llvm-ld -native -o=out.a -
  //
  // See http://llvm.org/docs/CommandGuide/ for details on these tools.
  //
  
  return 0;
}