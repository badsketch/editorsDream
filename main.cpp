#include <iostream>
#include <vector>
#include <time.h>
#include "vheap.h"
#include <fstream>
#include <sstream>
#include "bst.h"
#include <string>
using namespace std;

int standardize(string &line);
int numSyllables(string &word);
void outputHeap(vheap<string> data[]);
void outputBST(bst<string> data[]);
bool hasExtension(string &str, string ext);


void parseText(string fileName, bool selection);
bool wayToSort(node<string>* lhs, node<string>* rhs){return lhs->getCount() >rhs->getCount();}

bool again();


int main()
{
    do
    {
        string file;
        string selection;
        cout<<"Enter text file to examine: ";
        getline(cin,file);
        if(!hasExtension(file,".txt"))
            file += ".txt";
        cout<<"1. HEAP"<<endl;
        cout<<"2. BST"<<endl;
        cout<<"Enter selection(0,1): ";
        getline(cin, selection);
        if(selection == "1")
            parseText(file, true);
        else if(selection == "2")
            parseText(file, false);
        else
            cout<<"Unrecognized Option."<<endl;



    }while(again());

    return 0;
}

bool again()
{
   char ans;
   cout<<"Do you want to do this again? ";
   cin >> ans;
   cin.ignore();
   return toupper(ans) == 'Y';
}


int standardize(string &line)
{
    size_t sentence = 0;
    for(size_t i = 0;i<line.size();i++)
    {
        if(line[i] == '.')
        {
            sentence++;
        }
        if(!isalpha((unsigned char)line[i]))
        {
            line[i] = ' ';
        }

    }
    return sentence;


}

int numSyllables(string &word)
{
    int num = 0;
    bool prev = false;
    for(size_t i = 0;i<word.size();i++)
    {
        if(word[i] == 'a' ||word[i] == 'e'||word[i] == 'i'||word[i] == 'o'||word[i] == 'u'||word[i] =='y')
        {
            if (!prev)
            {
                num++;
                prev = true;
                if(i == word.size() - 1 && word[i] == 'e')
                {
                    num--;
                }
            }
        }
        else {
            prev = false;
        }
    }
    if(!num)
        num++;
    return num;
}



void parseText(string fileName, bool selection)
{
    int readingLevel, syllables, sentence = 0, paragraph = 0,
            totalSyllables = 0,lineNum = 0, totalWords = 0;
    vector<node<string>*> allTop;

    ifstream myfile(fileName);
    string line;
    vheap<string> heapList[26];
    bst<string> bstList[26];
    clock_t start, end;
    start = clock();
    if(!myfile.eof())
    {
        while(getline(myfile, line))
        {
            lineNum++;
            if(line == "")
                paragraph++;
            sentence += standardize(line);

            stringstream ss;
            ss << line;
            string word;
            while(ss >> word)
            {
                word[0] = toupper(word[0]);
                syllables = numSyllables(word);
                totalSyllables += syllables;
                if(selection)
                {
                    hnode<string> toBeAdded(word, syllables, lineNum, paragraph);
                    heapList[word[0]-'A']<<toBeAdded;

                }
                else
                {
                   bstList[word[0] - 'A'].insert(word,1,syllables,lineNum,paragraph );
                }

            }

        }
        myfile.close();
    }
    else
        cout<<"UNABLE TO OPEN FILE.";


    cout<<"---------------------------------------------------------"<<endl;
    cout<<"SUMMARY"<<endl;
    cout<<"---------------------------------------------------------"<<endl;
    if (selection)
    {
        for(size_t i = 0;i<26;i++)
        {
            totalWords += heapList[i].size();
            cout<<char(i+ 'a')<<": "<<heapList[i].size()<<endl;

        }


    }
    else
    {
        for(size_t i = 0;i<26;i++)
        {
            totalWords += bstList[i].totalData();
            cout<<char(i+ 'a')<<": "<<bstList[i].totalData()<<endl;
            bstList[i].getTop();
        }
        for(size_t i = 0;i<26;i++)
        {
            for(size_t j = 0;j<10;j++)
            {
                allTop.push_back(bstList[i].getFreq()[j]);
            }
        }

        sort(allTop.begin(),allTop.end(),wayToSort);
        cout<<"---------TOP10 MOST FREQUENT-------------------"<<endl;
        for(size_t i = 0;i<10;i++)
        {
            cout<<*allTop[i]<<endl;
        }
        cout<<"-----------------------------------------------"<<endl;
    }

    cout<<"Word Count: "<<totalWords<<endl;
    cout<<"Paragraph Count: "<<paragraph<<endl;
    cout<<"Sentence Count: "<<sentence<<endl;
    readingLevel = 206.835 - 1.015*((double)totalWords/(double)sentence)-84.6*((double)totalSyllables/(double)totalWords);
    cout<<"Reading Level: "<<readingLevel<<endl;
    end = clock();
    float seconds = ((float)end - (float)start)/CLOCKS_PER_SEC;

    cout<<"completed in "<<seconds<<" seconds"<<endl;

    char option;
    cout<<"Would you like to print the detailed page? (y/n): ";
    cin >> option;
    if(option == 'y')
    {
        if(selection)
        {

            outputHeap(heapList);
        }
        else

            outputBST(bstList);
    }
    else
        return;



}

