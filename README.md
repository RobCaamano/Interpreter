# Interpreter
Interpreter for a simple programming language

-------------------------------------------------------------------------------------------------------------------

The Language is describes as follows:

1: The language has two types: INT, and FLOAT.

2: The PLUS and MINUS operators in Expr represent addition and subtraction operations.

3: The MULT, DIV, and REM operators in represent multiplication, division and remainder operations.

4: The PLUS, MINUS, MULT, DIV, and REM operators are left associative.

5: MUL, DIV, and REM have higher precedence than PLUS and MINUS.

6: Unary operator for the sign have higher precedence than MULT, DIV, and REM.

7: All variables have to be declared in declaration statements.

8: An IfStmt evaluates a logical expression (LogicExpr) as a condition. If the logical expression value is true, then the Stmt block is executed, otherwise it is not.

9: A WriteStmt evaluates the list of expressions (ExprList), and prints their values in order from left to right.

10: The ASSOP operator in the AssignStmt that assigns a value to a variable. It evaluates the Expr on the right-hand side and saves its value in a memory location associated with the left-hand side variable (Var). The left-hand side variable can be assigned any of the numeric types (i.e., INT, FLOAT) of the language. For example, an integer variable can be assigned a real value, and a real variable can be assigned an integer value. In either case, conversion of the value to the type of the variable must be applied. It is illegal to assign a string literal (SCONST) to a numeric type variable.

11: The binary operations for addition, subtraction, multiplication, and division are performed upon two numeric operands (i.e., INT or FLOAT) of the same or different types. If the operands are of the same type, the type of the result is the same type as the operator’s operands. Otherwise, the type of the result is real.

12: The binary operation for REM must be performed upon two integer operands. It is illegal to have a real operand for the REM operator.

13: The EQUAL and GTHAN relational operators operate upon two compatible operands. The evaluation of a logical expression, based on EQUAL or GTHAN operation, must be of a Boolean type that produces either a true or false value that controls whether the statement(s) of the selection IfStmt is executed or not.

14: It is an error to use a variable in an expression before it has been assigned.

15: The unary sign operators (+ or -) are applied upon a unary numeric operands (i.e., INT or FLOAT).

-------------------------------------------------------------------------------------------------------------------

The Interpreter provides the following:

1: It performs syntax analysis of the input source code statement by statement, then executes the statement if there is no syntactic or semantic error.

2: It builds information of variables types in the symbol table for all the defined variables.

3: It evaluates expressions and determines their values and types.

4: The results of an unsuccessful parsing and interpreting are a set of error messages printed by the parser/interpreter functions, as well as the error messages that might be detected by the lexical analyzer.

5: Any failures due to the process of parsing or interpreting the input program should cause the process of interpretation to stop and return back.

6: In addition to the error messages generated due to parsing, the interpreter generates error messages due to its semantics checking. The assignment does not specify the exact error messages that should be printed out by the interpreter. However, the format of the messages should be the line number, followed by a colon and a space, followed by some descriptive text, similar to the format used in Programming Assignment 2. Suggested messages of the interpreter’s semantics check might include messages such as "Run-Time Error-Illegal Mixed Type Operands", " Run-Time Error-Illegal Assignment Operation", "Run-Time Error-Illegal Division by Zero", etc.
