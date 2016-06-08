#ifndef BST_H
#define BST_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <limits>
#include <cmath>
#include "node.h"

using namespace std;

enum BST_TRAVERSAL_TYPES {IN_ORDER, PRE_ORDER, POST_ORDER};
enum BST_ERROR_TYPES {EMPTY};


template<typename T>
class bst
{
    public:
        typedef void (bst<T>::*fptr)(node<T>*,ostream &) const;
        //constructs given traversal type
        bst(BST_TRAVERSAL_TYPES  t = IN_ORDER);

        //constructs gives node data, count, and traversal type
        bst(const T &d, size_t = 1, BST_TRAVERSAL_TYPES t = IN_ORDER);
        //copy constructor
        bst(const bst<T> &other);
        //destructor
        ~bst();
        //assignment operator
        bst<T>& operator=(const bst<T> &other);


        //insert node
        void insert(const T &d, size_t c = 1, int syll = 0, int linenum = 0, int paragraph = 0);
        //output node
        bst<T>& operator<<(const T &d);
        //
        size_t find(const T &d);
        void clear();
        bool remove(const T &d, size_t c = 1);
        bool removeAll(const T &d);
        size_t totalData();
        size_t totalNodes();
        size_t depth();
        const T& leftMost() const;
        const T& rightMost() const;
        bool empty() const;
        bool balanced();
        void rebalance();
        void setTraversal(BST_TRAVERSAL_TYPES t);
        BST_TRAVERSAL_TYPES currentTraversal();

        void getTop();
        vector<node<T>*> getFreq();
        template<typename U>
        friend
        ostream& operator<<(ostream& out, const bst<U> &other);

        template<typename U>
        friend
        istream& operator>>(istream& in, bst<U> &other);


    private:
        node<T> *root;
        node<T>* findParent(node<T> *r, T d);
        node<T>* theMost(node<T>*r, WHICH_CHILD child) const;
        node<T>* balanceVine(node<T>* r, size_t nodeCount);
        size_t totalNodes(node<T> *r);
        void getTop(node<T> *r);
        vector<node<T>*> top;
        BST_TRAVERSAL_TYPES  traverse;
        fptr whatToDo[3];
        static WHICH_CHILD direction[2];

        void copy(node<T> *r);
        void nukem(node<T>* &r);
        void inOrder(node<T>* r, ostream &out) const;
        void preOrder(node<T>* r, ostream &out) const;
        void postOrder(node<T>* r, ostream &out) const;
        size_t depth(node<T> *r);     
        bool balanced(node<T> *r);
        void rebalance(node<T> *&r);

        void initialize(node<T> *n, BST_TRAVERSAL_TYPES  t );
        size_t totalDataItems(node<T> *r);
        node<T>*  leftRotate(node<T>* &r);
        node<T> *rightRotate(node<T>* &r);
        node<T>* createRightVine(node<T>* r);
        ostream& print(ostream &out = cout) const;



};

template<typename T>
WHICH_CHILD bst<T>::direction[2] = {RIGHT,LEFT};

//initalize given traversal type
template<typename T>

bst<T>::bst(BST_TRAVERSAL_TYPES  t )
{
    initialize(NULL,t);
    for(int i = 0;i<10;i++)
        top.push_back(new node<T>("0",0,0,0,0));
}

//initialize by calling node constructor with data and count and then traversal type
template<typename T>
bst<T>::bst(const T &d, size_t c, BST_TRAVERSAL_TYPES t)
{
    initialize(new node<T>(d,c),t);
}

//copy constructor
template<typename T>
bst<T>::bst(const bst<T> &other)
{
    copy(other.root);
}

//destructor
template<typename T>
bst<T>::~bst()
{
    nukem(root);
}

//assignment operator
template<typename T>
bst<T>& bst<T>::operator=(const bst<T> &other)
{
    if(this != &other)
    {
        nukem(root);
        copy(other.root);
    }
    return *this;
}


//
template<typename T>
void bst<T>::insert(const T &d, size_t c, int syll, int linenum, int paragraph)
{

    node<T> *temp;
    bool notDone = true;
    //set value to root if tree is empty
    if(!root)
    {

        root = new node<T>(d,c,syll, linenum, paragraph);
    }
    else
    {

        temp = root;
        while(notDone)
            //if temp data is same as arg, then increment count
            if(d == *temp)
            {
                (*temp) += c;
                temp->addLine(linenum);
                temp->addParagraph(paragraph);
                notDone = false;
            }
            //otherwise
            else
            {
                //compare if it belongs to right or left subtree

                WHICH_CHILD child = direction[d < temp->getData()];
                //if there is no child, then create new node
                if(!temp->childPtr(child))
                {
                    temp->childPtr(child) = new node<T>(d,c,syll, linenum, paragraph);
                    notDone = false;
                }
                //otherwise set the temp to child and recompare
                else
                    temp = temp->childPtr(child);
            }
    }

    if(!balanced(root))
    {

      temp = findParent(root, d);

        while (!balanced(temp))
        {
            rebalance(temp);

            temp = findParent(root,temp->getData());

        }
    }


}

