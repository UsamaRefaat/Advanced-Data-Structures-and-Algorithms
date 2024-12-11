#include <iostream>

using namespace std;

template<class T>
class RBTree
{
    struct Node
    {
        bool red;
        T value;
        Node *par;
        Node *left, *right;

        public:
        Node(T value, bool red, Node *par)
        : Node(value, red, par, NULL, NULL){}
        Node(T value, bool red, Node *par, Node *left, Node *right)
        : value(value), red(red), par(par), left(left), right(right){}
        void flip(){ red = !red; }
        Node*& next(T element){ return element < value ? left : right; }
        Node* sibling(){ return isLeftChild() ? par->right : par->left; }
        Node* uncle(){ return par->sibling(); }
        Node* grandParent(){ return par->par; }
        bool isLeftChild(){ return par->left == this; }
        bool isRightChild(){ return !isLeftChild(); }
        bool leftChildCol(){ return left && left->red; }
        bool rightChildCol(){ return right && right->red; }
    };
    Node *root;
    size_t count = 0;

    void rotateLeft(Node *node)
    {
        Node *other = node->right;
        node->right = other->left;
        if (node->left != NULL)
            node->left->par = node;
        
        other->par = node->par;
        if (node->par == NULL)
            root = other;
        else if (node->isLeftChild())
            node->par->left = other;
        else
            node->par->right = other;
        
        other->left = node;
        node->par = other;
    }
    void rotateRight(Node *node)
    {
        Node *other = node->left;
        node->left = other->right;
        if (node->left != NULL)
            node->left->par = node;

        other->par = node->par;
        if (node->par == NULL)
            root = other;
        else if (node->isLeftChild())
            node->par->left = other;
        else
            node->par->right = other;

        other->right = node;
        node->par = other;
    }
    void rotate(Node *node, bool dir){ (dir ? rotateRight(node) : rotateLeft(node)); }

    void fixInsert(Node *node)
    {
        Node *parent, *grand, *uncle;
        while (node->par && node->red && node->par->red)
        {
            parent = node->par;
            grand = node->grandParent();
            uncle = node->uncle();
            if (uncle != NULL && uncle->red)
            {
                parent->flip();
                uncle->flip();
                grand->flip();
                node = grand;
            }
            else
            {
                if (parent->isLeftChild())
                {
                    if (node->isRightChild())
                        rotateLeft(parent), swap(node, parent);
                    rotateRight(grand);
                    swap(parent->red, grand->red);
                    node = parent;
                }
                else
                {
                    if (node->isLeftChild())
                        rotateRight(parent), swap(node, parent);
                    rotateLeft(grand);
                    swap(parent->red, grand->red);
                    node = parent;
                }
            }
        }
        root->red = 0;
    }
    void fixErase(Node *node)
    {
        Node *parent, *sibling;
        while (node->par != NULL && !node->red)
        {
            parent = node->par;
            sibling = node->sibling();

            if (sibling->red)
            {
                swap(sibling->red, parent->red);
                rotate(parent, node->isRightChild());
            }
            else if (!sibling->leftChildCol() && !sibling->rightChildCol())
            {
                sibling->red = 1;
                node = parent;
            }
            else
            {
                Node *near = sibling->left, *far = sibling->right;
                if (node->isRightChild())
                    swap(near, far);
                if (near && near->red)
                {
                    swap(sibling->red, near->red);
                    rotate(sibling, !node->isRightChild());
                }
                else
                {
                    far->red = 0;
                    swap(sibling->red, parent->red);
                    rotate(parent, node->isRightChild());
                    break;
                }
            }
        }
        node->red = 0;
    }

    Node *find(T element)
    {
        Node *node = root;
        while (node != NULL && node->value != element)
            node = node->next(element);
        return node;
    }
    Node *diveLeft(Node *node)
    {
        while (node->left != NULL)
            node = node->left;
        return node;
    }
    Node *diveRight(Node *node)
    {
        while (node->right != NULL)
            node = node->right;
        return node;
    }
    
    void print(Node *root, Node *par, string indent, bool last)
    {
        if (root != nullptr) {
            cout << indent;
            if (last) {
                cout << "R----";
                indent += "   ";
            }
            else {
                cout << "L----";
                indent += "|  ";
            }

            string sColor
                = (root->red) ? "RED" : "BLACK";
            cout << root->value << "(" << sColor << ")"
                 << endl;
            print(root->left, root, indent, false);
            print(root->right, root, indent, true);
        }
    }

    public:
    RBTree(): root(NULL), count(0){}
    size_t size(){ return count; }
    bool empty(){ return count == 0; }
    bool exists(T element){ return find(element) != NULL; }
    void insert(T element)
    {
        if (exists(element))
            return;
        if (root == NULL)
            root = new Node(element, 0, NULL);
        else
        {
            Node *node = root;
            while (node->next(element) != NULL)
                node = node->next(element);
            node->next(element) = new Node(element, 1, node);
            fixInsert(node->next(element));
        }
        count++;
    }
    void erase(T element)
    {
        Node *node = find(element);
        if (node == NULL)
            return;
        if (node->left && node->right)
        {
            Node *sub = diveLeft(node->right);
            swap(node->value, sub->value);
            node = sub;
        }
        Node *child = (node->left ? node->left : node->right);
        bool col = node->red, childCol = (child && child->red);
        if (!col && !childCol)
            fixErase(node);

        if (node->isLeftChild())
            node->par->left = child;
        else
            node->par->right = child;

        if (child)
            child->par = node->par;

        delete node;
        node = child;

        if (node)
            node->red = col && childCol;
        count--;
    }
    void print(){ print(root, NULL, "", 1), cout << string(30, '=') << endl; }
};

int main()
{
    RBTree<char> rbtree;

    // Inserting values into Red-Black Tree
    rbtree.insert('M');
    rbtree.insert('T');
    rbtree.insert('F');
    rbtree.insert('Q');
    rbtree.insert('P');

    rbtree.print();

    // Deleting nodes from Red-Black Tree
    cout << "After deleting F:" << endl;
    rbtree.erase('F');
    rbtree.print();

    cout << "After deleting Q:" << endl;
    rbtree.erase('Q');
    rbtree.print();

    cout << "After deleting T:" << endl;
    rbtree.erase('T');
    rbtree.print();

    return 0;
}