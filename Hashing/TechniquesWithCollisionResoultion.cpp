#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>

using namespace std;

class HashTable
{
private:
    int TableSize;
    vector<int> table;
    vector<vector<int>> ChainTable;
    string HashingTechnique;
    string CollisionResolution;
    string SecondHashTechnique;

    int DivisionMethod(int key)
    {
        return key % TableSize;
    }

    int MultiplicationMethod(int key)
    {
        double a = 0.545;
        double fractionalPart = fmod(key * a, 1);
        return floor(TableSize * fractionalPart);
    }

    int MidSquareMethod(int key)
    {
        int square = key * key;
        string SquareStr = to_string(square);
        int NumDigits = SquareStr.length();
        int MiddleDigits = 0;

        if (NumDigits == 3)
        {
            MiddleDigits = SquareStr[1] - '0';
        }

        else if (NumDigits > 3)
        {
            int middle = NumDigits / 2;
            int left = middle - 1;
            MiddleDigits = stoi(SquareStr.substr(left, 2));
        }

        else
        {
            MiddleDigits = square;
        }

        return MiddleDigits % TableSize;
    }

    int FoldingMethod(int key, int AddressSpace)
    {
        int sum = 0;
        int digits = pow(10, AddressSpace);

        while (key > 0)
        {
            sum += key % digits;
            key /= digits;
        }
        return sum % TableSize;
    }

    int HashKey(int key)
    {
        if (HashingTechnique == "Division")
            return DivisionMethod(key);

        else if (HashingTechnique == "Multiplication")
            return MultiplicationMethod(key);

        else if (HashingTechnique == "MidSquare")
            return MidSquareMethod(key);

        else if (HashingTechnique == "Folding")
            return FoldingMethod(key, 2);

        return -1;
    }

    int SecondHash(int key)
    {
        int hash;
        if (SecondHashTechnique == "Division")
            hash = DivisionMethod(key);

        else if (SecondHashTechnique == "Multiplication")
            hash = MultiplicationMethod(key);

        else if (SecondHashTechnique == "MidSquare")
            hash = MidSquareMethod(key);

        else if (SecondHashTechnique == "Folding")
            hash = FoldingMethod(key, 2);

        else
            return 1;

        return (hash == 0) ? 1 : hash; // no-zero, if zero --> gessing infinite loop
    }

public:
    HashTable(int size, const string &hashTech, const string &collision, const string &secondHash = "")
        : TableSize(size), HashingTechnique(hashTech), CollisionResolution(collision), SecondHashTechnique(secondHash)
    {
        table.resize(TableSize, -1);
        ChainTable.resize(TableSize);
    }

    void insert(int key)
    {
        int index = HashKey(key);

        if (CollisionResolution == "Chaining")
        {
            ChainTable[index].push_back(key);
        }

        else
        {
            int i = 0, jump = 0;

            if (CollisionResolution == "DoubleHashing")
                jump = SecondHash(key);

            while (table[index] != -1 && table[index] != -2) // Skip deleted
            {
                if (CollisionResolution == "Linear")
                    index = (index + 1) % TableSize;

                else if (CollisionResolution == "Quadratic")
                    index = (index + i * i) % TableSize;

                else if (CollisionResolution == "DoubleHashing")
                    index = (index + i * jump) % TableSize;

                i++;
            }
            table[index] = key;
        }
    }

    bool search(int key)
    {
        int index = HashKey(key);

        if (CollisionResolution == "Chaining")
        {
            for (int val : ChainTable[index])
            {
                if (val == key)
                    return true;
            }
            return false;
        }
        else
        {
            int i = 0, jump = 0;

            if (CollisionResolution == "DoubleHashing")
                jump = SecondHash(key);

            while (table[index] != -1)
            {
                if (table[index] == key)
                    return true;

                if (CollisionResolution == "Linear")
                    index = (index + 1) % TableSize;

                else if (CollisionResolution == "Quadratic")
                    index = (index + i * i) % TableSize;

                else if (CollisionResolution == "DoubleHashing")
                    index = (index + i * jump) % TableSize;

                i++;
            }
            return false;
        }
    }

