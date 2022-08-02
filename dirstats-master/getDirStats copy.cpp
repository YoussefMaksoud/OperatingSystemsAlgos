/// =========================================================================
/// Written by pfederl@ucalgary.ca in 2020, for CPSC457.
/// =========================================================================
/// You need to edit this file.
///
/// You can delete all contents of this file and start from scratch if
/// you wish, but you need to implement the getDirStats() function as
/// defined in "getDirStats.h".

#include "getDirStats.h"
#include "digester.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <stdio.h>
#include <algorithm>
#include <utility>
using namespace std;

//forward declaration of the comparator for the sort
bool comp(pair<string, int> a, pair<string, int> b);

static bool is_dir(const std::string &path)
{
	struct stat buff;
	if (0 != stat(path.c_str(), &buff))
		return false;

	return S_ISDIR(buff.st_mode);
}

// ======================================================================
// You need to re-implement this function !!!!
// ======================================================================
//
// getDirStats() computes stats about directory dir_name
// if successful, it return true and stores the results in 'res'
// on failure, it returns false, and res is in undefined state/

//comparator for the sort function on hash table
bool comp(pair<string, int> a, pair<string, int> b)
{
	return a.second > b.second;
}

bool getDirStats(const std::string &dir_name, Results &res)
{
	// The results below are all hard-coded, to show you all the fields
	// you need to calculate. You should delete all code below and
	// replace it with your own code.

	vector<string> stack;						  // stack for the directory paths
	unordered_map<string, int> common_types;	  // unordered map for the most common file types
	stack.push_back(dir_name);					  // addding the starting directory to the stack
	unordered_map<string, vector<string>> hashes; // unordered map of duplicate file groups
	vector<string> all_files;					  // vector of all files to compute the file types

	struct stat file_stat;
	FILE *fp;
	char buffer[128];

	res.largest_file_path = " ";
	res.largest_file_size = -1;
	res.n_files = 0;
	res.n_dirs = 0;
	res.all_files_size = 0;

	//while the stack still contains elements
	while (!stack.empty())
	{

		auto dirname = stack.back(); //get a directory
		stack.pop_back();			 //remove the last element

		DIR *dp = opendir(dirname.c_str()); // open the directory
		if (!dp)
			return false;

		if (dp)
		{ // if the directoy is open

			while (1)
			{
				dirent *ep = readdir(dp); // find an entry in the directory
				if (!ep)
					break;
				string path = dirname + "/" + ep->d_name; //create the path name
				if (stat(path.c_str(), &file_stat) < 0)
					cerr << "error"; // the the entry information

				//the code below gave the correct output however it made the program very very slow,
				//i adjusted the code by calling the popen sytem call outside of the while loop to reduce the time
				//the process waited for the subprocess popen to finish excecuting

				/*	string command = "file -b ";
			        string instruc = command + path;
				string result;
				
				fp = popen(instruc.c_str(), "r");
				
				while((fgets(buffer, sizeof buffer, fp))){
					
					result += buffer;
					if(common_types.find(result) == common_types.end()) common_types.emplace(result, 1);
					else common_types[result]++;
				}

				pclose(fp);
			*/
				//if the path leads to a directory
				if (is_dir(path.c_str()))
				{
					string name = ep->d_name;
					//if(name[0] == '.') continue;
					if (name.compare(".") == 0 || name.compare("..") == 0)
						continue;
					stack.push_back(path); //add it to the stack to be opened later
					res.n_dirs++;		   //increment the number of directories
				}
				//if the path leads to a file
				else if (ep->d_type == DT_REG)
				{
					res.all_files_size += file_stat.st_size; //add the file's size to total size
					all_files.push_back(path.c_str());		 // add the path to the all_files vector
					if (file_stat.st_size > res.largest_file_size)
					{ // and set the largest file in the directories
						res.largest_file_size = file_stat.st_size;
						res.largest_file_path = path;
					}

					res.n_files++; // increment the number of files

					string hash = sha256_from_file(path.c_str()); //get the hexadecimal digest of the file

					if (hashes.find(hash) == hashes.end())
					{											//organize the files by group using the hash as the key
						hashes.emplace(hash, vector<string>()); //creates a new group of duplicate files
						hashes[hash].push_back(path.c_str());
					}
					else
					{
						hashes[hash].push_back(path.c_str()); //adds file to existing group
					}
				}
			}
			closedir(dp); // close the directory
		}
	}

	//the following loop guesses the file types of the all_files vector and pushes them onto
	//comm_file_types.
	//this is the solution to the above commented code, it significantly speeds up the program
	for (auto &it : all_files)
	{
		string command = "file -b ";   // command to be used
		string instruc = command + it; //the terminal file(1) command with argument
		string result;

		fp = popen(instruc.c_str(), "r"); //start the subprocess

		//push the common_types onto the common_types
		while ((fgets(buffer, sizeof buffer, fp)))
		{

			result += buffer;
			if (common_types.find(result) == common_types.end())
				common_types.emplace(result, 1);
			else
				common_types[result]++;
		}

		pclose(fp); //close the file pointer
	}

	//the next loop takes the sorted vector of file types and pushes it onto res.common_file_types
	vector<pair<string, int>> elems(common_types.begin(), common_types.end());
	sort(elems.begin(), elems.end(), comp);

	string filetype;
	for (auto it = elems.begin(); it != elems.end(); it++)
	{
		filetype = it->first;
		res.most_common_types.push_back(filetype);
	}

	//the last loop uses the hashes table of duplicate files, finds the largest group in the map
	//the loop then pushes it to res.duplicate_files then erases the element from the map
	//to avoid returning the same group twice
	pair<string, long unsigned int> largest;
	for (long unsigned int i = 0; i < 5 && i < hashes.size(); i++)
	{
		largest = {" ", 0};
		for (auto it = hashes.begin(); it != hashes.end(); it++)
		{
			if (it->second.size() > largest.second)
			{
				largest = {it->first.c_str(), it->second.size()};
			}
		}
		if (largest.second > 1)
		{
			res.duplicate_files.push_back(hashes[largest.first.c_str()]);
		}
		hashes.erase(largest.first.c_str());
	}

	return true;
}
