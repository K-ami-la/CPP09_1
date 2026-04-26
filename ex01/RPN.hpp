#ifndef RPN_HPP
#define RPN_HPP

#include <stack> // la pile (std::stack)
#include <string> //std::string

//reverse polish notation ex. 3 4 + > 7
class RPN 
{
//tout est accessible depuis l'exterieur
public:
//constructeurs /desctructeurs
    RPN();
    RPN(const RPN& other);
    RPN& operator=(const RPN& other);
    ~RPN();

    //fonction principale
    //prendre une expression en chaîne(expr)
    //retourner un int
    //lis les tokens(nombres+ opérateurs)
    //utilise une pile pour calculer le résultat
    int evaluate(const std::string& expr);

    //une pile d'entiers
    //empiler les nombres
    //deplier pour faire les opérations + - * /
private:
    std::stack<int> _stack;
};

#endif

//en notation polonaise l'opérateur est avant les opérandes!!!!
//LIFO last in First out : push 3, pop 3, top(voir le sommet), empty(vérifier si vide)

//Jan Łukasiewicz logicien et mathématicien polonais

