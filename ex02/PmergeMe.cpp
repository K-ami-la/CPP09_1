#include "PmergeMe.hpp"

#include <iostream> //<<
#include <sstream> //std::istringstream iss
#include <stdexcept> //runtime_error
#include <algorithm> //std:swap ; std::lower_bound
#include <climits> // INT_MAX
#include <ctime> //std::clock()
#include <iomanip> //format/affichage: std::fixed ; std::precision()


PmergeMe::PmergeMe() {}

PmergeMe::PmergeMe(const PmergeMe& other) : _vec(other._vec), _deq(other._deq) {}

PmergeMe& PmergeMe::operator=(const PmergeMe& other)
{
    if (this != &other) 
    {
        _vec = other._vec;
        _deq = other._deq;
    }    return *this;

}

PmergeMe::~PmergeMe() {}




//parse
// on parse les chiffres à l'entrée (input)

//on commence à 1 (les nombres)
//istringstream > on transforme la string en flux (ça peut être double, long, float, string!). ex "42" devient 42
//istringstream est objet de la bibliothéue c++ qui transforme une string en flux (stream). on a : "42 17 8" et puis on peut lire un par un : 42 → 17 → 8
//iss : input string stream


//long n : variable pour stocker les grands nombres (float = nbre à virgule). éviter le dépassement d'un int. LONG_MAX = 9223372036854775807
//if (!(iss >> n) || n < 0 || n > static_cast<long>(INT_MAX) >>> vérifie 3 choses : - - nombre positif - pas trop grand  > évite les dépassements de int.
//--si erreur lance une erreur
//iss >> n : je lis un nombre depuis la string. échoue si pas slmt chiffres
//n > static_cast<long>(INT_MAX) : on convertit le INT_MAX en long et on compare.


//leftover >>> vérifie qu'il n'y a rien en trop (ex. "42abb")
//--si le texte reste on throw une erreur
//on ajoute des conteneurs
//static_cast >>> converti long en int
//_vec.empty() >>> si aucun nombre n'a été donné throw erreur identique à_deque.empty ...

void      PmergeMe::parse(int argc, char** argv) 
{
    for (int i = 1; i < argc; i++) 
    {
        std::istringstream iss(argv[i]);
        
        //long = type demandé à iss >> n = je lis tant que je peux convertir en type demandé
        long  n;
        if(!(iss >> n) || n < 0 || n > static_cast<long>(INT_MAX) )
            throw std::runtime_error("Error: invalid argument.");

        //il reste du texte dans le stream
        std::string leftover;
        if (iss >> leftover)
            throw std::runtime_error("Error: invalid argument.");

        //on crée les deux tableaux avec les suites de nombres
        _vec.push_back(static_cast<int>(n));
        _deq.push_back(static_cast<int>(n));
    }

    if (_vec.empty())
        throw std::runtime_error("Error: no input.");
}

//math + algo pur
//suite de Jacobsthal : chaque nombre = le précédent + 2 × l’avant-précédent
// 1, 3, 5, 11, 21, 43, 85, ...   (t_{k} = t_{k-1} + 2*t_{k-2})
//jac.back : dernier élément du tableau. si 1, 3, 5 ca sera le 5
//!!!! c est le nombre d'éléments dans pend et non les valeures


//la fonction va générer une liste de Jacobstahla >= n 
//std::size_t n : le nombre de nombres dans pend
//ici on aurait pu utiliser le deque au lieu de vec
//pourquoi ici one n + 2 d ou vient cette expression ?
//donne un exemple de cette partie : jac.push_back(jac[sz - 1] + 2 * jac[sz - 2]);


static std::vector<std::size_t> buildJacobsthal(std::size_t  n) 
{
        std::vector<std::size_t> jac;
        jac.push_back(1);
        jac.push_back(3);

    //tant que le dernier est plus petit que n (nbre de tous les nbres dans pend + 2)
    while (jac.back() < n +  2 ) //ex n = 10. jac.back = 3; 10 + 2 = 12. 3 < 12.
    {
        //number of elements à l'instant t; ex jac = [1,3,5]; sz = 3
        std::size_t sz = jac.size();

        //ex sz = 2 (1, 3) : jac[2 - 1 = 1] + 2 * jac[sz - 2 = 0]    = jac[(3)] + 2 * jac[(1)] =   5
        jac.push_back(jac[sz - 1] + 2 * jac[sz - 2]);

    }

    return jac;
}


// Ford-Johnson for std::vector<int>

