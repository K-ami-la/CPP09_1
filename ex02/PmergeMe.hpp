#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <vector> //tableau dynamique (accès rapide, mémoire contigue)
#include <deque> //structure double-ended queue(instertion rapides aux deux bouts)
#include <string> //


//classe PmergeMe
//encapsuler un algorithme de tri
//la classe va stocker les données, parser des arguments, trier ces données, comprare possiblement vector vs deque
class PmergeMe 
{
    public:
        PmergeMe();
        PmergeMe(const PmergeMe& other);
        PmergeMe& operator=(const PmergeMe& other);
        ~PmergeMe();

    //prend les argument du programme
    //rôle : convertir les arguments en int
    //remplir _vec et _deq???
    void parse(int argc, char** argv);
    void sort();//fct principale lance le tri

private:
    std::vector<int> _vec;
    std::deque<int>  _deq;

    //static de dépend pas de l'objet(this n'est pas utilisé)
    //fj >>> Ford Johnson 
    static void fjVec(std::vector<int>& arr);
    static void fjDeq(std::deque<int>& arr);
};

#endif

//idée du projet performance vector vs deque (mesurer le temps de tri vector et temps de tri deque)
// je prends des nombres, je les stockes dans 2 structures différentse, je les trie ave algo spécifique et je compare


//que fait le progr 
//1../PmergeMe 3 5 1 9 >> parse transforme les arguments en entiers.
//2. les stockes quand _vec ou _deq
//3. sort() qui appelle fjVEc(_vec); fjDeq(_deq);
//4. affichage (dans cpp) >> état initial, état final trié, temps d'exécution


//FORD Johnson (sorting) algorithm = aka MErge-Insertion sort
//comment il fonctionne : ex. [3, 5, 1, 9]. on va faire des paires (3,5) (1,9). puis on compare dans chaque paire. le + grand de chaque paire et on note le plus petit comme "pendant". ici 5 et  grands et 3 et 1 petit. Etape 3: trier les gradns: [5,9]. Etape4: 3 et 1 insére les petits intelligemment(de manière optimisé non naI¨f). souvent vector est plus rapide en pratique. deque peut être diff selon accès memoire. 
//Input: 3 5 1 9

//         ┌ vector ── fjVec ── sorted
// input ──┤
//         └ deque  ── fjDeq ── sorted
//puis comparaison des deux


//ex.
// Pairs:
// (3,5) (1,9) (2,7)

// Step 1:
//    a = [5,9,7]
//    b = [3,1,2]

// Step 2:
//    sort a → [5,7,9]

// Step 3:
//    insert b dans ordre optimisé




//La suite de Jacobsthal est une suite de nombres :

// 1, 3, 5, 11, 21, 43, 85, ..
//J(n) = J(n-1) + 2 * J(n-2)
//on l utilise pour décider dans quel ordre insérer les petits(b)
//swap → garantit (grand, petit)
// larger → liste des grands
// Jacobsthal → ordre optimal pour insérer les petits
// !!!! 
//stratégie d'ordre d'insertion, pas un tri

//l'algo fait 
// 1.

// paires → (grand, petit)

// 2.

// trier les grands

// 3.

// mettre les grands dans mainChain

// 4.

// garder les petits (pend)

// 5.

// Jacobsthal décide l’ordre d’insertion des petits

// 6.

// insertion avec binary search