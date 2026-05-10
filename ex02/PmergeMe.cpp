#include "PmergeMe.hpp"

#include <iostream> //<<
#include <sstream> //std::istringstream iss
#include <stdexcept> //reuntime_error
#include <algorithm> //std:swap ; std::lower_bound
#include <climits> // INT_MAX
#include <ctime> //std::clock()
#include <iomanip> //format/affichage : std::fixed ; std::precision()


//Canonical form

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
//istringstream > on transforme la string en flux. ex "42" devient 42
//istringstream est objet de la bibliothéue c++ qui transforme une string en flux (stream). on a : "42 17 8" et puis on peut lire un par un : 42 → 17 → 8
//iss : input string stream


//long n : variable pour stocker le nombre. éviter le dépassement d'un int.
//if (!(iss >> n) || n < 0 || n > static_cast<long>(INT_MAX) >>> vérifie 3 choses : - - nombre positif - pas trop grand  > évite les dépassements de int.
//--si erreur lance une erreur
//iss >> n : je lis un nombre depuis la string. échoue si pas slmt chiffres
//n > static_cast<long>(INT_MAX) : on convertit le INT_MAX en long et on compare.


//leftover >>> vérifie qu'il n'y a rien en trop (ex. "42abb")
//--si le texte reste on throw une erreur
//on ajoute des conteneurs
//static_cast >>> converit long en int
//_vec.empty() >>> si aucun nombre n'a été donné throw erreur identique à_deque.empty ...

