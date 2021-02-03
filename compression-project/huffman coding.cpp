#pragma once
#include"huffman coding.hpp"
#include<iostream>
#include<filesystem>
#include<bitset>
namespace fs = std::filesystem;
typedef unsigned short us;
typedef std::vector<std::vector<bool>> booltable;
huffNode::huffNode() :data('\0'), freq(0), left(nullptr), right(nullptr) {}

huffNode::huffNode(const huffNode& other)
{
	data = other.data;
	freq = other.freq;
	add_leaves(other.get_leftleaf(), other.get_rightleaf());
}

huffNode::huffNode(huffNode* tleft, huffNode* tright)
{
	freq = tleft->freq + tright->freq;
	add_leaves(tleft, tright);
	data = '\0';
}

huffNode::huffNode(const char tdata, const size_t tfreq)
{
	data = tdata;
	freq = tfreq;
	left = right = nullptr;
}

huffNode& huffNode::operator=( huffNode& other)
{
	if (!(this == &other))
	{
		this->data = other.data;
		this->freq = other.freq;
		this->left = other.left;
		this->right = other.right;
	}
	return *this;
}

void huffNode::add_leaves(huffNode* left, huffNode* right)
{
	this->left = left;
	this->right = right;
}

bool huffNode::leaf() /// checks if current node is leaf
{
	{ return (get_leftleaf() == nullptr && get_rightleaf() == nullptr ? true : false); }
}

void huffNode::root_delete()
{
	if (!leaf())
	{
		left->root_delete();
		right->root_delete();
	}
	delete right, delete left;
	right = left = nullptr;
}

const size_t huffNode::root_depth(huffNode* root)const 
{
	if (root == nullptr)
		return 0;
	else
	{
		int ldepth = root_depth(root->get_leftleaf());
		int rdepth = root_depth(root->get_rightleaf());
		if (ldepth > rdepth)
			return(ldepth + 1);
		else return(rdepth + 1);
	}
}

bool compare::operator()(const huffNode* l, const huffNode* r) const
{
	return (l->getfreq() > r->getfreq());
}

bool compare::operator()(huffNode* l, huffNode* r)
{
	return (l->getfreq() > r->getfreq());
}

huffNode* maketree(std::string file)
{
	int* characters = getcounts(file);
	if (!characters)
		throw std::logic_error("file seems broken");
	minheap* pyramid = new minheap;
	for (int i = 0; i < 256; ++i) /// for each part of the character array we create a new huffnode and pushes it into pyramid
	{
		if (characters[i] == 0)
			continue;
		else
		{
			huffNode* buff = new huffNode(i, characters[i]);
			pyramid->push(buff);
		}
	}
	while (pyramid->getsize() != 1) /// while we have more than 1 element in pyramid we use @make_huffman_code to attach everything in a tree
	{
		make_huffman_code(pyramid);
	}
	huffNode* root = new huffNode(*pyramid->top()); 
	pyramid->pop();
	delete[] characters;
	return	root;
	
}

int* getcounts(std::filesystem::path path) /// goes through the entire directory and counts how many times we find each character in the file 
{
	int* freq_arr = new int[256];
	for (int i = 0; i < 256; ++i)
		freq_arr[i] = 0;
	freq_arr['?']++; /// hardcoding these 2 to make sure we will be able to translate filenames into codes later 
	freq_arr['*']++;
	char buff = 0;
	if (fs::is_regular_file(path)) /// if the user only gave us a path to normal file
	{
		std::ifstream filee(path);
		if (!filee.is_open())
		{

			throw fs::filesystem_error("Cant open: " + path.string(), std::error_code());
		}
		while (filee>>buff) /// we simply count and mark the chars inside
		{
			freq_arr[static_cast<int>(buff)]++;
		}
		filee.close();
	}
	else for (const fs::directory_entry& it : fs::recursive_directory_iterator(path)) /// but if he gave us path to directory we go recursively everywhere and to every file
	{
		if (it.is_regular_file())
		{
			std::ifstream freq(it.path());
			if (!freq.is_open())
				throw fs::filesystem_error("Cant open: " + path.string(), std::error_code());
			while (freq.good())
			{
				freq.get(buff);
				freq_arr[static_cast<int>(buff)]++;
			}
			freq.close();
		}
		
	}
	return freq_arr;

}

