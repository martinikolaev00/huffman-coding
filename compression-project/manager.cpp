#pragma once
#include"manager.hpp"
namespace fs = std::filesystem;
void test()
{
	
	
	fs::path writefileshere = "..\\..\\compression-project\\decompressed";
	std::ifstream in("..\\..\\compression-project\\output.bin", std::ios::binary);
	takein(in, writefileshere);
	in.close();
}
void starter()
{
	std::cout << "\t\tMenu\n"<<"1: creating archive of files/directories\n"<<"2: take out files out of archive\n"<<"3: information about archive\n";
	int choice = 0;
	std::cin >> choice;
	if (choice == 1)
		createarc();
	else if (choice == 2)
		decrypt();
	else if (choice == 3)
		info();
	else std::cout << "Error";
	return;
}
void createarc()
{
	std::cout << "How many files/folders do you want your archive to include: ";
	int num_paths = 0;
	std::cin >> num_paths;
	std::vector<fs::path> paths;
	std::string input;
	std::cout << "Please enter " << num_paths << " paths that reach the files/folders you want to archive.\n" <<
		"An example of path D:\\DSA\\file.txt, you can use ..\\ to skip a parent folder. Write at least 1 parent folder.\n";
	for (int i = 0; i < num_paths; ++i)
	{
		std::cout << i+1<<": ";
		std::cin >> input;
		fs::path path;
		path.assign(input);
		paths.push_back(path);
		std::cout << "\n";
	}
	huffNode* root = maketree(paths);
	booltable codes;
	std::vector<char> chars;
	create_codes(root, codes, chars); /// after this line root is nullptr
	std::ofstream out("..\\..\\compression-project\\output.bin", std::ios::binary); /// here we will write the output
	writecodes(out, codes, chars, paths); /// we write our code table and total number of files in the output
	for (int i = 0; i < num_paths; ++i) /// we write each path in the output
		takeout(out, codes, chars, paths[i]);
	/// after loop we are done 
	out.close();
	std::cout << "Finished compressing, have a nice day!";
	return;
}

void decrypt()
{
	std::cout << "Write the path to the compressed file\n"<< "An example of path D:\\DSA\\file.txt, you can use ..\\ to skip a parent folder. Write at least 1 parent folder.\n";
	std::string dec;
	std::cin >> dec;
	std::ifstream in(dec, std::ios::binary);
	fs::path writefileshere = "..\\..\\compression-project\\decompressed"; /// will allow user to write this path
	takein(in, writefileshere);
	in.close();
}

void info()
{
}
