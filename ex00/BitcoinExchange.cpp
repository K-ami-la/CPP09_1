#include "BitcoinExchange.hpp"
#include <fstream> //lire fichier (data.csv, input.txt)
#include <sstream> //manipuler du texte(pas bcp utilisé ici)
#include <iostream> //affiche(cout, cerr)
#include <cstdlib> //convertir texte (atoi, strtod)
#include <stdexcept> //lancer des erreurs (throw runtime_error)

BitcoinExchange::BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const BitcoinExchange& other) : _db(other._db) {}

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& other) {
    if (this != &other)
        _db = other._db;
    return *this;
}

BitcoinExchange::~BitcoinExchange() {}




//vérifier si "YYYY-MM-DD" est valide
//return true par défaut (sinon false)

bool BitcoinExchange::isValidDate(const std::string& date) const 
{
    if (date.size() != 10)
        return false;
    if (date[4] != '-' || date[7] != '-')
        return false;

    for (int i = 0; i < 10; i++) 
    {
        //on saute les tirets
        if (i == 4 || i == 7) 
        continue;
        if (date[i] < '0' || date[i] > '9')
            return false;
    }


    //on convertit en nbre "2021" devient 2021
    int year  = std::atoi(date.substr(0, 4).c_str());
    int month = std::atoi(date.substr(5, 2).c_str());
    int day   = std::atoi(date.substr(8, 2).c_str());

    //errors date inexistantes
    if (year < 0   || month < 1 || month > 12 || day < 1)
        return false;

    //commence with january
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
//     //!!💡 règle :

// Une année est bissextile si :

// divisible par 4 ✔️
// MAIS pas par 100 ❌
// sauf si divisible par 400 ✔️

    if (isLeap)  daysInMonth[1] = 29;


    if (day  > daysInMonth[month - 1])
        return false;

    return true;
}   

// Résumé global loadDatabase

// Cette fonction :

//Ouvre le fichier CSV
// Ignore le header
// Lit chaque ligne
// Sépare date et valeur
// Convertit la valeur en double
// Stocke dans une map


//Exemple final dans _db

// apres lecture :

// _db = {
//     {"2011-01-03", 0.3},
//     {"2011-01-04", 0.32},
//     {"2011-01-05", 0.35}
// }

// lit data.csv et stocke les données (dates et taux de bitcoin) et stock dans _db (std::map<std::string, double>
void BitcoinExchange::loadDatabase(const std::string& filename) 
{
    //open the file (to read it)
    //converts std::string in const char*
    std::ifstream file(filename.c_str());
    if (!file.is_open())
        throw std::runtime_error("Error: could not open database.");

    std::string line;

    //skip le header
    std::getline(file, line);

    //lit les  lines
    while (std::getline(file, line)) 
    {
        //ignore the empty lines
        if (line.empty()) continue;

        //we search the , if nope continue
        std::size_t sep = line.find(',');
        if (sep == std::string::npos) continue;
        
        //separate date and value
        std::string date    =  line.substr(0, sep);
        std::string rateStr  =  line.substr(sep + 1);

        //checks if the is empty
        if (date.empty() || rateStr.empty()) continue;

        //conversion in double + stockage
        //std::strtod >> converts string in double
        //NULL >>> on ignore la position de fin
        //example: "0.3" → 0.3 (double)
        //stockage : _db["2011-01-03"] = 0.3;

        _db[date] = std::strtod(rateStr.c_str(), NULL);
    }
}



//get the bitcoin exchange rate for certain date from the _db
//prend une date en string("YYY-MM-DD") 
//et retourne un double (le taux BTC ) cette date
//const :garantit que : _db n’est pas modifié
//méthode de la classe BitcoinExchange

double      BitcoinExchange::getRate(const std::string & date) const 
{

    //la fonction est const >> (écrit à la fin de la signature) >> _db ne peut pas être modifié
    //va pointer (créer un itérateur (de type const) qui va pointer) vers la position de la map
    //retourne un iterator vers la première clé > = date
    //ex.lower_bound("2011-01-03") méthode de std_map. retourne un itérator.
    //
    std::map<std::string, double>::const_iterator it = _db.lower_bound(date);


    //if the exact date doens't exist
    //_db.end après le dernier élément = en dehors de la map (plus grande que toutes les dates)
    //!!! std::map<std::string, double> >> chaque élément est pair<clé, valeur>. it->first   → "2011-01-03"   (clé). it->second  → 0.3 (valeur) 

    if (it == _db.end() || it->first != date) 
    {
        //tout premier élement
        //evite crash car on itère juste après
        if (it == _db.begin())
            throw std::runtime_error("Error: date out of range.");
        --it;
    }

    //le prix BTC
    return it->second;
}


//prend (le nom du fichier) en paramètre filename et ne retourne rien
//va lire le fichier et afficher les résultats


// vérifier format
// extraire date + valeur
// valider date
// convertir valeur
// vérifier erreurs
// récupérer prix BTC
// calculer
// afficher

void BitcoinExchange::processInput(const std::string& filename) 
{
    //Crée un objet (flux d’entrée fichier (ifstream))
    //c_str : conversion en const char*
    std::ifstream file(filename.c_str());
    //l'ouvre
    //.c_str() convertit std::string en chaîne C
    if (!file.is_open())
        throw std::runtime_error("Error: could not open file.");

    //déclare une variable line pour stocker chaque ligne du fichier
    std::string line;

    //lit la 1re ligne mais la skip (header date | value)
    std::getline(file, line); 

    //Lit chaque ligne (à partir de la 2e ligne) du fichier jusqu’à la fin
    //chaque ligne est stockée dans line
    while (std::getline(file, line)) 
    {
        if (line.empty()) continue;

        // Expect format: "date | value"
        std::size_t sep = line.find(" | ");
        
        //npos = "pas trouvé"
        //std::string::npos = valeur spéciale
        
        if (sep == std::string::npos) 
        {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }


        std::string date     = line.substr(0, sep);
        std::string valueStr = line.substr(sep + 3);

        if (!isValidDate(date)) 
        {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }


        //pour vérifier que toute la string est un nombre valide
        char* endPtr;

        //strdot convertit std::string en double et retourne via endPtr la position où la conversion s'arrête
        //strdot("42.5abc", &endPtr) 1- lit le nombre ( 42.5 (s'arrête là) 2-endPtr pointe là où la conversion s'est arrêté (a) 
        
        double value = std::strtod(valueStr.c_str(), &endPtr);
        
        //si aucun chiffre n'a été lu ex. "abc", ou il reste des caractères après le nombre (ex. 4 2 . 5 a b c \0)
        //on accèpte seulement "42" "42.5" !!! pas "42abc" "abc" "42"
        if (endPtr == valueStr.c_str() || *endPtr != '\0')
        {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }

        if (value < 0) 
        {
            std::cerr << "Error: not a positive number." << std::endl;
            continue;
        }
        if (value > 1000) 
        {
            std::cerr << "Error: too large a number." << std::endl;
            continue;
        }

        //sert à gérer les erreurs qui peuvent arriver dans getRate
        try 
            {
                double rate   = getRate(date);
                double result = value * rate;
                std::cout << date << " => " << value << " = " << result << std::endl;
            } 
        catch (const std::exception& e) 
        {
            std::cerr << e.what() << std::endl;
        }
    }
}