void make_huffman_code(minheap* pyramid) /// removing 2 tops from the pyramid and combining their freq into 1 new el and putting them as left and right leaves
{	
	
	const huffNode* buff1 = pyramid->top(); pyramid->pop();
	huffNode* buffer1 = const_cast<huffNode*> (buff1);
	const huffNode* buff2 = pyramid->top(); pyramid->pop();
	huffNode* buffer2 = const_cast<huffNode*> (buff2);
	huffNode* bigbuff = new huffNode(buffer1,buffer2); /// remember to delete
	pyramid->push(bigbuff);
}

void create_codes(huffNode* root, booltable& codes, std::vector<char>& chars)
{
	 /// this table will help us store all the different codes for each char
	size_t num_of_codes = leafcount(root);
	codes.reserve(num_of_codes);
	/// im thinking of creating additional array which will indacate what char corresponds to codes[i] 
	chars.reserve(num_of_codes);
	std::vector<bool> current;
	huffNode* copy = root;
	rec_lrr(copy, current, codes, chars); /// this will fill up codes and chars correspondingly
	root->root_delete();
	delete root;
}

size_t leafcount(huffNode* root)
{
	if (root == nullptr)
		return 0;
	else if (root->get_rightleaf() == nullptr && root->get_leftleaf() == nullptr)
		return 1;
	return (leafcount(root->get_leftleaf() + leafcount(root->get_rightleaf())));
}

void rec_lrr(huffNode* root, std::vector<bool> current, booltable codes, std::vector<char> chars)
{
	if (root->leaf())
	{
		codes.push_back(current);
		chars.push_back(root->getdata());
		current.erase((--current.end())); /// deleting the last ad	ded digit 0/1 and going back 1 step
		return;	 /// recursion will end at the most right leaf 
	}
	if (root->get_leftleaf() != nullptr)
	{
		current.push_back(false); /// 0 for left
		rec_lrr(root->get_leftleaf(), current, codes, chars);
	}
	if (root->get_rightleaf() != nullptr)
	{
		current.push_back(true); /// 1 for right
		rec_lrr(root->get_rightleaf(), current, codes, chars);
	}
}

