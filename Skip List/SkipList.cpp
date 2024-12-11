#include <iostream>
#include <time.h>
#include <random>

// for the menu
#include <functional>

using namespace std;

template<class K, class V, class Rand, class Comp = less<K>>
class SkipList
{
    class Iterator;
    class Node
    {
        friend SkipList;
        friend Iterator;
        bool boundary;
        K key;
        V value;
        vector<Node *> next;
        Node(size_t levels)
        : boundary(true), next(levels){}
        Node(K key, V value, size_t levels)
        : boundary(false), key(key), value(value), next(levels){}
    };
    class Iterator
    {
        friend SkipList;
        Node *node;
        Iterator(Node *node): node(node){}

        public:
        K key(){ return node->key; }
        V value(){ return node->value; }
        Iterator operator++(int)
        { 
            Iterator tmp  = *this;
            node = node->next[0];
            return tmp;
        }
        Iterator operator++()
        { 
            node = node->next[0]; 
            return *this ;
        }
        bool operator==(Iterator &oth){ return node == oth.node; }
    };
    
    Node *head;
    size_t maxLevels, count;
    Rand rand;
    Comp comp;

    vector<Node *> trace(K key)
    {
        vector<Node *> pastNodes(maxLevels);
        Node *current = head;
        ssize_t level = maxLevels - 1;
        while (level >= 0)
        {
            if (current->next[level] == NULL || !comp(key, current->next[level]->key))
                pastNodes[level--] = current;
            else    
                current = current->next[level];
        }
        return pastNodes;
    }

    public:
    SkipList(size_t maxLevels)
    : maxLevels(maxLevels), head(new Node(maxLevels)), count(0){}
    bool exists(K key)
    { 
        Node *potNode = trace(key)[0]->next[0];
        return potNode != NULL && potNode->key == key;
    }
    V &operator[](K key)
    {
        if (!exists(key))
            insert(key, V());
        Node *potNode = trace(key)[0]->next[0];
        return potNode->value;
    }
    void insert(K key, V value)
    {
        if (exists(key))
            return;
        vector<Node *> pastNodes = trace(key);
        size_t levels = 1;
        while (levels < maxLevels && rand())
            levels++;

        Node *current = new Node(key, value, levels);

        for (size_t i = 0; i < levels; i++)
        {
            current->next[i] = pastNodes[i]->next[i];
            pastNodes[i]->next[i] = current;
        }
        count++;
    }
    void erase(K key)
    {
        if (!exists(key))
            return;
        vector<Node *> pastNodes = trace(key);
        Node *current = pastNodes[0]->next[0];

        for (size_t i = 0; i < current->next.size(); i++)
        {
            pastNodes[i]->next[i] = current->next[i];
        }
        delete current;
        count--;
    }
    size_t size(){ return count; }
    Iterator begin(){ return Iterator(head->next[0]); }
    Iterator end() { return Iterator(NULL); }
    void print()
    {
        for (ssize_t i = maxLevels - 1; i >= 0; i--)
        {
            Node *current = head;
            cout << "|";
            while (current->next[i] != NULL)
            {
                current = current->next[i];
                cout << " -> " << current->key;
            }
            cout << endl;
        }
    }
};

struct Rand
{
    bool operator()(){ return rand() % 2; }
};

struct Player
{
    string name;
    int score;
    Player(): score(INT_MAX){}
    Player(string name, int score)
    : name(name), score(score){}
    operator string(){ return name + ": " + to_string(score); }
};

int main()
{
    srand(time(0));
    SkipList<string, Player, Rand> playerNames(20);
    SkipList<int, Player, Rand> playerScores(20);
    vector<string> menu{
        "PLAYER MANAGEMENT SYSTEM",
        string(20, '='),
        "1) Add a player",
        "2) Remove a player",
        "3) Retrieve a player's score",
        "4) Change a player's score",
        "5) Retrieve the top N players",
        "q) Exit",
        string(20, '='),
    };
    bool cont = true;
    auto insert = [&](Player player){
        playerNames[player.name] = player;
        playerScores[player.score] = player;
    };
    auto erase = [&](Player player){
        playerNames.erase(player.name);
        playerScores.erase(player.score);
    };
    auto add = [&](){
        Player player;
        cout << "Player's name: ";
        cin >> player.name;

        if (playerNames.exists(player.name))
            return void(cout << "Player already exists");
        cout << "Player's score: ";
        cin >> player.score;
        insert(player);
    };
    auto remove = [&](){
        Player player;
        cout << "Player's name: ";
        cin >> player.name;
        if (!playerNames.exists(player.name))
            return void(cout << "Player not found");
        erase(playerNames[player]);
    };
    auto retrieve = [&](){
        Player player;
        cout << "Player's name: ";
        cin >> player.name;
        if (!playerNames.exists(player.name))
            return void(cout << "Player not found");
        cout << string(playerNames[player.name]);
    };
    auto changeScore = [&](){
        Player player;
        cout << "Player's name: ";
        cin >> player.name;
        if (!playerNames.exists(player.name))
            return void(cout << "Player not found");
        erase(playerNames[player.name]);
        cout << "New score: ";
        cin >> player.score;
        insert(player);
    };
    auto retrieveN = [&](){
        int n;
        cout << "N: ";
        cin >> n;
        n = min(size_t(n), playerNames.size());
        auto it = playerScores.begin();
        for (int i = 0; i < n; i++, it++)
            cout << string(it.value()) << endl;
    };
    auto quit = [&](){
        cont = false;
    };

    SkipList<char, function<void()>, Rand> functionMap(10);
    functionMap['1'] = add;
    functionMap['2'] = remove;
    functionMap['3'] = retrieve;
    functionMap['4'] = changeScore;
    functionMap['5'] = retrieveN;
    functionMap['q'] = quit;

    while (cont)
    {
        for (size_t i = 0; i < menu.size(); i++)
            cout << menu[i] << endl;

        cout << "Operation: ";
        int op;
        cin >> op;
        if (!functionMap.exists(op))
            cout << "Unknown operation";
        else
            functionMap[op]();
        cout << endl;
    }
}