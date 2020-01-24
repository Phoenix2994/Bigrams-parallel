#ifndef GRAMS_COMPUTING_H
#define GRAMS_COMPUTING_H

#include <string>

#define NGRAM_LENGTH 2

class GramsComputing {

public:
    static void shiftArrayOfStrings(std::string *array);

    static bool computeLetters(char a, char *group, int &index);

    static bool computeWords(char a, std::string *group, std::string &tmp_string, int &index);
};

#endif //GRAMS_COMPUTING_H
