#include <iostream>
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "Visitor/XML_Visitor.h"
#include "Visitor/Semantic_Visitor.h"
#include "Visitor/Interpreter_Visitor.h"

int main(int argc, char **argv) {
    std::string _program_ =
            "float Square (x : float) {\n"
            "\treturn x*x;\n"
            "}\n"
            "bool XGreaterThanY (x : float , y : float) {\n"
            "\tlet ans : bool = true;\n"
            "\tif (y > x) {ans = false ;}\n"
            "\treturn ans;\n"
            "}\n"
            "bool XGreaterThanYv2 (x : float , y : float) {\n"
            "\treturn x > y;\n"
            "}\n"
            "float AverageOfThree (x : float , y : float , z : float) {\n"
            "\tlet total : float = x + y + z ;\n"
            "\treturn total / 3.0;\n"
            "}\n"
            "\n"
            "string JoinStr (s1 : string , s2 : string) {\n"
            "\tlet s3 : string = s1 + s2 ;\n"
            "\treturn s3 ;\n"
            "}\n"
            "let x : float = 2.4;\n"
            "let y : float = Square (2.5);\n"
            "print y ; //6.25\n"
            "print XGreaterThanY (x , 2.3 ) ; // t r u e\n"
            "print XGreaterThanYv2 (Square ( 1.5 ) , y ) ; // f a l s e\n"
            "print AverageOfThree (x , y , 1.2 ) ; //3.28\n"
            "print JoinStr(\"Hello\" , \"World\" ) ; // H e l l o World\n"
            "for(let ii : int = 0; ii < 10; ii = ii + 1){\n"
            "   print ii;\n"
            "}\n"
            "let ii : int = 0;\n"
            "while (ii < 10){\n"
            "   print ii;\n"
            "   ii = ii + 1;\n"
            "}\n";
    std::string _program_2 =
            "auto XGreaterY(toCompare[] : float){\n" // 1
            "    let ans : auto = false;\n"
            "    if(toCompare[0] > toCompare[1])   {\n"
            "        ans=true;\n"
            "    }\n"
            "    return ans;\n"
            "}\n"
            "\n"
            "auto XGreaterY(x : int, y : int){\n"
            "    if(x>y){\n" //10
            "        return true;\n"
            "    }else{\n"
            "        return false;\n"
            "    }\n"
            "}\n"
            "\n"
            "float Average(toAverage[] : float , count : float){\n"
            "    let total : float = 0.0;\n"
            "    for(let i : float = 0.0; i < count; i=i+1.0){\n"
            "        total = total + toAverage[i];\n" //20
            "    }\n"
            "    return total / count;\n"
            "    }\n"
            "\n"
            "let arr1[2] : float;\n"
            "let arr2[4] : float = {2.4, 2.8, 10.4, 12.1};\n"
            "\n"
            "arr1[0] = 22.4;\n"
            "arr1[1] = 6.25;\n"
            "print arr1[1]; //6.25\n" //30
            "print XGreaterY(arr1); //true\n"
            "print XGreaterY(2,3); //false\n"
            "print Average(arr2, 4.0); //6.92\n"
            "\n"
            "tlstruct Vector{\n"
            "    let x : float = 0.0;\n"
            "    let y : float = 0.0;\n"
            "    let z : float = 0.0;\n"
            "\n"
            "    int Scale(s : float){\n" //40
            "        x=x*s;\n"
            "        y=y*s;\n"
            "        z=z*s;\n"
            "        return 0;//Because functions always return something\n"
            "    }\n"
            "    int Translate(tx : float,ty : float,tz : float){\n"
            "        x=x+tx;\n"
            "        y=y+ty;\n"
            "        z=z+tz;\n"
            "        return 0;//Language does not support void\n" //50
            "    }\n"
            "}\n"
            "\n"
            "Vector Add(v1 : Vector, v2 : Vector){\n"
            "    let v3 : Vector;\n"
            "    v3.x = v1.x + v2.x;\n"
            "    v3.y = v1.y + v2.y;\n"
            "    v3.z = v1.z + v2.z;\n"
            "    return v3;\n"
            "}\n" //60
            "\n"
            "let v1 : Vector;\n"
            "v1.x=1.0;\n"
            "v1.y=2.0;\n"
            "v1.z=2.0;\n"
            "\n"
            "let v2 : Vector;\n"
            "v2.x=2.0;\n"
            "v2.y=1.2;\n"
            "v2.z=0.0;\n" //70
            "\n"
            "let v3 : Vector = Add(v1,v2);\n"
            "print v3.x;//3.0\n"
            "print v3.y;//3.2\n"
            "print v3.z;//2.0\n"
            "\n"
            "v3.Translate(1.0, 1.0, 1.0);\n"
            "\n"
            "let v4 : Vector=Add(v1, v3);\n"
            "print v3.x;//5.0\n" //80
            "print v3.y;//6.2\n"
            "print v3.z;//5.0";
    if (std::string("-l") == argv[1]){
//        std::cout << "TESTING LEXER" << std::endl;
        lexer::Lexer lexer;
        lexer.extractLexemes(argv[2]);
        std::cout << "value: type" << std::endl;
        int j = 0;
        for (const auto& i: lexer.tokens){
            std::cout << j++ << " : " << i.value << " : " << i.type << std::endl;
        }
    }else if (std::string("-p") == argv[1]){
//        std::cout << "TESTING PARSER" << std::endl;
        lexer::Lexer lexer;
        lexer.extractLexemes(_program_2);

        parser::Parser parser(lexer.tokens);
        auto programNode = std::shared_ptr<parser::ASTProgramNode>(parser.parseProgram());
    }else if (std::string("-x") == argv[1]) {
//        std::cout << "TESTING XML Generator" << std::endl;

        lexer::Lexer lexer;
        lexer.extractLexemes(argv[2]);

        parser::Parser parser(lexer.tokens);
        auto programNode = std::shared_ptr<parser::ASTProgramNode>(parser.parseProgram());

        visitor::XMLVisitor xmlVisitor;
        auto *programNode1 = new parser::ASTProgramNode(programNode);
        xmlVisitor.visit(programNode1);

        delete programNode1;
    }else if (std::string("-s") == argv[1]) {
//        std::cout << "TESTING Semantic Analyzer" << std::endl;
        lexer::Lexer lexer;
        lexer.extractLexemes(argv[2]); //_program_

        parser::Parser parser(lexer.tokens);
        auto programNode = std::shared_ptr<parser::ASTProgramNode>(parser.parseProgram());

        visitor::SemanticAnalyser semanticAnalyser;
        auto *programNode1 = new parser::ASTProgramNode(programNode);
        semanticAnalyser.visit(programNode1);

        delete programNode1;
    }else if (std::string("-i") == argv[1]){
//        std::cout << "TESTING Interpreter" <<  aasdasdstd::endl;

        lexer::Lexer lexer;
        lexer.extractLexemes(_program_); //argv[2]

        parser::Parser parser(lexer.tokens);
        auto programNode = std::shared_ptr<parser::ASTProgramNode>(parser.parseProgram());

        visitor::SemanticAnalyser semanticAnalyser;
        auto *programNode1 = new parser::ASTProgramNode(programNode);
        semanticAnalyser.visit(programNode1);

        visitor::Interpreter interpreter;
        interpreter.visit(programNode1);

        delete programNode1;
    }
    return 0;
}