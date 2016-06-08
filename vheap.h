#ifndef VHEAP
#define VHEAP
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <iomanip>

using namespace std;

enum HEAP_ERRORS {HEAP_EMPTY};

template<typename T>
struct hnode
{
    T data;
    hnode<T>* child[2];
    int syllables, line, paragraph;

    hnode()
    {
        data = T();
        child[0] = child[1] = NULL;
    }
    hnode(T d)
    {
        data = d;
        child[0] = child[1] = NULL;
    }

    hnode(T d, int syllNum, int lineNum, int paragraphNum )
    {
        data = d;
        child[0] = child[1] = NULL;
        syllables = syllNum;
        line = lineNum;
        paragraph = paragraphNum;
    }


    hnode(const hnode<T> &hn)
    {
        data = hn.data;
        child[0] = child[1] = NULL;
        syllables = hn.syllables;
        line = hn.line;
        paragraph = hn.paragraph;
    }

    ~hnode()
    {
        data = T();
        child[0] = child[1] = NULL;
    }

    T getData() { return data;}


    int getSyllables() { return syllables; }
    int getLine() { return line; }
    int getParagraph() { return paragraph; }

};






template<typename T>
class vheap
{
    public:
    //private:
    vector<hnode<T> *> heap;

    //determining frequency
    //vector<hnode<T> *> mostFrequent;
    map<hnode<T>*, int > mostFrequent;
    T prev;
    size_t count;

    void heapifyUp(size_t x);
    void heapifyDown(size_t x);
    size_t findParent(size_t x);
    void copy(const T &other);
    void swap(hnode<T>** first, hnode<T>** second);

    //public:
        vheap();
        vheap(T data);
        ~vheap();
        vheap(const vheap<T> &other);
        vheap<T>& operator=(const vheap<T> &other);

        //insert
        vheap<T>& operator<<(const T& data);
        //insert based on hnode
        vheap<T>& operator<<(hnode<T> &n);
        //remove
        vheap<T>& operator>>(T &data);

        bool empty();
        size_t size();
        void clear();


        //output
        template<typename U>
        friend
        ostream& operator<<(ostream& out, const vheap<U> &h);

        template<typename U>
        friend
        istream& operator>>(istream& in, vheap<U> &h);

};




template<typename T>
vheap<T>::vheap()
{

}

template<typename T>
vheap<T>::vheap(T data)
{
    heap.push_back(new hnode<T>(data));
    count = 0;
}

template<typename T>
vheap<T>::~vheap()
{

}

template<typename T>
vheap<T>::vheap(const vheap<T> &other)
{
      copy(other);
}

template<typename T>
vheap<T>& vheap<T>::operator=(const vheap<T> &other)
{
    if(this != &other)
    {
        copy(other);
    }
    return *this;
}

//insert
template<typename T>
vheap<T>& vheap<T>::operator<<(const T& data)
{
    if(empty())
    {
        heap.push_back(new hnode<T>(data));
    }
    else
    {
        heap.push_back(new hnode<T>(data));
        heapifyUp(size() - 1);
    }

    return *this;
}


template<typename T>
vheap<T>& vheap<T>::operator<<(hnode<T> &n)
{

    hnode<T>* temp = new hnode<T>(n.getData(),n.getSyllables(),n.getLine(),n.getParagraph());

    if(empty())
    {
        heap.push_back(temp);

    }
    else
    {
        heap.push_back(temp);
        heapifyUp(size() - 1);

    }
    return *this;

}


//remove
template<typename T>
vheap<T>& vheap<T>::operator>>(T &data)
{
    for(size_t i = 0;i<heap.size();i++)
    {
        if(heap[i]->getData() == data)
        {


            swap(&heap[i],&heap[heap.size() -1 ]);

            heap.pop_back();

            heapifyDown(0);
            break;
        }

    }

    return *this;
}

template<typename T>
bool vheap<T>::empty()
{
    return heap.empty();
}

template<typename T>
size_t vheap<T>::size()
{
    return heap.size();
}

template<typename T>
void vheap<T>::clear()
{

}


template<typename T>
void vheap<T>::heapifyUp(size_t x)
{
    if(!x)
        return;
    else
    {
        size_t parent = findParent(x);
        if(heap[x]->getData() <= heap[parent]->getData())
        {

            swap(&heap[x],&heap[parent]);
            heapifyUp(parent);
        }
    }
}

template<typename T>
void vheap<T>::heapifyDown(size_t x)
{

    size_t leftChild = 2 * x + 1;
    size_t rightChild = 2 * x + 2;

    if (x >= size())
    {
        return;
    }
    if (leftChild < size()  && rightChild < size())
    {
        if (heap[x]->getData() > min(heap[leftChild]->getData(),heap[rightChild]->getData()))
        {
            if (heap[leftChild]->getData() < heap[rightChild]->getData())
            {

                swap(&heap[x],&heap[leftChild]);


                heapifyDown(leftChild);
            }
            else if (heap[leftChild]->getData() > heap[rightChild]->getData())
            {
                swap(&heap[x],&heap[rightChild]);


                heapifyDown(rightChild);
            }
        }
    }
    else if (rightChild >= size() && leftChild < size())
    {
        if (heap[x]->getData() > heap[leftChild]->getData())
            swap(&heap[x],&heap[leftChild]);
    }
    else
    {
        return ;
    }




}

template<typename T>
size_t vheap<T>::findParent(size_t x)
{
    return (x-1)/2;
}

template<typename T>
void vheap<T>::copy(const T &other)
{

}

template<typename T>
void vheap<T>::swap(hnode<T> **first, hnode<T> **second)
{
    hnode<T>* temp;
    temp = *first;
    *first = *second;
    *second = temp;
}


//output
template<typename U>
ostream& operator<<(ostream& out, vheap<U> &h)
{
    U last = "";
    while(!h.empty())
    {


        U data = h.heap[0]->getData();
        if( last == data)
        {
            out<<setw(35)<<h.heap[0]->getLine()<<setw(10)<<h.heap[0]->getParagraph()<<endl;

        }
        else
        {
            out<<setw(15)<<data<<setw(10)<<h.heap[0]->getSyllables()<<setw(10)<<h.heap[0]->getLine()<<setw(10)<<h.heap[0]->getParagraph()<<endl;

        }
        last = data;
        h>>data;


    }
    return out;
}

template<typename U>
istream& operator>>(istream& in, vheap<U> &h)
{

    return in;
}




#endif // VHEAP

