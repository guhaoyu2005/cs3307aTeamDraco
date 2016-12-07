#include "database/CSVWriter.h"
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <QDebug>

using namespace std;

CSVWriter::CSVWriter(){

}

/*
 * FUNCTION FOR SAVING
 */
bool CSVWriter::writeToCSVDueToShitCodeForGrantShits(std::string path, vector<string> header, vector<vector<string>> data) {
    createBackup(path);

    ofstream out;
    out.open(path);
    for (int i=0;i<header.size()-1;i++) {
        out<<header[i]<<(i==header.size()-1? "":",");
    }

    int whoWannaEatShits[] = {1, 4, 18, 19, 21, 26, 27, 37};

    for (int i=0;i<data.size();i++) {
        out<<endl;
        for (int j=0;j<data[i].size();j++){
            std::string s = data[i][j];
            bool needShit = false;
            for (int i=0;i<8;i++)
                if (j==whoWannaEatShits[i])
                    needShit = true;
            if (s!="") {
                if (needShit) {
                     out<<'"'<<s<<'"';
                }
                else {
                    out<<s;
                }
            }
            out<<((j==data[i].size()-1)?"":",");
        }
    }
    out<<endl;
    out.close();

    removeBackup(path);
    return true;
}


bool CSVWriter::writeToCSV(std::string path, vector<string> header, vector<vector<string>> data) {
    createBackup(path);

    ofstream out;
    out.open(path);
    for (int i=0;i<header.size()-1;i++) {
        out<<header[i]<<(i==header.size()-1? "":",");
    }
    for (int i=0;i<data.size();i++) {
        out<<endl;
        for (int j=0;j<data[i].size();j++){
            std::string s = data[i][j];
            //out<<((s.find(',')==std::string::npos)&&(s.find('$')==std::string::npos)?"":"\"")<<data[i][j]<<((s.find(',')==std::string::npos)&&(s.find('$')==std::string::npos)?"":"\"")<<(j==data[i].size()-1? "":",");
            bool quoteFlag = s.length()>0?1:0;
            out<<(quoteFlag?"\"":"")<<data[i][j]<<(quoteFlag?"\"":"");
            out<<(j==data[i].size()-1? "":",");
        }
    }
    out<<endl;
    out.close();

    removeBackup(path);
    return true;
}

bool CSVWriter::isExistingBackup(std::string path) {
    stringstream ss;
    ss<<path<<".bak";
    string bakPath = ss.str();
    ifstream in;
    in.open(bakPath);
    if (in) {
        in.close();
        return true;
    }
    in.close();
    return false;
}

void CSVWriter::createBackup(std::string path) {
    stringstream ss;
    ss<<path<<".bak";
    string bakPath = ss.str();
    ifstream in;
    ofstream out;
    in.open(path);
    out.open(bakPath);

    string buff;
    while (getline(in, buff)) {
        out<<buff;
    }

    in.close();
    out.close();
}

void CSVWriter::removeBackup(std::string path) {
    remove((path+".bak").c_str());
}

void CSVWriter::recoverBackup(std::string path) {
    stringstream ss;
    ss<<path<<".bak";
    ifstream in;
    in.open(ss.str());
    if (in) {
        remove(path.c_str());
        rename(ss.str().c_str(), path.c_str());
    }
    in.close();
}

