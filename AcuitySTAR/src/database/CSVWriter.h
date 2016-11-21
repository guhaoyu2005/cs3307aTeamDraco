#ifndef CSVWRITER_H
#define CSVWRITER_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>

class CSVWriter {
public:
    CSVWriter();
    bool writeToCSV(std::string path, std::vector<std::string>, std::vector<std::vector<std::string>>);
    bool writeToCSVDueToShitCodeForGrantShits(std::string path, std::vector<std::string>, std::vector<std::vector<std::string>>);
    bool isExistingBackup(std::string path);
private:
    void createBackup(std::string path);
    void removeBackup(std::string path);
    void recoverBackup(std::string path);
};

#endif // CSVWRITER_H
