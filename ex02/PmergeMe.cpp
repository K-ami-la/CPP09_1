#include "PmergeMe.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <climits>
#include <ctime>
#include <iomanip>

// ─────────────────────────────────────────────────────────────────────────────
// Orthodox Canonical Form
// ─────────────────────────────────────────────────────────────────────────────

PmergeMe::PmergeMe() {}

PmergeMe::PmergeMe(const PmergeMe& other)
    : _vec(other._vec), _deq(other._deq) {}

PmergeMe& PmergeMe::operator=(const PmergeMe& other) {
    if (this != &other) 
    {
        _vec = other._vec;
        _deq = other._deq;
    }
    return *this;
}

PmergeMe::~PmergeMe() {}





// ─────────────────────────────────────────────────────────────────────────────
// Input parsing
// ─────────────────────────────────────────────────────────────────────────────

//on commence à 1 (les nombres)
//istringstream > on transforme la string en flux. ex "42" devient 42
//long n : variable pour stocker le nombre
//if (!(iss >> n) || n < 0 || n > static_cast<long>(INT_MAX) >>> vérifie 3 choses : - - nombre positif - pas trop grand  > évite les dépassements de int.
//--si erreur lance une erreur
//leftover >>> vérifié qu'il n'y a rien en trop (ex. "42abb")
//--si le texte reste on throw une erreur
//on ajoute des conteneurs
//static_cast >>> converit long en int
//_vec.empty() >>> si aucun nombre n'a été donné throw erreur
void PmergeMe::parse(int argc, char** argv) 
{
    for (int i = 1; i < argc; i++) 
    {
        std::istringstream iss(argv[i]);
        long n;
        if (!(iss >> n) || n < 0 || n > static_cast<long>(INT_MAX))
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

// ─────────────────────────────────────────────────────────────────────────────
// Helper: Jacobsthal sequence ≥ n
//   Sequence: 1, 3, 5, 11, 21, 43, 85, ...   (t_{k} = t_{k-1} + 2*t_{k-2})
// ─────────────────────────────────────────────────────────────────────────────
static std::vector<std::size_t> buildJacobsthal(std::size_t n) 
{
    std::vector<std::size_t> jac;
    jac.push_back(1);
    jac.push_back(3);
    while (jac.back() < n + 2) {
        std::size_t sz = jac.size();
        jac.push_back(jac[sz - 1] + 2 * jac[sz - 2]);
    }
    return jac;
}

// ─────────────────────────────────────────────────────────────────────────────
// Ford-Johnson for std::vector<int>
// ─────────────────────────────────────────────────────────────────────────────
void PmergeMe::fjVec(std::vector<int> & arr) 
{
    //si 0 ou 1 élément on arrête
    if (arr.size() <= 1)
        return;

    // ── Step 0: handle odd element 
    //on vérifie si le nombre d'éléments est impair
    bool hasStraggler = (arr.size() % 2 != 0);
    
    //si impair, on garde le dernier nombre
    int  straggler    = hasStraggler ? arr.back() : 0;

    //on enlève ce dernier nombre pour l'instant
    if (hasStraggler) arr.pop_back();


    //--créer des paires
    //on calcule combien de paires on peut faire ex. [5, 2, 8, 3] > 2 paires
    std::size_t m = arr.size() / 2; // number of pairs

    
    // ── Step 1: sort each adjacent pair so arr[2i] ≥ arr[2i+1] ──────────────
   
   //on force chaque paire à être (grand à gauche, petit à droite). (2, 5) devient (5, 2)
    for (std::size_t i = 0; i < m; i++)
        if (arr[2 * i] < arr[2 * i + 1])
            std::swap(arr[2 * i], arr[2 * i + 1]);


    // ── Step 2: keep a copy of original pairs, extract larger elements ───────
    std::vector<std::pair<int, int> > origPairs(m);
    std::vector<int> larger(m);
    for (std::size_t i = 0; i < m; i++) 
    {
       //on garde les pairs originales prigPairs[i] = (grand, petit)
        origPairs[i] = std::make_pair(arr[2 * i], arr[2 * i + 1]);
        //on prend seulement les grands nombres larger [i] = grand
        larger[i]  = arr[2 * i];
    }

    // ── Step 3: recursively sort the larger elements (Ford-Johnson) 
    //on trie les grands(récursion) Ford Johnson. la fonction s'appelle elle même
    fjVec(larger);

    // ── Step 4: re-associate each sorted larger with its smaller partner ──────
    //   (O(n²) but correct; handles duplicates via "used" mask)

    //on remet chaque grand avec son petit????

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


    // ── Step 5: build main chain [b1, a1, a2, …, am] and pend [b2, …, bm] ───
    //   b1 ≤ a1 ≤ a2 ≤ … is guaranteed after the recursive sort.

    //on reconstruit la base mainChain (contient le premier petit et tous les grands triés)
    //pend : les élements à insérer 

    std::vector<int> mainChain;
    mainChain.reserve(m + 1);
    mainChain.push_back(sortedPairs[0].second); // b1
    for (std::size_t i = 0; i < m; i++)
        mainChain.push_back(sortedPairs[i].first); // a1 … am

    std::vector<int> pend;
    pend.reserve(m - 1);
    for (std::size_t i = 1; i < m; i++)
        pend.push_back(sortedPairs[i].second); // b2 … bm

    // ── Step 6: Jacobsthal-ordered binary insertion of pend elements ─────────
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

    // ── Step 7: insert straggler ───────────
    //si on avait un nombre seul, on l'insère à la fun correctement

    if (hasStraggler)
    {
        std::vector<int>::iterator pos =
            std::lower_bound(mainChain.begin(), mainChain.end(), straggler);
        mainChain.insert(pos, straggler);
    }

    //on remplace l'ancien tableau par le trié
    arr = mainChain;
}

// ─────────────────────────────────────────────────────────────────────────────
// Ford-Johnson for std::deque<int>
// (Same algorithm — intentionally written separately per subject requirements)
// ─────────────────────────────────────────────────────────────────────────────
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
    for (std::size_t i = 0; i < m; i++)
        mainChain.push_back(sortedPairs[i].first);

    std::vector<int> pend;
    pend.reserve(m - 1);
    for (std::size_t i = 1; i < m; i++)
        pend.push_back(sortedPairs[i].second);

    if (!pend.empty()) {
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

    if (hasStraggler) {
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

void PmergeMe::sort() 
{
    // Print unsorted sequence
    std::cout << "Before:";
    for (std::size_t i = 0; i < _vec.size(); i++)
        std::cout << " " << _vec[i];
    std::cout << std::endl;

    // Sort std::vector and measure time
    // on note le temps actuel
    std::clock_t startVec = std::clock();
    fjVec(_vec);
    std::clock_t endVec = std::clock();

    double timeVec = static_cast<double>(endVec - startVec) /
                     CLOCKS_PER_SEC * 1000000.0;

    // Sort std::deque and measure time
    std::clock_t startDeq = std::clock();
    fjDeq(_deq);
    std::clock_t endDeq = std::clock();
    double timeDeq = static_cast<double>(endDeq - startDeq) /
                     CLOCKS_PER_SEC * 1000000.0;
                
    // Print sorted sequence
    std::cout << "After: ";
    for (std::size_t i = 0; i < _vec.size(); i++)
        std::cout << " " << _vec[i];
    std::cout << std::endl;

    // Print timings

    std::cout << std::fixed << std::setprecision(5);
    std::cout << "Time to process a range of " << _vec.size()
              << " elements with std::vector : " << timeVec << " us" << std::endl;
    std::cout << "Time to process a range of " << _deq.size()
              << " elements with std::deque  : " << timeDeq << " us" << std::endl;
}
