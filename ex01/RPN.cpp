#include "RPN.hpp"
#include <sstream> //découper une string en morceaux tokens (std::istringstream)
#include <stdexcept> //lande des erreurs (std::runtime_error)

RPN::RPN() {}

RPN::RPN (const RPN& other) :  _stack(other._stack)  {}

RPN &RPN::operator=(const RPN& other) {
    if (this != &other)
         _stack = other._stack;
    return  *this;
}

RPN::~RPN() {}




//la fct prend une string et retourne un int
int RPN::evaluate(const std::string& expr) 
{
    // Clear the stack before evaluation
    while (!_stack.empty())
        _stack.pop(); //pop on enlève

    //Cette ligne transforme une string (expr) en flux de lecture et permet de lire la string mot par mot
    std::istringstream iss(expr);
    std::string token;

    //lit un élément un par un
    // >> lit jusqu a un espace
    while (iss >> token) 
    {
        //si le token est un opérateur
        if (token.size() == 1 &&
            (token[0] == '+' || token[0] == '-' ||
             token[0] == '*' || token[0] == '/')) 
             
             {

            //on doit avoir 2 nombres minimum
            if (_stack.size() < 2)
                throw std::runtime_error("Error");

            //on regarde puis on enlève
            //on récupère les deux opérandes
            int b = _stack.top(); _stack.pop();
            int a = _stack.top(); _stack.pop();

//ex équivalent :
//             if (op == '+') ...
//             else if (op == '-') ...
//             else if (op == '*') ...
            switch (token[0]) {
                case '+': _stack.push(a + b); break;
                case '-': _stack.push(a - b); break;
                case '*': _stack.push(a * b); break;
                case '/':

//          0 / 5 = 0   ✅ OK
//         5 / 0 = ❌ interdit
                    if (b == 0)
                        throw std::runtime_error("Error: division by zero.");
                    _stack.push(a / b);
                    break;
            }
        } 
            else if (token.size() == 1 &&
                   token[0] >= '0' && token[0] <= '9') 
                    {
                     _stack.push(token[0] - '0');
                    }       
                else 
                {
                    throw std::runtime_error("Error");
                 }
    }

    if (_stack.size() != 1)
        throw std::runtime_error("Error");

    
    return _stack.top();//on retourne le résultat final
}