//stackable insertion operator
template<typename T>
bst<T>& bst<T>::operator<<(const T &d)
{
    //insert data
    insert(d);
    return *this;
}

//locate data
//return count (used for conditional; if count >= 1, then it has been found)
template<typename T>
size_t bst<T>::find(const T& d)
{
    //begin at root
    node<T> *ptr = root;
    //traverse while not found and there are still nodes , ie ptr data is not equal to arg
    while(ptr && *ptr != d)
        //ptr is equal to child based on comparison with pointer data
        ptr = ptr->childPtr(direction[ d < *ptr]);
    //ptr is not null, then return the count, otherwise return 0
    return ptr ? ptr->getCount() : 0;
}

template<typename T>
void bst<T>::clear()
{
    nukem(root);
}

template<typename T>
bool bst<T>::remove(const T &d, size_t c)
{
    static node<T> temp;
    temp.childPtr(LEFT) = temp.childPtr(RIGHT) = root;
    node<T> *parent = findParent(&temp, d),
            *bye,
            *leftMost;
    if(!parent)
        return false;
    WHICH_CHILD child = direction[d < *parent];
    bye = parent->childPtr(child);
    *bye -= c;
    if( bye->getCount() < 1)
    {
           //delete node if there are no children
           if(!bye->childPtr(LEFT) && !bye->childPtr(RIGHT))
           {
               delete bye;
               parent->childPtr(child) = NULL;
           }
           //if there is left or right node, make parent point to right/left child
           else if (!bye->childPtr(LEFT))
           {
               parent->childPtr(child) = bye->childPtr(RIGHT);
               delete bye;
           }
           else if (!bye->childPtr(RIGHT))
           {
               parent->childPtr(child) = bye->childPtr(LEFT);
               delete bye;
           }
           //if there are children, replace node with left most node value and delete original
           else
           {
               leftMost = theMost(bye->childPtr(RIGHT), LEFT);
               T theData = leftMost->getData();
               int theCount = leftMost->getCount();
               remove(theData, theCount);
               bye->setNodeValue(theData, theCount);
           }

    }
    return true;
}

template<typename T>
bool bst<T>::removeAll(const T &d)
{
    return remove(d,UINT_MAX);
}

template<typename T>
size_t bst<T>::totalData()
{
    return totalDataItems(root);
}

//count total number of nodes
template<typename T>
size_t bst<T>::totalNodes()
{
    return totalNodes(root);
}

template<typename T>
size_t bst<T>::depth()
{
    return root ? 1 + max(depth(root->rightChild()), depth(root->leftChild())) : 0;
}

template<typename T>
const T& bst<T>::leftMost() const
{
    node<T>* temp = theMost(root,LEFT);
    return temp ? temp->getData() : 0 ;
}

template<typename T>
const T& bst<T>::rightMost() const
{
    node<T>* temp = theMost(root, RIGHT);
    return temp ? temp->getData() : 0 ;
}

template<typename T>
bool bst<T>::empty() const
{
    return !root;
}

template<typename T>
bool bst<T>::balanced()
{
   return balanced(root);
}
template<typename T>
void bst<T>::rebalance()
{
    rebalance(root);
}

template<typename T>
void bst<T>::rebalance(node<T> *&r)
{
    //cout<<"reblancing "<<*r<<endl;
//    size_t count = totalNodes();
//    root = createRightVine(root);
//    root = balanceVine(root,count);
    int heightDifference =  depth(r->childPtr(LEFT)) - depth(r->childPtr(RIGHT));
    //cout<<heightDifference<<endl;
    if(heightDifference > 1)
    {
        //LEFT LEFT
        if(depth(r->childPtr(LEFT)->childPtr(LEFT)) >= depth(r->childPtr(LEFT)->childPtr(RIGHT)))
        {
            //cout<<"a"<<endl;
            //cout<<"r before rotation: "<<*r<<endl;
            r = rightRotate(r);
            //cout<<"r after rotation: "<<*r<<endl;
            //cout<<r->childPtr(LEFT)<<endl;
            //cout<<r->childPtr(RIGHT)<<endl;
            //cout<<"root is still "<<*root<<endl;
        }
        //LEFT RIGHT
        else
        {
            //cout<<"b"<<endl;
            r->childPtr(LEFT) = leftRotate(r->childPtr(LEFT));
            r = rightRotate(r);
        }


    }
    else if (heightDifference < -1)
    {
        //RIGHT RIGHT
        if(depth(r->childPtr(RIGHT)->childPtr(RIGHT)) >= depth(r->childPtr(RIGHT)->childPtr(LEFT)))
        {
            //cout<<"c"<<endl;
            r = leftRotate(r);
        }
        //RIGHT LEFT
        else
        {
            //cout<<"d"<<endl;
            r->childPtr(RIGHT) = rightRotate(r->childPtr(RIGHT));
            r = leftRotate(r);
        }
    }

}