void PmergeMe::fjVec(std::vector<int> & arr) 
{
   
    //si 0 ou 1 element on arrete
    if (arr.size() <= 1)
        return;

    //step 0: les elements pairs
    //on verifie si le nombre d'elements est impair
    //true = 1 ; false = 0;
    //est ce qu'il est impair ?
    bool hasStraggler = (arr.size() % 2 != 0);
    
    //si impair, on garde le dernier nombre du arr.back dans Straggler
    int  straggler    = hasStraggler ? arr.back() : 0;

    //on enleve ce dernier nombre pour l'instant
    if (hasStraggler) arr.pop_back();


    //step 1 : faire des paires
    //a) on calcule combien de paires on peut faire ex. [5, 2, 8, 3] > 2 paires
    // m = nombre de pairs
    std::size_t m = arr.size() / 2;

    
    //b) on met les grands d'un côté [2i] ≥ arr[2i+1]
    //on force chaque paire à être (grand à gauche, petit à droite). (2, 5) devient (5, 2)
    //arr : tableau vec 
    //ex. first round : indice 0 et indice 1, seconde round: indice 2*1= 2 et indice 2*1+1 = 3...etc.
    for (std::size_t i = 0; i < m; i++)
        if (arr[2 * i] < arr[2 * i + 1])
            std::swap(arr[2 * i], arr[2 * i + 1]);


    //step 2: garder la copie des pairs orginaux + extraire les grands éléments
    //a) on cree un tableau vec, qui s'appelle origPairs et on alloue directement m éléments
    //pair: objet qui contient (.first, .second)
    std::vector <std::pair <int, int> > origPairs(m);

    //tableau stocke les plus grands éléments
    std::vector<int> larger(m);
    

    //on garde les pairs originales origPairs[i] = (grand, petit)
    //première paire : (2 * 0) = 0 puis (2 * 0  + ) = 1...etc
    for (std::size_t i = 0; i < m; i++) 
    {

        origPairs[i] = std::make_pair(arr[2 * i], arr[2 * i + 1]);
        //on prend seulement les grands nombres larger [i] = grand (celui de gauche). ex : (2 * 0) = 0; (2 * 1) = 2; (2 * 2) = 4; etc.
        larger[i]  = arr[2 * i];
    }



    //step 3: étape de récursion. "merge sort" 
    //La fonction s'appelle elle même
    //ex. : arr = [12,2,9,1,7,3,8,4] puis : larger = [12,9,7,8], puis fjVec(larger), puis fjVec([12,8]), puis 12 on arrête et on remonte 
    fjVec(larger);


    // Step 4: réassocier chaque grand avec son petit grâce à OrigPairs

    //tableau de booléans used (tout initialisé à false) (ex. false false false). false = n'a pas été appelé
    //sert à dire est ce que cette paire a été déjà utilisée ?
    std::vector<bool> used(m, false);

    //on crée le tableau final qui contiendra les gradns triés
    std::vector<std::pair<int, int> > sortedPairs(m);

    //on parcourt les grands triés
    // tant que i < nombres de pairs
    for (std::size_t i = 0; i < m; i++) 
    {
        for (std::size_t j = 0; j < m; j++) 
        {
            //on parcourt les grands triés
            //quel pair original avait ce grand élément
            if (!used[j] && origPairs[j].first == larger[i]) 
            {
                sortedPairs[i] = origPairs[j];
                used[j]        = true;
                break;
            }
        }
    }


    //step 5 : on a SortedPairs. Maintenant on fait le squelette trié(MainChain) (b1, a1, a2, a3); pend éléments à insérer.
    std::vector<int> mainChain;

    //reserve : réserve de la mémoire à l'avance
    mainChain.reserve(m + 1);

    //on rajoute le second élément de la paire, le petit >>> construction spécifique à Ford–Johnson !.
    mainChain.push_back(sortedPairs[0].second);
    for (std::size_t i = 0; i < m; i++)
        mainChain.push_back(sortedPairs[i].first); //on rajoute les grands

    std::vector<int> pend;
    pend.reserve(m - 1);
    for (std::size_t i = 1; i < m; i++)
        pend.push_back(sortedPairs[i].second); // on rajoute les petits

    
    
   //step 6 Jacobsthal insertion.

    if (!pend.empty()) 
    {
        //on crée un tableau jac de taille >= n + 2 de pend avec la suite de jacobsthal
        //on donne la taille du tabl pend !
        //ex . pend.size() = 6; jac = [1, 3, 5, 11] (c est une suite qui dépasse 6 (=the size) et pas qui est plus grand que size)
        std::vector<std::size_t> jac  = buildJacobsthal(pend.size());
        std::vector<bool>  inserted(pend.size(), false);

        for (std::size_t k = 1; k < jac.size(); k++) 
        {
            //jac va découper pend en groupes
            //indice dans jac[]
            //lo et hi : indices dans pend[]
            //min: on prend une valeur qui ne depasse la taille de pend[]
            //pend[lo ... hi]

            //ex: pend = [10, 20, 30, 40, 50, 60] ; jac[1, 3, 5, 11] qui va donner 
            //>>> lo = jac[k - 1] - 1 > (=jac[0] - 1) > (= 1 - 1) = 0. début du group index = 0.
            //hi = min(jac[k] - 2, pend.size() - 1) = jac[1] = 3 - 2 =  1...
            //groupe 1 : pend[0..1].

            //- 1 dans lo : pour aligner la base de jac > -1 = conversion math → C++ indexing
            // - 2 dans hi : aligner la base de jac + on traite à partir de b2 car b1 déjà trié !!!
            std::size_t hi = std::min(jac[k] - 2, pend.size() - 1);
            std::size_t lo = jac[k - 1] - 1;

            // insetion dans l ordre inverse (car les grands sont plus risque)
            for (int i = static_cast<int>(hi); i >= static_cast<int>(lo); i--) 
            {
                if (i < 0 || static_cast<std::size_t>(i) >= pend.size())
                    continue;
                if (inserted[i])
                    continue;

                int val     = pend[i];
                int aPaired = sortedPairs[i + 1].first; // a_{i+2}

                // Find the current position of aPaired in mainChain
                std::vector<int>::iterator bound = mainChain.end();
                for (std::vector<int>::iterator it = mainChain.begin();
                     it != mainChain.end(); ++it) {
                    if (*it == aPaired) {
                        bound = it + 1;
                        break;
                    }
                }

                // Binary search in [begin, bound) and insert
                std::vector<int>::iterator pos =
                    std::lower_bound(mainChain.begin(), bound, val);
                //on insère les petits un par un (mais dnas un ordre spécial(JAcobsthal)) avec rechrche rapide lower_bound
                //--lower_bound veut dirre trouver la bonne position pour insérer sans casser l'ordre
                 mainChain.insert(pos, val);
                inserted[i] = true;
            }
        }
    }

    //Step 7: inserer straggler
    //si on avait un nombre seul, on l'insère à la fin correctement

    if (hasStraggler)
    {
        // on crée un itérateur pos ?
        std::vector<int>::iterator pos =
            std::lower_bound(mainChain.begin(), mainChain.end(), straggler);
        mainChain.insert(pos, straggler);
    }

    //on remplace l'ancien tableau par le trié
    arr = mainChain;
}

