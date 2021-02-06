#pragma once
#include<iostream>
#include<fstream>
#include<filesystem>
#include<queue>
#include<vector>
using std::cin;
using std::cout;
using std::endl;
namespace fs = std::filesystem;
typedef unsigned short us;
typedef std::vector	<std::vector<bool>> booltable;
class huffNode 
{
	unsigned char data;
	size_t freq;
	huffNode* left,* right;
public:
	huffNode();
	huffNode(huffNode* tleft, huffNode* tright);
	huffNode(const huffNode& other);
	huffNode(const char tdata, const size_t tfreq);
	huffNode& operator=(huffNode& other);
	bool leaf();
	unsigned char getdata() { return data; }
	const size_t  getfreq() const { return freq; }
	size_t		  getfreq() { return freq; }
	void		  add_leaves(huffNode* left, huffNode* right);
	huffNode*	  get_leftleaf() { return left; }
	huffNode*	  get_leftleaf()const { return left; }
	huffNode*	  get_rightleaf() { return right; }
	huffNode*	  get_rightleaf()const { return right; }
	void		  root_delete();
	const size_t  root_depth(huffNode*root)const;
};
struct compare
{
	bool operator()(const huffNode* l, const huffNode* r)const;
	bool operator()(huffNode* l,huffNode* r);
};
class minheap
{
	std::priority_queue<huffNode*, std::vector<huffNode*>, compare> data;
public:
	void push( huffNode* test);
	void pop();
	const huffNode* top();
	int getsize() { return data.size(); }
};
struct translatetree
{
	unsigned char data;
	translatetree* left, * right;
	translatetree() :data('\0'), left(nullptr), right(nullptr) {}
	
};

huffNode* maketree(std::vector<fs::path> paths);
size_t leafcount(huffNode* root);
int* getcounts(std::vector<fs::path> paths);
void make_huffman_code(minheap* pyramid);
void create_codes(huffNode* root,booltable& codes,std::vector<char>& chars);
void rec_lrr(huffNode* root, std::vector<bool>& current, booltable& codes, std::vector<char>& chars);
std::ofstream&  writecodes(std::ofstream& out, booltable codes, std::vector<char>chars, std::vector<fs::path> paths);
std::ofstream&  takeout(std::ofstream& out, booltable codes, std::vector<char> chars,fs::path path);
std::ofstream&  writelenght(std::string input, std::ofstream& out, std::vector<char> chars, booltable codes);
std::ofstream&  writevector(std::vector<bool> input, std::ofstream& out);
std::ofstream&  writestrings(std::string input, std::ofstream& out, std::vector<char> chars, booltable codes);
std::ofstream&  writefiles(fs::path path, std::ofstream& out,std::vector<char> chars, booltable codes);
std::ifstream&  readfile(std::ifstream& in, fs::path writefileshere,translatetree* root);
std::ifstream&  readvector(std::ifstream& in, std::vector<bool>& code, us size);
std::ifstream&  readname(std::ifstream& in, std::vector<bool>& code, size_t size);
std::ifstream&  readwholefile(std::ifstream& in, std::vector<bool>& code,size_t size);
std::ofstream&  fromvectofile(std::vector<bool> code, std::ofstream& out, translatetree* root,std::string name);
std::ifstream&  readfolder(std::ifstream& in, translatetree* root, fs::path writefileshere,us& total_number);
int				find(std::vector<char> chars, char wanted);
void			takein(std::ifstream& in,fs::path writefileshere);
void			takeonefile(std::ifstream& in, fs::path writefileshere, std::string filename);
translatetree*  buildtree(std::ifstream& in);
void			buildtranslationtree(translatetree* root, std::vector<bool> code, char current,us current_bit,us len_code);
void			stringfromvector(std::string& input, std::vector<bool> code, us current_bit, size_t size, translatetree* root);
void			charfromvec(std::string& input, std::vector<bool> code, us& current_bit, size_t size, translatetree* root);
void			killtree(translatetree* root);
void			print(std::ifstream& in);
std::ifstream&	printfile(std::ifstream& in,translatetree* root);
std::ifstream&  printfolder(std::ifstream& in, translatetree* root,us& total_number);