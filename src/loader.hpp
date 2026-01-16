#ifndef loader_hpp
#define loader_hpp

#include <string>

class Term;

class Loader{
public:
    static Term loadTerm(std::string fileName);
};

#endif