std::ofstream& takeout(std::ofstream& out, booltable codes, std::vector<char> chars, fs::path path)
{
	us num_of_codes = chars.size();
	us size = 0;
	unsigned char transporter = 0;
	out.write((const char*)&num_of_codes, sizeof(us)); /// 1st we write the amount of different chars
	for (int i = 0; i < num_of_codes; ++i) /// then we start writing each row like  char size code
	{
		out.write((const char*)&chars[i], sizeof(char)); /// then we write the current char 
		size = codes[i].size();  out.write((const char*)&size, sizeof(us)); /// then we write the lenght of the binary array
		for (int j = 0; j < size; ++j) /// then we write the code
		{
			transporter = 0;
			for (unsigned char mask = 1; mask > 0 && j < size; mask <<= 1, ++j)
			{
				if (codes[i].at(j))
					transporter |= mask;
			}
			out.write((const char*)&transporter, sizeof(unsigned char));
		}
	} /// after this loop is finished we've written our coding table 
	/// now we write the number of files we have
	if (fs::is_directory(path))
	{
		us counter = 0; /// we consider there will be less than 30k files
		for (const fs::directory_entry& it : fs::recursive_directory_iterator(path))
				++counter;
		out.write((const char*)&counter, sizeof(us));
	}
	else
	{
		short x = 1;
		out.write((const char*)&x, sizeof(us)); /// if its just a file (not directory) that the user entered we write 1
		bool f = true;
		out.write((const char*)&f, sizeof(bool)); /// we write 1 to indacete its a file
		try {
			writelenght(path, out); /// now we write the lenght of the name of the file
		}
		catch (...)
		{
			throw std::logic_error("file with name" + path.filename().string() + "broke while writing to file");
		} 
		writestrings(path.filename().string(), out, chars, codes); /// now we write the actual name of the file writtin in our code
		writefiles(path, out, chars, codes); /// and finally we write the file itself
		return out;
	} /// after we wrote the number of files we start to iterate and write each one
	bool isFile = false;
	for (const fs::directory_entry& it : fs::recursive_directory_iterator(path))
	{
		if (!fs::is_directory(it)) /// if its not a folder 
		{
			isFile = true;
			out.write((const char*)&isFile, sizeof(bool)); /// 1 for file and 0 for folder
			size_t  file_size = fs::file_size(it);
			out.write((const char*)&file_size, sizeof(size_t)); /// we write the size of the current file
			fs::path n = it.path().filename();
			std::string name = path.string(); /// planning to write the the file/folder name's lenght 1st with just 1 byte so max lenght will be 255
			writelenght(path, out);
			/// now we will write the actual name but 1st have to tranform it according to our codes
			writestrings(name, out, chars, codes); /// this will write the actual name of the current file in our out file
			writefiles(it, out, chars, codes); /// now we write the entire fail
		}
		else /// if its a folder
		{
			isFile = false;
			out.write((const char*)&isFile, sizeof(bool)); /// 0 for a folder
			us cnt = 0;
			for (fs::directory_entry ite : fs::directory_iterator(it))
					cnt++;
			out.write((const char*)&cnt, sizeof(us)); /// now we write how many elements does this folder have
			writelenght(path, out); /// we write the lenght of the name of our folder
			writestrings(it.path().filename().string(), out, chars, codes); /// and we write the folder's name and thats it for folders
		}
	}
	return out;
}

std::ofstream& writelenght(fs::path path, std::ofstream& out)
{
	std::string name = path.string();
	us size = name.length();
	out.write((const char*)&size, sizeof(us)); /// writing the lenght of the filename
	return out;
	
}

std::ofstream& writevector(std::vector<bool> input, std::ofstream& out)
{/// Note by the time we use this function we should have already written the size of the vector inside the file
	size_t size = input.size(); 
	for (int j = 0; j < size; ++j) /// then we write the code
	{
		unsigned char transporter = 0;
		for (unsigned char mask = 1; mask > 0 && j < size; mask <<= 1, ++j)
		{
			if (input.at(j))
				transporter |= mask;
		}
		out.write((const char*)&transporter, sizeof(unsigned char));
		if (!out)
			throw;
	}
	return out;
}

std::ofstream& writestrings(std::string input, std::ofstream& out,std::vector<char> chars, booltable codes)
{ ///now we convert that string using the codes into a bool vector and we push it in using writevector
	std::vector<bool> output;
	size_t size = input.length(), holder=0,vessel=0;
	output.reserve(size); ///reserving minimum amount so we dont have to instantly resize
	for (size_t i = 0; i < size; ++i) 
	{
		holder = find(chars, input[i]); /// getting the index of the char remember chars[i]'s code is in codes[i]
		if (holder == -1) /// should be impossible case
		{
			throw std::logic_error("needed code not found");
		}
		vessel = codes[holder].size();
		for (int j = 0; j < vessel; ++j) /// kinda slow
			output.push_back(codes[holder].at(j));
	}/// after this loop we should have all chars from string written in output using codes
	try {
		writevector(output, out);
	}
	catch(...) {
		throw std::logic_error("failed to write " + input + " into file");
	}
	return out;

}

