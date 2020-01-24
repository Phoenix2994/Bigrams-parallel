#include "grams_computing.h"

void GramsComputing::shiftArrayOfStrings(std::string *array) {
    for (int i = 0; i < NGRAM_LENGTH - 1; i++) {
        array[i] = array[i + 1];
    }
}

bool GramsComputing::computeLetters(char a, char *group, int &index) {
    if (isalpha(a)) {
        if (isupper(a))
            a = tolower(a);
        group[index] = a;
        index++;
    } else
        index = 0;
    return (index == NGRAM_LENGTH);
}

bool GramsComputing::computeWords(char a, std::string *group, std::string &tmp_string, int &index) {

    if (isalpha(a)) {
        if (isupper(a))
            a = tolower(a);
        tmp_string += a;
    } else {
        if (a == '\'')
            tmp_string += a;
        if (!(!(isalpha(a) || isspace(a)) && (index + 1) < NGRAM_LENGTH) && !tmp_string.empty()) {
            group[index] = tmp_string;
            index++;
        } else
            index = 0;
        tmp_string = "";
    }
    return (index == NGRAM_LENGTH);
}