template<typename T>
void bst<T>::setTraversal(BST_TRAVERSAL_TYPES t)
{
    traverse = t;
}

template<typename T>
BST_TRAVERSAL_TYPES bst<T>::currentTraversal()
{
    return traverse;
}


template<typename T>
void bst<T>::copy(node<T> *r)
{
    if(!r)
        return;
    insert(r->getData(),r->getCount());
    copy(r->getChildPtr(LEFT));
    copy(r->getChildPtr(RIGHT));
}

template<typename T>
vector<node<T>*> bst<T>::getFreq()
{
    return top;
}

template<typename T>
void bst<T>::nukem(node<T> *&r)
{
    if(!r)
        return;
    nukem(r->childPtr(LEFT));
    nukem(r->childPtr(RIGHT));
    delete r;
    r = NULL;
}
template<typename T>
void bst<T>::getTop()
{
    getTop(root);
}

template<typename T>
void bst<T>::getTop(node<T>* r)
{
    if(!r)
        return;
    getTop(r->childPtr(LEFT));
    int min  = 0;
    for(int i = 0;i<10;i++)
    {
        if(top[i]->getCount() < top[min]->getCount())
            min = i;

    }
    if (r->getCount() > top[min]->getCount())
        top[min] = r;
    getTop(r->childPtr(RIGHT));
}


template<typename T>
void bst<T>::inOrder(node<T>* r, ostream &out) const
{
    if(!r)
        return;

    inOrder(r->childPtr(LEFT),out);

    out<<setw(15)<<r->getData()<<setw(10)<<r->getSyllables();
    out<<setw(10)<<r->getLines()[0]<<setw(10)<<r->getParagraphs()[0]<<endl;
    for(size_t i = 1;i<r->getLines().size();i++)
    {
        out<<setw(35)<<r->getLines()[i]<<setw(10)<<r->getParagraphs()[i]<<endl;
    }

    inOrder(r->childPtr(RIGHT),out);
}




template<typename T>
void bst<T>::preOrder(node<T>* r, ostream &out) const
{
    if(!r)
        return;
    out<<setw(15)<<r->getData()<<setw(10)<<r->getSyllables();
    out<<setw(10)<<r->getLines()[0]<<setw(10)<<r->getParagraphs()[0]<<endl;
    for(size_t i = 1;i<r->getLines().size();i++)
    {
        cout<<setw(35)<<r->getLines()[i]<<setw(10)<<r->getParagraphs()[i]<<endl;
    }
    preOrder(r->childPtr(LEFT),out);
    preOrder(r->childPtr(RIGHT),out);
}

template<typename T>
void bst<T>::postOrder(node<T>* r, ostream &out) const
{
    if(!r)
        return;
    postOrder(r->childPtr(LEFT),out);
    postOrder(r->childPtr(RIGHT),out);
    out<<setw(15)<<r->getData()<<setw(10)<<r->getSyllables();
    out<<setw(10)<<r->getLines()[0]<<setw(10)<<r->getParagraphs()[0]<<endl;
    for(size_t i = 1;i<r->getLines().size();i++)
    {
        cout<<setw(35)<<r->getLines()[i]<<setw(10)<<r->getParagraphs()[i]<<endl;
    }
}

//find depth of node
template<typename T>
size_t bst<T>::depth(node<T> *r)
{
    return !r ? 0 : 1 + max(depth(r->childPtr(LEFT)) , depth(r->childPtr(RIGHT)));
}

//check if subtree is balanced
template<typename T>
bool bst<T>::balanced(node<T> *r)
{
    return abs((double)depth(r->childPtr(LEFT)) - depth(r->childPtr(RIGHT))) <= 1;
}