//lowe_bound function :

// template <class Iterator, class T>
// Iterator lower_bound(Iterator first, Iterator last, const T& value)
// {
//     while (first != last)
//     {
//         // 1. on calcule le milieu de la zone
//         Iterator mid = first + (last - first) / 2;

//         // 2. comparaison avec l'élément du milieu
//         if (*mid < value)
//         {
//             // value est PLUS GRAND
//             // on ignore toute la partie gauche (mid inclus)
//             first = mid + 1;
//         }
//         else
//         {
//             // value <= *mid
//             // on garde la gauche (mid devient la nouvelle fin)
//             last = mid;
//         }
//     }

//     // 3. first == position d'insertion
//     return first;
// }



// Ford-Johnson for std::deque<int>


void PmergeMe::fjDeq(std::deque<int>& arr) 
{
    if (arr.size() <= 1)
        return;

    bool hasStraggler = (arr.size() % 2 != 0);
    int  straggler    = hasStraggler ? arr.back() : 0;
    if (hasStraggler) arr.pop_back();

    std::size_t m = arr.size() / 2;

    for (std::size_t i = 0; i < m; i++)
        if (arr[2 * i] < arr[2 * i + 1])
            std::swap(arr[2 * i], arr[2 * i + 1]);

    std::vector<std::pair<int, int> > origPairs(m);
    std::deque<int> larger;
    for (std::size_t i = 0; i < m; i++) {
        origPairs[i] = std::make_pair(arr[2 * i], arr[2 * i + 1]);
        larger.push_back(arr[2 * i]);
    }

    fjDeq(larger);

    std::vector<bool> used(m, false);
    std::vector<std::pair<int, int> > sortedPairs(m);
    for (std::size_t i = 0; i < m; i++) {
        for (std::size_t j = 0; j < m; j++) {
            if (!used[j] && origPairs[j].first == larger[i]) {
                sortedPairs[i] = origPairs[j];
                used[j]        = true;
                break;
            }
        }
    }

    std::deque<int> mainChain;
    mainChain.push_back(sortedPairs[0].second);
    //boucle sur toutes les paires
    for (std::size_t i = 0; i < m; i++)
        mainChain.push_back(sortedPairs[i].first);

    //on crée un tbaleau autre vecteur pend
    std::vector<int> pend;
    //m - 1 : parce que b1 est déjà dans mainChain
    pend.reserve(m - 1);
    //i = 1 : et pas 0 parce que b1 a déjà été utilisé 
    for (std::size_t i = 1; i < m; i++)
    
        //on rajoute les petits éléments restants
        pend.push_back(sortedPairs[i].second);

//resultat : mainChain = [1,7,8,9]
// pend      = [4,3]

    
    if(!pend.empty()) 
    {
        std::vector<std::size_t> jac      = buildJacobsthal(pend.size());
        std::vector<bool>        inserted(pend.size(), false);

        for (std::size_t k = 1; k < jac.size(); k++) {
            std::size_t hi = std::min(jac[k] - 2, pend.size() - 1);
            std::size_t lo = jac[k - 1] - 1;

              for (int i = static_cast<int>(hi); i >= static_cast<int>(lo); i--) {
                if (i < 0 || static_cast<std::size_t>(i) >= pend.size())
                    continue;
                if (inserted[i])
                    continue;

                 int val     = pend[i];
                 int aPaired = sortedPairs[i + 1].first;

                std::deque<int>::iterator bound = mainChain.end();
                for (std::deque<int>::iterator it = mainChain.begin();
                     it != mainChain.end(); ++it) {
                    if (*it == aPaired) {
                        bound = it + 1;
                        break;
                    }
                }

                std::deque<int>::iterator pos =
                    std::lower_bound(mainChain.begin(), bound, val);
                mainChain.insert(pos, val);
                inserted[i] = true;
            }
        }
    }

      if (hasStraggler) 
    {
        std::deque<int>::iterator pos =
            std::lower_bound(mainChain.begin(), mainChain.end(), straggler);
        mainChain.insert(pos, straggler);
    }

        arr = mainChain;
}


