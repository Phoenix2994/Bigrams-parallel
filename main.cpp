#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
#include <iomanip>
#include <unordered_map>
#include <omp.h>
#include <cmath>

#include "grams_computing.h"

using namespace std;

static const int num_threads = 10;

class Worker {
public:
    Worker(unordered_map<string, int> *letters_hashtable, unordered_map<string, int> *words_hashtable,
           const long int *positions,
           const char *buffer, int tid) : letters_hashtable(letters_hashtable), words_hashtable(words_hashtable),
                                          positions(positions), buffer(buffer), tid(tid) {};

    void operator()() {
        doWork();
    }

protected:
    void doWork() {
        std::string ngram[NGRAM_LENGTH], words_ngram;
        int index = 0, letters_index = 0;
        char next_char, letters_ngram[NGRAM_LENGTH + 1];
        long int start_position = positions[tid];
        long int end_position = positions[tid + 1];
        std::string tmp_string;

        for (int i = start_position; i < end_position; i++) {
            next_char = buffer[i];
            if (GramsComputing::computeWords(next_char, ngram, tmp_string, index)) {
                words_ngram = ngram[0] + " " + ngram[1];
                words_hashtable[tid][words_ngram] += 1;
                if (!(isalpha(next_char) || isspace(next_char)))
                    index = 0;
                else {
                    index = NGRAM_LENGTH - 1;
                    GramsComputing::shiftArrayOfStrings(ngram);
                }
            }

            if (GramsComputing::computeLetters(next_char, letters_ngram, letters_index)) {
                letters_hashtable[tid][letters_ngram] += 1;
                letters_ngram[0] = letters_ngram[1];
                letters_index = 1;
            }
        }
    }

private:
    unordered_map<string, int> *letters_hashtable;
    unordered_map<string, int> *words_hashtable;
    const long int *positions;
    const char *buffer;
    int tid;
};


int main(int argc, char *argv[]) {
    const char *kInputPath = argv[1];
    const char *kOutputPath = argv[2];
    ifstream input, size;
    ofstream output_words, output_letters;
    string line, ngram;
    char next_char;
    char *buffer;
    long int tmp_position;
    long int positions[num_threads + 1];
    unordered_map<string, int> letters_hashtable[num_threads], words_hashtable[num_threads];
    std::unordered_map<std::string, int> letters_reduce, words_reduce;

    //start time
    double start = omp_get_wtime();

    //get file size
    size.open(kInputPath, ios::ate);
    const long int file_size = size.tellg();
    size.close();

    //read file
    buffer = new char[file_size];
    input.open(kInputPath, ios::binary);
    input.read(buffer, file_size);
    input.close();

    std::thread threads[num_threads];

    positions[0] = 0;

    for (int i = 1; i < num_threads + 1; i++) {
        if (i == num_threads)
            positions[i] = file_size - 1;
        else {
            tmp_position = floor((file_size - 1) / num_threads) * i;
            next_char = buffer[tmp_position - 1];
            if (isalpha(next_char) || isspace(next_char)) {
                do {
                    next_char = buffer[tmp_position];
                    tmp_position++;
                } while (isalpha(next_char) || isspace(next_char));
            }
            tmp_position++;
            positions[i] = tmp_position;
        }
    }

    for (int i = 0; i < num_threads; i++) {
        Worker w(letters_hashtable, words_hashtable, positions, buffer, i);
        threads[i] = std::thread(w);
    }

    for (auto &thread : threads)
        thread.join();

    /* REDUCE STEP */
    for (int i = 0; i < num_threads; i++) {
        for (const auto &r : letters_hashtable[i])
            letters_reduce[r.first] += r.second;
        for (const auto &r : words_hashtable[i])
            words_reduce[r.first] += r.second;
    }

    //end time
    double elapsedTime = omp_get_wtime() - start;

    /* PRINT RESULTS */
    output_words.open((std::string) kOutputPath + "parallel_output_words.txt", std::ios::binary);
    output_letters.open((std::string) kOutputPath + "parallel_output_letters.txt", std::ios::binary);

    for (auto &mapIterator : letters_reduce)
        output_letters << mapIterator.first << " : " << mapIterator.second << std::endl;
    output_letters << letters_reduce.size() << " elements found." << std::endl;

    for (auto &mapIterator : words_reduce)
        output_words << mapIterator.first << " : " << mapIterator.second << std::endl;
    output_words << words_reduce.size() << " elements found." << std::endl;

    output_words.close();
    output_letters.close();

    cout << elapsedTime;

    return 0;
}