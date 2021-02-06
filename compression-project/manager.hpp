#pragma once
#include<iostream>
#include<fstream>
#include<string>
#include<filesystem>
#include"huffman coding.hpp"
void test();
void starter();
void createarc();
void decrypt();
void info();
void extractfile(std::string filename, std::ifstream& in, fs::path writefileshere);