//determines farthest leaf in subtree
template<typename T>
node<T>* bst<T>::theMost(node<T>*r, WHICH_CHILD child) const
{
    //if empty tree
    if(empty())
        throw EMPTY;
    //otherwise traverse children
    node<T> *temp = r;
    while(temp)
    {
        if(temp->childPtr(child))
            temp = temp->childPtr(child);
        else
            break;
    }

    return temp;
}

//determine parent given node root
template<typename T>
node<T>* bst<T>::findParent(node<T> *r, T d)
{
    //in case there is no root
    if(!r)
        return NULL;
    if(r == root)
        return root;
    //otherwise check children
    if((r->childPtr(LEFT) && r->childPtr(LEFT)->getData() == d) || r->childPtr(RIGHT) && r->childPtr(RIGHT)->getData() == d)
        return r;
    //check recursively if not children
    else
        findParent(r->childPtr(direction[d<*r]),d);
}

//determines number of nodes recursively
template<typename T>
size_t bst<T>::totalNodes(node<T> *r)
{
    //if pointer is null, return 0, otherwise add to total node of right and left tree
    return r ? 1 + totalNodes(r->childPtr(LEFT)) + totalNodes(r->childPtr(RIGHT)) : 0;
}

//determines node count recursively
template<typename T>
size_t bst<T>::totalDataItems(node<T> *r)
{
    return r ? r->getCount() + totalDataItems(r->childPtr(LEFT)) + totalDataItems(r->childPtr(RIGHT)) : 0;
}

template<typename T>
void bst<T>::initialize(node<T> *n, BST_TRAVERSAL_TYPES  t )
{
    root  = n;
    traverse = t;
    whatToDo[IN_ORDER] = &bst<T>::inOrder;
    whatToDo[PRE_ORDER] = &bst<T>::preOrder;
    whatToDo[POST_ORDER] = &bst<T>::postOrder;
    direction[0] = RIGHT;
    direction[1] = LEFT;
}



//BALANCE PORTION
template<typename T>
node<T>* bst<T>::rightRotate(node<T>* &r)
{

    node<T>* newRoot = r->childPtr(LEFT);
    r->childPtr(LEFT) = newRoot->childPtr(RIGHT);
    newRoot->childPtr(RIGHT) = r;


    r = newRoot;

    root = r;

    return newRoot;

}


template<typename T>
node<T>* bst<T>::leftRotate(node<T>* &r)
{
    //cout<<*r<<" is old root"<<endl;
    node<T>* newRoot = r->childPtr(RIGHT);
    r->childPtr(RIGHT) = newRoot->childPtr(LEFT);
    newRoot->childPtr(LEFT) = r;


    r = newRoot;
    root = r;
    //cout<<"now it is "<<*r<<endl;
    return newRoot;

}


template<typename T>
node<T>* bst<T>::createRightVine(node<T>* r)
{

    cout<<"wot"<<endl;
    node<T> *temp = r;
    while(temp)
    {
        if(temp->childPtr(LEFT))
        {
            //r = rightRotate(temp->childPtr(LEFT));
            temp = temp->childPtr(LEFT);
        }
        else
            temp = temp->childPtr(RIGHT);
    }
    return r;
}

template<typename T>
node<T>* bst<T>::balanceVine(node<T>* r, size_t nodeCount)
{
    size_t times = log10(nodeCount*1.)/log10(2.);
    node<T>* newRoot = r;
    for(size_t i = 0; i < times; ++i)
    {
        //newRoot = leftRotate(newRoot);
        r = newRoot->childPtr(RIGHT);
        for(size_t j = 0; j < nodeCount/2-1; ++j)
        {
            //r = leftRotate(r);
            r = r->childPtr(RIGHT);
        }
        nodeCount >>= 1;
    }
    return newRoot;
}

//output based on traversal order
template<typename T>
ostream& bst<T>::print(ostream &out) const
{
    (this->*whatToDo[traverse])(root, out);
    return out;
}

//ostream operator
template<typename U>
ostream& operator<<(ostream& out, const bst<U> &other)
{
    //call print helper function
    return other.print(out);
}

//istream operator
template<typename U>
istream& operator>>(istream& in, bst<U> &other)
{
    U data;
    int count;
    char junk;
    string line;
    stringstream ss;
    bool moreData = true;
    if(&in == &cin)
        while(moreData)
        {
            cout<<"Data: ";
            getline(in,line);
            if(line.empty())
                moreData = false;
            else
            {
                ss<<line;
                ss>>data;
                ss.clear();
                cout<<"Count: ";
                cin>>count;
                other.insert(data,count);
            }
        }
    else
        while(in>>data>>junk>>count>>junk)
            other.insert(data,count);
    return in;
}



#endif // BST_H
