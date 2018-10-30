#ifndef DISK
#define DISK

#include <map>

template <class Knoop>
class Disk: private std::map<unsigned int, Knoop>
{
    public:

        /* Public fields */
        const static unsigned int NULL_BLOKINDEX = 0;

        /* Constructors */
        Disk() : std::map<unsigned int, Knoop>(), counter(500) { };

        /* Functions */

        // Write T to disk
        // Return index to T
        unsigned int write(const Knoop& knoop) {
            
            counter++;
            // this->emplace(counter, knoop);
            this->operator[](counter) = knoop;
            return counter;
        }

        // Remove index
        void remove(unsigned int i) {
            this->erase(i);
        }

        // Write T to disk on specific index 
        void overwrite(const unsigned int i, const Knoop& knoop) {
            this->operator[](i) = knoop;
        }

        // Read Knoop from disk
        Knoop& read(const unsigned int i) {
            return this->operator[](i);
        }

    private:

        /* Private fields */
        unsigned int counter;
};

#endif