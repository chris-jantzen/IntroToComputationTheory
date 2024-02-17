#ifndef TM_TAPE_H
#define TM_TAPE_H

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>

#define BLANK_SYMBOL -1
#define BEGIN_SYMBOL 1
#define UTILITY_SYMBOL -9999

class TuringMachineTape {
public:
    // construct a tape with input read from the first line of the given
    // stream (or no input at all)
    TuringMachineTape(std::istream *input=NULL) : tape(1, BLANK_SYMBOL) {
        if (input) {
            std::string line;
            getline(*input, line);
            tape.insert(tape.begin(), line.begin(), line.end());
        }
        position = 0;
    }

    // return the symbol at the head
    int read() const { return tape[position]; }

    // write given symbol at the head
    void write(int symbol) { if (symbol == BLANK_SYMBOL) reject(); tape[position] = symbol; }

    // move right / left
    void right() { ++position; tape.resize(std::max(tape.size(), position + 1), BLANK_SYMBOL); }
    void left() { if (position <= 0) reject(); --position; }

    // dump the contents of the tape
    void debug(std::ostream *out=&std::cout) const {
        std::vector<size_t> widths;
        for (auto i : tape) {
            std::ostringstream term;
            if (i >= 0 && isprint(i)) {
                term << (char)i;
            } else {
                term << "<" << i << ">";
            }
            widths.push_back(term.str().length());
            *out << "| " << term.str() << " ";
        }
        *out << "|" << std::endl;

        for (size_t i = 0; i < tape.size(); ++i) {
            *out << "| " << std::setw(widths[i]) << ((i == position) ? '^' : ' ') << " ";
        }
        *out << "|" << std::endl;

        *out << '\n';
    }

    // accept / reject special methods
    static void accept() { std::cout << "accept" << std::endl; exit(0); }
    static void reject() { std::cout << "reject" << std::endl; exit(0); }

private:
    std::vector<int> tape;
    size_t position;
};


// Utility functions -- Use these and examine them to see how they work.
class TuringMachineUtility {
public:
    // seek to the left until we find the given symbol
    static void findLeft(TuringMachineTape *t, int symbol) {
        while (t->read() != symbol)
            t->left();
    }

    // seek to the right until we find the given symbol (or blank, causing
    // us to reject)
    static void findRight(TuringMachineTape *t, int symbol) {
        while (t->read() != symbol && t->read() != BLANK_SYMBOL)
            t->right();
        if (t->read() != symbol)
            t->reject();
    }

    // go left, back to BEGIN_SYMBOL, then advance one
    static void rewind(TuringMachineTape *t) {
        findLeft(t, BEGIN_SYMBOL);
        t->right();
    }

    // open a space at the current head position by shifting everything to
    // the right by one, then write the given symbol at that position.
    static void shiftAndInsert(TuringMachineTape *t, int symbol) {
        int saved = t->read();
        t->write(UTILITY_SYMBOL);
        while (saved != BLANK_SYMBOL) {
            t->right();
            int savedNext = t->read();
            t->write(saved);
            saved = savedNext;
        }

        findLeft(t, UTILITY_SYMBOL);
        t->write(symbol);
        t->right();
    }

    // insert the special BEGIN_SYMBOL at the current position of the head
    static void insertBegin(TuringMachineTape *t) {
        shiftAndInsert(t, BEGIN_SYMBOL);
    }
};

#endif