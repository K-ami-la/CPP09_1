#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <map>
#include <string>

class BitcoinExchange 
{
    public:
        BitcoinExchange();
        BitcoinExchange(const BitcoinExchange& other);
        BitcoinExchange& operator=(const BitcoinExchange& other);
        ~BitcoinExchange();

        //btc.loadDatabase("data.csv");
        //stocke tous les prix du bitcoin dans la mémoire (_db["2011-01-03"] = 0.3;)
        void loadDatabase(const std::string& filename);//lit un fichier CSV (ac ifstream file()); ignore le header(ac getline);lire chaque ligne(ac getline aussi); trouve la virgule(line.find); découper(date = ; rateStr = ); stocker( _db[date] = std::strtod.(...)
    
        //btc.processInput("input.txt");
        void processInput(const std::string& filename);

    private:
            std::map<std::string,  double> _db;

    bool        isValidDate(const  std::string&  date) const;//verifie si une date est correcte(taille; les tirets; le reste sont des chiffres, transforme en nbres (atoi), mois valides, gère les mois, années bissextile, vérifie le jour(30 février >> faux))
    double      getRate(const  std::string&  date) const;//trouve le taux. trouve la date exacte OU la 1re date AVANT!!! (it = _db.lower_bound(date);). cas erreur (pas de date avant)
};

#endif
