#ifndef CSVREADER_H
#define CSVREADER_H

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

class CSVReader {
    std::vector<std::string> headers;
    std::vector<std::vector<std::string>> all_data;
public:
    enum CSVFileType {CSVFileTypeGrants = 0,
                      CSVFileTypePresentations,
                      CSVFileTypePublications,
                      CSVFileTypeTeaching,
                      CSVFileTypeBadFile};
public:
    CSVReader();
    CSVReader(std::string);
    void loadCSV(std::string);
    int parseDateString(std::string dateString);
    std::vector<std::string> getHeaders();
    std::vector<std::vector<std::string>> getData();

public:
    CSVFileType getFileType(std::string&);
};

#endif // CSVREADER_H