std::ofstream& writefiles(fs::path path, std::ofstream& out, std::vector<char> chars, booltable codes)
{/// NOTE this function sshould only be called by files, no directories
	/// since we went through all files recursively there shouldnt be a missing char that exist in our files but not in codes
	/// will go through file making a string out of it and then call writestrings with all chars from the file
	if (!fs::is_regular_file(path)) /// just in case
		return out;
	std::ifstream file(path);
	std::string output;
	char buffer;
	while (file >> buffer)
	{
		output.push_back(buffer);
	}
	try {
		writestrings(output, out, chars, codes);
	}
	catch (...) {
		throw;
	}
	return out;

}

std::ifstream& readfile(std::ifstream& in, fs::path writefileshere,translatetree* root)
{
	std::vector<bool> code;
	size_t file_size = 0;
	in.read((char*)&file_size, sizeof(size_t)); /// we read the lenght of the file
	us len_name = 0;
	in.read((char*)&len_name, sizeof(us)); /// now we read the lenght of the name of the file
	code.reserve(len_name);
	readvector(in, code, len_name); /// read the actual name the file
	std::string name;
	stringfromvector(name, code, 0, len_name, root); /// now we have the name of the file in name
	std::ofstream currentfile(writefileshere.string() +"\\" + name);
	if (!currentfile.is_open())
	{
		throw fs::filesystem_error("File with name " + name + " failed to open for writing", std::error_code());
	}
	code.clear(); code.reserve(file_size); /// preparing to read the file info and put all in code
	readwholefile(in, code, file_size); /// file info should be stored in code
	fromvectofile(code, currentfile, root, name); /// writing upon the new created/opened file
	currentfile.close();
	return in;
}

int find(std::vector<char> chars, char wanted)
{
	size_t size = chars.size();
	for (size_t i = 0; i < size; ++i)
		if (chars[i] == wanted)
			return i;
	return -1;
}

void takein(std::ifstream& in, fs::path writefileshere)
{
	us num_of_codes = 0;
	in.read((char*)&num_of_codes, sizeof(us)); /// 1st we read the amount of codes we have
	translatetree* root = new translatetree();
	us len_of_code=0; char current;
	std::vector<bool> code;
	for (us i = 0; i < num_of_codes; ++i)
	{
		in.read((char*)&current, sizeof(char)); /// we read each char
		in.read((char*)&len_of_code, sizeof(us)); /// then we have our lenght
		code.clear();
		code.resize(len_of_code);
		readvector(in, code, len_of_code);
		if (!in)
			throw std::logic_error("takein broke while reading coding table on code number: " + i);
		buildtranslationtree(root, code, current, 0, len_of_code); /// making our tree while reading codes from file
	} /// after this we have written all codes and built our tree
	us total_number = 0; /// number of all files in the compressed file
	in.read((char*)&total_number, sizeof(us));
	bool isFile = false;
	for (us i = 0; i < total_number;)
	{
		in.read((char*)&isFile, sizeof(bool)); /// 1 - file 0 - folder
		if (isFile)
		{
			readfile(in, writefileshere, root);
			++i;
		}
		else /// isFile will be 0 => folder
		{
			readfolder(in, root, writefileshere,i);
		}
	} /// after this loop we should have read all compressed files
	killtree(root);
}

void buildtranslationtree(translatetree* root, std::vector<bool> code, char current, us current_bit,us len_code)
{
	if (current_bit + 1 == len_code)
	{
		root->data = current;
		return;
	}
	if (code.at(current_bit) == false)
	{
		if (root->left)
			buildtranslationtree(root->left, code, current, ++current_bit, len_code);
		else
		{
			root->left = new translatetree();
			buildtranslationtree(root->left, code, current, ++current_bit, len_code);
		}
	}
	else
	{
		if (root->right)
			buildtranslationtree(root->right, code, current, ++current_bit, len_code);
		else
		{
			root->right = new translatetree();
			buildtranslationtree(root->right, code, current, ++current_bit, len_code);
		}
	}
}