//fct sort

//affiche les nombres avant : std::cout << "before";
//--for(std::size_t i = 0; i < _vec.size(); i++) + affiche chaque nombre avec esapce
//avec std::clock_t startVEc =.. on note le temps actuel
//fjVec(_vec) >>> on trie le vector. _vec contient les nombres
//std::clock_t endVec = std::clock() on reprend le temps après le tri

//    double timeDeq = static_cast<double>(endDeq - startDeq) /        CLOCKS_PER_SEC * 1000000.0; >>> on transforme le temps en microsecondes

//std::cout << std::fixed << std::setprecision(5); fixed : toujours afficher les décimales; setprecision (5 chiffres arpès la virgule)



//permet d'afficher les nombres avant tri, tirer avec std::vector puis mesurer le temps du tri et même chose avec deque

//boucle parcourt tout vector
//size_t pour index
//affiche chaque élément du vecteur
// " " avec un espace
// ex: before: 5 2 9 1

//std::clock_t startVec = std::clock();
//on stocke un compteur CPU (type t_clock puis on voit appeler la fct std::clock()) (ticks = unité de mesure du CPU) à l instant T
// on note le temps actuel
//startVec : le temps de depart


//calcul de temps du vecteur e, microsecondes
//conversion en double (car les division entiers perd les décimales)
//CLOCKS_PER_SEC = constante système   sert à convertir le résultat en secondes ! car le prog retourne le nbre de “ticks” CPU (unité de temps interne en CPU)!! 
//1 seconde = 1 000 000 microsecondes.
//temps final = secondes → microsecondes
//double comme float (32 bits) mais plus grand  64 bits. gardent les décimales

// µs = us >> micosecondes > millionième de seconde

void PmergeMe::sort() 
{

    std::cout << "before:";

    for (std::size_t i = 0; i < _vec.size(); i++)
        std::cout << " " << _vec[i];
    std::cout << std::endl;

    std::clock_t startVec = std::clock();

    //fct qui trie le vect
    fjVec(_vec);

    //on récupère le temps après
    std::clock_t endVec = std::clock();

    double  timeVec = static_cast<double>(endVec - startVec) / //calcul du nbr ticks écoulés et converti en double
                     CLOCKS_PER_SEC * 1000000.0;//constante système. sert à convertr en seconde. CLOCKS_PER_SEC = nombre de ticks par seconde = secondes. * 1000000.0 = secondes en microsecondes.

    //pareil, mais avec deque
    std::clock_t startDeq = std::clock();
    fjDeq(_deq);
    std::clock_t endDeq  = std::clock();
    double timeDeq = static_cast<double>(endDeq - startDeq) /
            CLOCKS_PER_SEC * 1000000.0;
     
                    
    // Print le tri après le fjVec
    std:: cout << "After: ";
    for (std::size_t i = 0; i < _vec.size(); i++)
        std::cout << " " << _vec[i];
    std::cout << std::endl;

    
    //notation en interne: surcharge d'opérateur + flag changé : std::ostream& operator<<(std::ostream&, manipulator);
    std::cout << std::fixed << std::setprecision(5);
    std::cout << "time to process a range of " << _vec.size()
              << " elements with std::vector : " << timeVec << " us" << std::endl;

    std::cout <<  "time to process a range of " << _deq.size()
              << " elements with std::deque  : " << timeDeq << " us" << std::endl;
}
