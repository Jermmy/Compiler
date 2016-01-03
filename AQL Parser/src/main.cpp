#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Parser.h"
#include "Lexer.h"
#include "Tokenizer.h"

using namespace std;

vector<string> getFilesFromDirectory(string dir) {
	DIR *dp; // 代表目录的流
	struct dirent *dirp;
	struct stat filestat;
	vector<string> files;

	dp = opendir(dir.c_str());

	if (dp == NULL) {
		cout << "Directory " << dir << " not found!" << endl;
		return files;
	}

	while ((dirp = readdir(dp))) {
        
        string file_path = dir;
        if (dir[dir.length()-1] != '/') {
		   file_path += "/";
	    }
	    file_path += dirp->d_name;

		// 如果该目录下存在其他子目录，则忽略跳过
	    if (stat(file_path.c_str(), &filestat)) {
	    	continue;
	    }
	    if (S_ISDIR(filestat.st_mode)) {
	    	continue;
	    }

        // 只读取.txt文件
	    if (file_path.find(".txt") != string::npos) {
	    	files.push_back(file_path);
	    }
	}

	closedir(dp);
	return files;
}


int main(int argc, char** argv) {
	string file(argv[2]);
	size_t dot_index = file.find(".txt");
	vector<string> files;

	// 找不到符号'.txt'，说明为目录名
	if (dot_index == string::npos) {
		files = getFilesFromDirectory(file);
	} else {
		if (file.find(".txt") != string::npos) {
           files.push_back(file);
		}
	}

	vector<string>::iterator it;
	for (it = files.begin(); it != files.end(); it++) {

		dot_index = (*it).find(".txt");
		string new_file = (*it).substr(0, dot_index);
		new_file += ".output";

		ofstream fout(new_file);

		fout << "Processing " << *it << endl;
		fout.close();

		Lexer *lexer = new Lexer(argv[1]);
		Tokenizer *tokenizer = new Tokenizer();
		Parser *parser = new Parser(lexer, tokenizer, (*it).c_str());

		try {
		    parser->program();
		} catch (string& err) {
			cout << err << endl;
		}
		delete parser;	
	}

	return 0;
}

