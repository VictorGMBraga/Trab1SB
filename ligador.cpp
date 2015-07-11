#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <cstdlib>

enum ReadMode { USETABLE, DEFTABLE, CODE };

using namespace std;

typedef map<string, int> UseTable;
typedef map<string, int> DefTable;

UseTable useTable;
DefTable defTable;
vector<string> outputVector;
string output = "";
int offset = 0;

void readModule (const char* fileName) {

	ifstream infile(fileName);
    string line;
    ReadMode readMode = USETABLE;

    while (getline(infile, line)) {

    	if (line.empty()){ 
    		continue;
    	} else if ("TABLE USE" == line) {
    		readMode = USETABLE;
    		continue;
    	} else if ("TABLE DEFINITION" == line) {
    		readMode = DEFTABLE;
    		continue;
    	} else if ("CODE" == line) {
    		readMode = CODE;
    		continue;
    	}

    	switch (readMode) {

    		case USETABLE: {

    			istringstream iss(line);
    			string labelName;
    			int memPos = offset;

    			iss >> labelName;
    			iss >> memPos;

    			useTable[labelName] = memPos + offset;

    			break;
    		}
    		case DEFTABLE: {

    			istringstream iss(line);
    			string labelName;
    			int memPos = offset;

    			iss >> labelName;
    			iss >> memPos;

    			defTable[labelName] = memPos + offset;

    			break;
    		}
    		case CODE: {

				int count = 0;
				istringstream iss(line);
				string tempStr;

				while (iss >> tempStr) {

					outputVector.push_back(tempStr);

					count++;
				}

				offset = count;

    			break;
    		}
    	}

    }
}

void replaceAddresses () {
	
	for (UseTable::iterator i = useTable.begin(); i != useTable.end(); ++i) {
		
		string label = i->first;
		int position = i->second;

		stringstream ss;
		ss << defTable[label];
		string str = ss.str();

		outputVector[position] = str;
	}
}

void outputVectorToString () {

	for (std::vector<string>::iterator i = outputVector.begin(); i != outputVector.end(); ++i) {
		
		output.append(*i+" ");

	}

}

int main (int argc, char const *argv[]) {

	char *file1 = (char*) malloc(sizeof(char)*strlen(argv[1]) + 1);
	char *file2 = (char*) malloc(sizeof(char)*strlen(argv[2]) + 1);
	char *file3 = (char*) malloc(sizeof(char)*strlen(argv[3]) + 1);

	strcpy(file1, argv[1]);
	strcpy(file2, argv[2]);
	strcpy(file3, argv[3]);

	strcat(file1, ".o");
	strcat(file2, ".o");
	strcat(file3, ".e");

	readModule(file1);
	readModule(file2);
	replaceAddresses();
	outputVectorToString();

	ofstream outputFile;
	outputFile.open(file3);
	outputFile << output;
	outputFile.close();

	return 0;
}