void outputHeap(vheap<string> data[26])
{
    //vector<hnode<string>*> copy;
    vheap<string> copy;
    cin.ignore();
    cout<<"---------------------------------------------------------"<<endl;
    cout<<"ALL WORDS"<<endl;
    cout<<"---------------------------------------------------------"<<endl;
    cout<<setw(15)<<"Word"<<setw(10)<<"Syllables"<<setw(10)<<"Lines"<<setw(10)<<"Paragraph"<<endl;
    for(int i = 0;i<26;i++)
    {
            for(size_t j = 0;j<data[i].heap.size();j++)
            {
                hnode<string> temp(*data[i].heap[j]);
                copy<<temp;
            }

            cout<<data[i]<<endl;


    }

    string ans;
    cout<<"Save to file?";
    getline(cin, ans);
    if(ans == "y")
    {
        string file;
        cout<<"Enter output file name: ";
        getline(cin, file);
        if(!hasExtension(file,".txt"))
            file += ".txt";
        ofstream myfile;
        myfile.open(file);
        myfile <<"---------------------------------------------------------"<<endl;
        myfile <<"ALL WORDS"<<endl;
        myfile <<"---------------------------------------------------------"<<endl;
        myfile <<setw(15)<<"Word"<<setw(10)<<"Syllables"<<setw(10)<<"Lines"<<setw(10)<<"Paragraph"<<endl;
        myfile<<copy<<endl;

        myfile.close();
    }
}

void outputBST(bst<string> data[26])
{
    bst<string> copy;
    cin.ignore();
    cout<<"---------------------------------------------------------"<<endl;
    cout<<"ALL WORDS"<<endl;
    cout<<"---------------------------------------------------------"<<endl;
    cout<<setw(15)<<"Word"<<setw(10)<<"Syllables"<<setw(10)<<"Lines"<<setw(10)<<"Paragraph"<<endl;


    for(int i = 0;i<26;i++)
    {
        cout<<data[i]<<endl;
    }

    string ans;
    cout<<"Save to file?: ";
    getline(cin, ans);
    if(ans == "y")
    {
        string file;
        cout<<"Enter output file name: ";
        getline(cin, file);
        if(!hasExtension(file,".txt"))
            file += ".txt";
        ofstream myfile;
        myfile.open(file);
        myfile <<"---------------------------------------------------------"<<endl;
        myfile <<"ALL WORDS"<<endl;
        myfile <<"---------------------------------------------------------"<<endl;
        myfile <<setw(15)<<"Word"<<setw(10)<<"Syllables"<<setw(10)<<"Lines"<<setw(10)<<"Paragraph"<<endl;

        for(int i = 0;i<26;i++)
        {


            myfile<<data[i]<<endl;
        }
        myfile.close();
    }


}

bool hasExtension(string &str, string ext)
{
    return str.size() >= ext.size() && str.compare(str.size() - ext.size(),ext.size(),ext) == 0;
}