    void remove(int key)
    {
        int index = HashKey(key);

        if (CollisionResolution == "Chaining")
        {
            auto &chain = ChainTable[index];

            auto it = find(chain.begin(), chain.end(), key);

            if (it != chain.end())
                chain.erase(it);
        }
        else
        {
            int i = 0, jump = 0;

            if (CollisionResolution == "DoubleHashing")
                jump = SecondHash(key);

            while (table[index] != -1)
            {
                if (table[index] == key)
                {
                    table[index] = -2; // deleted flag
                    return;
                }

                if (CollisionResolution == "Linear")
                    index = (index + 1) % TableSize;

                else if (CollisionResolution == "Quadratic")
                    index = (index + i * i) % TableSize;

                else if (CollisionResolution == "DoubleHashing")
                    index = (index + jump) % TableSize;

                i++;
            }
        }
    }

    void display()
    {
        if (CollisionResolution == "Chaining")
        {
            for (int i = 0; i < TableSize; ++i)
            {
                cout << i << ": ";
                if (ChainTable[i].empty())
                {
                    cout << "NULL\n";
                }
                else
                {
                    for (int key : ChainTable[i])
                        cout << key << " -> ";
                    cout << "NULL\n";
                }
            }
        }
        else
        {
            for (int i = 0; i < TableSize; ++i)
            {
                if (table[i] == -1)
                    cout << i << ": NULL\n";

                else if (table[i] == -2)
                    cout << i << ": DELETED\n";

                else
                    cout << i << ": " << table[i] << "\n";
            }
        }
    }
};

int main()
{
    // Linear Probing Example
    cout << "\nHash Table with Linear Probing (Division Method):\n";
    HashTable htLinear(10, "Division", "Linear");
    htLinear.insert(12);
    htLinear.insert(22);
    htLinear.insert(42);
    htLinear.display();

    cout << "\nSearching for 22: " << (htLinear.search(22) ? "Found" : "Not Found") << "\n";
    cout << "Searching for 35: " << (htLinear.search(35) ? "Found" : "Not Found") << "\n";

    cout << "Deleting 22...\n";
    htLinear.remove(22);
    htLinear.display();
    cout << "Searching for 22 after deletion: " << (htLinear.search(22) ? "Found" : "Not Found") << "\n";

    // Quadratic Probing Example
    cout << "\nHash Table with Quadratic Probing (Multiplication Method):\n";
    HashTable htQuadratic(10, "Multiplication", "Quadratic");
    htQuadratic.insert(12);
    htQuadratic.insert(22);
    htQuadratic.insert(42);
    htQuadratic.insert(32); // Causes collision
    htQuadratic.display();

    cout << "\nSearching for 42: " << (htQuadratic.search(42) ? "Found" : "Not Found") << "\n";
    cout << "Deleting 42...\n";
    htQuadratic.remove(42);
    htQuadratic.display();
    cout << "Searching for 42 after deletion: " << (htQuadratic.search(42) ? "Found" : "Not Found") << "\n";

    // Double Hashing Example
    cout << "\nHash Table with Double Hashing (MidSquare & Division Methods):\n";
    HashTable htDouble(10, "MidSquare", "DoubleHashing", "Division");
    htDouble.insert(1234);
    htDouble.insert(5678);
    htDouble.insert(9101);
    htDouble.display();

    cout << "\nSearching for 1234: " << (htDouble.search(1234) ? "Found" : "Not Found") << "\n";
    cout << "Searching for 9101: " << (htDouble.search(9101) ? "Found" : "Not Found") << "\n";

    cout << "Deleting 1234...\n";
    htDouble.remove(1234);
    htDouble.display();
    cout << "Searching for 1234 after deletion: " << (htDouble.search(1234) ? "Found" : "Not Found") << "\n";

    return 0;
}