void      PmergeMe::parse(int argc, char** argv) 
{
    for(int i = 1; i < argc; i++) 
    {
        std::istringstream iss(argv[i]);
        
        long  n;
        if(!(iss >> n) || n < 0 || n > static_cast<long>(INT_MAX) )
            throw std::runtime_error("Error: invalid argument.");

        std::string leftover;
        if (iss >> leftover)
            throw std::runtime_error("Error: invalid argument.");

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
//prend réf vers un tableau



//Les questions:

// explique en détail ça : if (arr[2 * i] < arr[2 * i + 1])

//explique ce resultat du programme (notammet les 19 et le 17 avec vec c est plus long ? c est quoi us?): ./PmergeMe 12 2 3 4 5
// Before: 12 2 3 4 5
// After:  2 3 4 5 12
// Time to process a range of 5 elements with std::vector : 19.00000 us
// Time to process a range of 5 elements with std::deque  : 17.00000 us
// ➜  ex02 git:(main) ✗


// //explique cette partie, notamment cette partie  std::make_pair(arr[2 * i], arr[2 * i + 1]) et et comment on reconnait les plus grands larger [i] ? :    origPairs[i] = std::make_pair(arr[2 * i], arr[2 * i + 1]);
//         //on prend seulement les grands nombres larger [i] = grand
//         larger[i]  = arr[2 * i];

//donne un exemple de comment il va trier les grands recursivment :  //La fonction s'appelle elle même
// fjVec(larger);


void PmergeMe::fjVec(std::vector<int> & arr) 
{
    //si 0 ou 1 élément on arrête
    if (arr.size() <= 1)
        return;

    // ── Step 0: les éléments pairs
    //on vérifie si le nombre d'éléments est impair
    //true = 1 ; false = 0;
    //est ce qu'il est impair ?
    bool hasStraggler = (arr.size() % 2 != 0);
    
    //si impair, on garde le dernier nombre du arr.back dans Straggler
    int  straggler    = hasStraggler ? arr.back() : 0;

    //on enlève ce dernier nombre pour l'instant
    if (hasStraggler) arr.pop_back();


    //--créer des paires
    //on calcule combien de paires on peut faire ex. [5, 2, 8, 3] > 2 paires
    std::size_t m = arr.size() / 2; // nbr de pairs

    
    // Step 1: sort each adjacent pair so arr[2i] ≥ arr[2i+1] ──────────────
   
   //on force chaque paire à être (grand à gauche, petit à droite). (2, 5) devient (5, 2)
   //m : nombre de pairs
   //arr : tableau vec 
    for (std::size_t i = 0; i < m; i++)
        if (arr[2 * i] < arr[2 * i + 1])
            std::swap(arr[2 * i], arr[2 * i + 1]);


    //step 2: garder la copie des pairs orginaux + extraire les grands éléments

    //tableau vec origParis
    //m : nbr de pairs
    //pair: objet qui contient (first, second)
    std::vector <std::pair <int, int> > origPairs(m);
    //plus grand élément
    std::vector<int> larger(m);
    
    for (std::size_t i = 0; i < m; i++) 
    {
       //on garde les pairs originales origPairs[i] = (grand, petit)
       //2 * 0 = 0 puis 2 * 1 = 2 (deuxième paire)
        origPairs[i] = std::make_pair(arr[2 * i], arr[2 * i + 1]);
        //on prend seulement les grands nombres larger [i] = grand
        //on prend celui de gauche (grâce au swap précédent)
        larger[i]  = arr[2 * i];
    }



    // ── Step 3: récursivement on range les plus grands (Ford-Johnson) 
    //La fonction s'appelle elle même
    //ex. : arr = [12,2,9,1,7,3,8,4] puis : larger = [12,9,7,8], puis fjVec(larger), puis fjVec([12,8]), puis 12 on arrête et on remonte 
    fjVec(larger);


    // Step 4: re-associate each sorted larger with its smaller partner 

    //ex: origPairs:
    // (3,9)
    // (1,7)
    // (4,8)
    //puis extraction : larger = 9 7 8
    //puis recherche 7
    //sortedParis remet les pairs ensemble

    //on remet chaque grand avec son petit

    //tableau de booléans (tout initialisé à false) (ex. false false false)
    //sert à dire est ce que cette paire a été déjà utilisée ? (ex avec les doublons)
    //m = nombres de pairs
    std::vector<bool> used(m, false);

    //on crée le tableau final qui contiendra p'rdre des gradns triés
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


    // step 5 : contruire mainchain et pend
    //on a déjà sortedPairs (petit, grand) !!! petit, grand... les grands sont déjà triés !!!
    //   b1 ≤ a1 ≤ a2 ≤ … est garantie après le tri récursif

    //on reconstruit la base mainChain (contient le premier petit et tous les grands triés)
    //pend : les élements à insérer 

    //on crée se vecteur
    //
    std::vector<int> mainChain;
    //reserve : réserve de la mémoire à l'avance
    mainChain.reserve(m + 1);
    //on rajoute le second élément de la paire, le petit
    mainChain.push_back(sortedPairs[0].second);
    for (std::size_t i = 0; i < m; i++)
        mainChain.push_back(sortedPairs[i].first); // a1 … am

    std::vector<int> pend;
    pend.reserve(m - 1);
    for (std::size_t i = 1; i < m; i++)
        pend.push_back(sortedPairs[i].second); // b2 … bm

    // ── Step 6: Jacobsthal-ordered binary insertion of pend elements
    //   pend[i] = b_{i+2}, paired with a_{i+2} = sortedPairs[i+1].first
    //   Search range for binary insertion: [begin, pos(a_{i+2})]
    if (!pend.empty()) {
        std::vector<std::size_t> jac      = buildJacobsthal(pend.size());
        std::vector<bool>        inserted(pend.size(), false);

        for (std::size_t k = 1; k < jac.size(); k++) {
            // Current group: pend 0-based indices from lo to hi (inclusive)
            std::size_t hi = std::min(jac[k] - 2, pend.size() - 1);
            std::size_t lo = jac[k - 1] - 1;

            // Insert in reverse order within the group
            for (int i = static_cast<int>(hi); i >= static_cast<int>(lo); i--) {
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


// Ford-Johnson for std::deque<int>


void PmergeMe::fjDeq(std::deque<int>& arr) {
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

// ─────────────────────────────────────────────────────────────────────────────
// Public sort: display, time both containers, display result
// ─────────────────────────────────────────────────────────────────────────────


//affiche les nombres avant : std::cout << "before";
//--for(std::size_t i = 0; i < _vec.size(); i++) + affiche chaque nombre avec esapce
//avec std::clock_t startVEc =.. on note le temps actuel
//fjVec(_vec) >>> on trie le vector. _vec contient les nombres
//std::clock_t endVec = std::clock() on reprend le temps après le tri

//    double timeDeq = static_cast<double>(endDeq - startDeq) /        CLOCKS_PER_SEC * 1000000.0; >>> on transforme le temps en microsecondes

//std::cout << std::fixed << std::setprecision(5); >> toujours afficher les décimales (5 chiffres arpès la virgule) (ex.12.34567)




//permet d'affocger les bombres avant tri, tirer avec std::vector puis mesurer le temps du tri et même chose avec deque

void PmergeMe::sort() 
{
    // affiche message
    std::cout << "before:";

    //boucle parcourt tout vector
    //size_t pour index
    //affiche chaque élément du vecteur
    // ex: before: 5 2 9 1
    for (std::size_t i = 0; i < _vec.size(); i++)
        std::cout << " " << _vec[i];
    std::cout << std::endl;

    // on le trie et mesure le temps
    // on note le temps actuel
    //clock ! retourne le temps CPU utilisé
    //startVec : le temps de depart
    std::clock_t startVec = std::clock();
    //fct qui trie le vect
    fjVec(_vec);
    //on récupère le temps après
    std::clock_t endVec = std::clock();


    //calcul de temps du vecteur e, microsecondes
    //conversion en double (car les division entiers perd les décimales)
    //CLOCKS_PER_SEC = constante système   sert à convertir le résultat en secondes ! car le prog retourne le nbre de “ticks” CPU (unité de temps interne en CPU)!! 
    //1 seconde = 1 000 000 microsecondes.
    //temps final = secondes → microsecondes
    double timeVec = static_cast<double>(endVec - startVec) /
                     CLOCKS_PER_SEC * 1000000.0;

    // trier std::deque et mesure du temps
    std::clock_t startDeq = std::clock();
    fjDeq(_deq);
    std::clock_t endDeq = std::clock();
    double timeDeq = static_cast<double>(endDeq - startDeq) /
                     CLOCKS_PER_SEC * 1000000.0;
                
    // Print le trie après le fjVec
    std::cout << "After: ";
    for (std::size_t i = 0; i < _vec.size(); i++)
        std::cout << " " << _vec[i];
    std::cout << std::endl;

    // imprimer timing

    //std::fixed  : notation décimale fixe; setprecision : on fixe 5 chiffres après la virgule. ex: 12.34567
    std::cout << std::fixed << std::setprecision(5);
    // _vec.size() : nombres d éléments
    // µs = us >> micosecondes > millionième de seconde
    std::cout << "time to process a range of " << _vec.size()
              << " elements with std::vector : " << timeVec << " us" << std::endl;

    std::cout << "time to process a range of " << _deq.size()
              << " elements with std::deque  : " << timeDeq << " us" << std::endl;
}