void stringfromvector(std::string& input, std::vector<bool> code, us current_bit, us size, translatetree* root)
{
	for (; current_bit < size;) /// we make different recursion for each char to avoid
	{ /// creating a very deep recursion and overflowing
		charfromvec(input, code, current_bit, size, root);
	}
	
	
}

void charfromvec(std::string& input, std::vector<bool> code, us current_bit, us size, translatetree* root)
{
	bool hold = code.at(current_bit);
	if (hold) /// 1 so we move right or we are in leaf
		if (root->right) /// checks if we can move right
			charfromvec(input, code, ++current_bit, size, root->right);
		else /// we are in a leaf
		{
			input.push_back(root->data); /// since we didnt move at all we wont increase current bit
			if (current_bit + 1 == size) /// this means we are at the last bit from the code and we are done
				current_bit++; /// so we increase bit and when we return the for loop will break
			return; /// we go back to the main root and go again
		}
	else /// 0 same as before but we move left
		if (root->left)
			charfromvec(input, code, ++current_bit, size, root->left);
		else
		{
			input.push_back(root->data);
			if (current_bit + 1 == size)
				current_bit++;
			return;
		}
}

void killtree(translatetree* root)
{
	if (root->left)
		killtree(root->left);
	if (root->right)
		killtree(root->right);
	delete root;
}

std::ifstream& readvector(std::ifstream& in, std::vector<bool>& code, us size)
{ /// maybe add a string here to translate data while reading it to avoid vector becoming gigantic
	for (us j = 0; j < size;) /// and with this for we read our code out of the file
	{
		unsigned char holder;
		in.read((char*)&holder, sizeof(unsigned char));
		for (unsigned char mask = 1; mask > 0 && j < size; ++j, mask <<= 1)
			code.at(j) = holder & mask;
	}
	return in;
}

std::ifstream& readwholefile(std::ifstream& in, std::vector<bool>& code, unsigned long long size)
{
	for (us j = 0; j < size;) /// and with this for we read our code out of the file
	{
		unsigned char holder;
		in.read((char*)&holder, sizeof(unsigned char));
		for (unsigned char mask = 1; mask > 0 && j < size; ++j, mask <<= 1)
			code.at(j) = holder & mask;
	}
	return in;
}

std::ofstream& fromvectofile(std::vector<bool> code, std::ofstream& out, translatetree* root,std::string name)
{
	std::string output; 
	us size = code.size();
	stringfromvector(output, code, 0, size, root);
	out << output;
	
	if (!out)
	{
		throw fs::filesystem_error("file " + name+ " broke while writing it from compressed version", std::error_code());
	}
	return out;
}

std::ifstream& readfolder(std::ifstream& in, translatetree* root, fs::path writefileshere,us& total_number)
{
	bool file = 0;
	us num_files = 0, len_name;
	std::string name; std::vector<bool> code;
	in.read((char*)&num_files, sizeof(us)); /// read how many files the folder has
	in.read((char*)&len_name, sizeof(us));
	code.clear(); code.reserve(len_name); /// preparing code to read name
	readvector(in, code, len_name);
	stringfromvector(name, code, 0, len_name, root);
	fs::path direc_path = writefileshere.string() + "\\" + name;
	fs::create_directory(direc_path);
	for (us j = 0; j < num_files; ++j) /// we start reading the contents of the folder
	{
		in.read((char*)&file, sizeof(bool));  /// true-file false-folder
		if (file)
		{
			readfile(in, direc_path, root);
			total_number++;
		}
		else readfolder(in, root, direc_path,total_number);
	}
	
	return in;
}


void minheap::push( huffNode* test)
{
	data.push(test);
}

void minheap::pop()
{
	data.pop();
}

const huffNode* minheap::top() 
{
	return data.top();
}


