//  A compiler from a very simple Pascal-like structured language LL(k)
//  to 64-bit 80x86 Assembly langage
//  Copyright (C) 2019 Pierre Jourlin
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Build with "make compilateur"

#include <string>
#include <iostream>
#include <cstdlib>

using namespace std;

char current;                // Current char
char nextcar;

void ReadChar(void) {
    while (cin.get(current) && (current == ' ' || current == '\t' || current == '\n'))
        ; // ignore spaces
    nextcar = cin.peek();
}

void Error(string s) {
    cerr << s << endl;
    exit(-1);
}

void AdditiveOperator(void) {
    if (current == '+' || current == '-')
        ReadChar();
    else
        Error("Opérateur additif attendu");
}

void Digit(void) {
    if ((current < '0') || (current > '9'))
        Error("Chiffre attendu");
    else {
        cout << "\tpush $" << current << endl;
        ReadChar();
    }
}

void ArithmeticExpression(void);
string RelationalOperator(void);

void Term(void) {
    if (current == '(') {
        ReadChar();
        ArithmeticExpression();
        if (current != ')')
            Error("')' était attendu");
        else
            ReadChar();
    } else if (current >= '0' && current <= '9')
        Digit();
    else
        Error("'(' ou chiffre attendu");
}

void ArithmeticExpression(void) {
    char adop;
    Term();
    while (current == '+' || current == '-') {
        adop = current;
        AdditiveOperator();
        Term();
        cout << "\tpop %rbx" << endl;
        cout << "\tpop %rax" << endl;
        if (adop == '+')
            cout << "\taddq %rbx, %rax" << endl;
        else
            cout << "\tsubq %rbx, %rax" << endl;
        cout << "\tpush %rax" << endl;
    }

    string op = RelationalOperator();
    if (op != "") {
        Term();
        while (current == '+' || current == '-') {
            adop = current;
            AdditiveOperator();
            Term();
            cout << "\tpop %rbx" << endl;
            cout << "\tpop %rax" << endl;
            if (adop == '+')
                cout << "\taddq %rbx, %rax" << endl;
            else
                cout << "\tsubq %rbx, %rax" << endl;
            cout << "\tpush %rax" << endl;
        }

        cout << "\tpop %rbx" << endl; // expr2
        cout << "\tpop %rax" << endl; // expr1
        cout << "\tcmpq %rbx, %rax" << endl;

        if (op == "=")
            cout << "\tsete %al" << endl;
        else if (op == "<>")
            cout << "\tsetne %al" << endl;
        else if (op == "<")
            cout << "\tsetl %al" << endl;
        else if (op == "<=")
            cout << "\tsetle %al" << endl;
        else if (op == ">")
            cout << "\tsetg %al" << endl;
        else if (op == ">=")
            cout << "\tsetge %al" << endl;

        cout << "\tmovzbq %al, %rax" << endl;
        cout << "\tpush %rax" << endl;
    }
}

string RelationalOperator(void) {
    if (current == '<') {
        ReadChar();
        if (current == '=') {
            ReadChar();
            return "<=";
        } else if (current == '>') {
            ReadChar();
            return "<>";
        }
        return "<";
    } else if (current == '>') {
        ReadChar();
        if (current == '=') {
            ReadChar();
            return ">=";
        }
        return ">";
    } else if (current == '=') {
        ReadChar();
        return "=";
    } else {
        return "";
    }
}

int main(void) {
    cout << "\t\t\t# This code was produced by the CERI Compiler" << endl;
    cout << "\t.text\t\t# The following lines contain the program" << endl;
    cout << "\t.globl main\t# The main function must be visible from outside" << endl;
    cout << "main:\t\t\t# The main function body :" << endl;
    cout << "\tmovq %rsp, %rbp\t# Save the position of the stack's top" << endl;

    ReadChar();
    ArithmeticExpression();
    ReadChar();

    cout << "\tpop %rsi\t\t# Valeur à afficher (2e argument pour printf)" << endl;
    cout << "\tleaq format(%rip), %rdi\t# Format string dans rdi" << endl;
    cout << "\txor %rax, %rax\t\t# rax doit être 0 avant appel à printf" << endl;
    cout << "\tcall printf" << endl;

    cout << "\tmovq %rbp, %rsp\t\t# Restore the position of the stack's top" << endl;
    cout << "\tret\t\t\t# Return from main function" << endl;

    cout << "\t.section .rodata" << endl;
    cout << "format:\t.string \"%ld\\n\"" << endl;

    if (cin.get(current)) {
        cerr << "Caractères en trop à la fin du programme : [" << current << "]";
        Error(".");
    }
}
