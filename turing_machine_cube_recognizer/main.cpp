#include "TuringMachineTape.h"
#include <iostream>

int main() {
    TuringMachineTape input_tape{&std::cin};
    TuringMachineUtility::insertBegin(&input_tape);

    TuringMachineTape count_tape{};
    TuringMachineUtility::insertBegin(&count_tape);
    count_tape.write('x');

    TuringMachineTape incrementer_tape{};
    TuringMachineUtility::insertBegin(&incrementer_tape);

    while(true) {
        while(incrementer_tape.read() != BLANK_SYMBOL) {
            TuringMachineUtility::shiftAndInsert(&count_tape, 'x');
            TuringMachineUtility::shiftAndInsert(&count_tape, 'x');
            TuringMachineUtility::shiftAndInsert(&count_tape, 'x');
            TuringMachineUtility::shiftAndInsert(&count_tape, 'x');
            TuringMachineUtility::shiftAndInsert(&count_tape, 'x');
            TuringMachineUtility::shiftAndInsert(&count_tape, 'x');

            incrementer_tape.right();
        }
        TuringMachineUtility::shiftAndInsert(&incrementer_tape, 'y');
        TuringMachineUtility::rewind(&count_tape);
        TuringMachineUtility::rewind(&incrementer_tape);

        while(true) {
            input_tape.right();
            count_tape.right();
            if (input_tape.read() == BLANK_SYMBOL && count_tape.read() != BLANK_SYMBOL) {
                TuringMachineTape::reject();
                return 0;
            } else if (count_tape.read() == BLANK_SYMBOL && input_tape.read() != BLANK_SYMBOL) {
                // through what needs done here, need to increment the count_tape
                break;
            } else if (count_tape.read() == BLANK_SYMBOL && input_tape.read() == BLANK_SYMBOL) {
                // Both ended on BLANK_SYMBOL at the same time, so it is a cube
                TuringMachineTape::accept();
                return 0;
            }
        }
        TuringMachineUtility::rewind(&count_tape);
    }
}
