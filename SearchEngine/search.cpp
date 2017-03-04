#include<algorithm>
#include<vector>
#include<hash_map>
#include<string>
#include<iostream>
#include<iterator>
#include <fstream>
#include<cstdlib>
#include<map>
#include<utility>
#include <cstring>
#include "Stemming.h"

using namespace std;

/*
struct string_less
{
int dd;

size_t operator()()const
{
return dd;
}
bool operator == (const string_less &lhs)const
{
return this->dd == lhs.dd;
}
bool operator()(const string_less &lhs)const
{
return this->dd == lhs.dd;
}
};
*/
struct string_less : public binary_function<const string, const string, bool>//the compare function for the hash_map
{
public:
	result_type operator()(const first_argument_type& _Left, const second_argument_type& _Right) const
	{
		return(_Left.compare(_Right) < 0 ? true : false);
	}
};

class vocabulary//the data struct inserted into the hash_map
{
public:
	int freq;
	map<string,vector<int>> sour;
};
typedef vocabulary* word;

vector<string> stopword;//the stopword vector
hash_map<string, word, hash_compare<string, string_less> > Index;//the global hash_map variable
vector<string>enquire;//the input vector


void Insert(string voc, string filename, int address, hash_map<string, word, hash_compare<string, string_less> >& Index);//insert a item when build the index
void BuildIndex(hash_map<string, word, hash_compare<string, string_less> > &Index);// build the index
void collect_stopword();//read the stop word into a vector
bool isstopword(string W);//judge if a word is a stopword
string Filename(int N, int M);//build a filename
bool isword(char ch);//judge if a char is vaild
void process(string INP);//process the input into the vector<string>enquire
void SearchEach(string INP, hash_map<string, word, hash_compare<string, string_less> > &Index);//search each word one by one
void SearchAll(string INP, hash_map<string, word, hash_compare<string, string_less> > &Index);//search the string
void EnterToContinue();

void Insert(string voc, string filename, int address, hash_map<string, word, hash_compare<string, string_less> >& Index)
{
	if (Index.find(voc) == Index.end())//judge if the word has been in the index,if not,create a new word
	{
		word NewW = new vocabulary;
		NewW->freq = 1;
		NewW->sour[filename].push_back(address);
		Index.insert(make_pair(voc,NewW));
	}
	else//if yes, update the word item
	{
		Index[voc]->freq++;
		if (Index[voc]->sour.find(filename) == Index[voc]->sour.end())
		{
			vector<int> NA;
			NA.push_back(address);
			Index[voc]->sour.insert(make_pair(filename, NA));
		}
		else
		{
			Index[voc]->sour[filename].push_back(address);
		}
	}
}

void BuildIndex(hash_map<string, word, hash_compare<string, string_less> > &Index)
{
	int cou;
	int num;

	string filename="";
	ifstream fp;
	string voc="";
	int address;
	char ch;
	for (cou = 1001; cou <= 1042; cou++)//read every file
	{

		for (num = 1; num <= 153; num++)
		{
			filename=Filename(cou, num);//build the file name
			fp.open("Shakespeare/"+filename,ios_base::in|ios_base::binary);
			if (!fp.is_open())
				continue;
			cout << filename << endl;
			address = 0;
			fp.get(ch);
			while (fp)//read words one by one
			{
			    voc = "";

				while (!isword(ch) && !fp.eof())
				{
					fp.get(ch);
				}

				if (!fp)
					break;

				while (isword(ch) && !fp.eof())
				{
					voc += ch;
					fp.get(ch);
				}
				transform(voc.begin(), voc.end(), voc.begin(), tolower);//transform the word into small letter
				address++;

				if (!isstopword(voc))
				{
					Insert(voc, filename, address,Index);
				}
				
			}
			fp.close();
		}
	}

}

void collect_stopword()
{
	ifstream fp;
	string stop;
	char ch;
	fp.open("stopword.txt" , ios_base::in | ios_base::binary);
	if (!fp)
	{
		cout << "read stopword error" << endl;
		return;
	}

	fp.get(ch);
	while (fp)
	{
		stop = "";

		while (!isword(ch) && !fp.eof())
		{
			fp.get(ch);
		}

		if (!fp)
			break;

		while (isword(ch) && !fp.eof())
		{
			stop += ch;
			fp.get(ch);
		}
		transform(stop.begin(), stop.end(), stop.begin(), tolower);

		stopword.push_back(stop);
	}
}

bool isstopword(string W)
{
	if (find(stopword.begin(), stopword.end(), W) == stopword.end())
		return 0;
	else
		return 1;
}

string Filename(int N, int M)
{
	string res;
	char file[15];
	char chap[10];
	_itoa_s(N, file, 10);
	_itoa_s(M, chap, 10);
	strcat_s(file, 15, " ");
	strcat_s(file, 15, chap);
	strcat_s(file, 15, ".txt");
	res=file;
	return res;
}

bool isword(char ch)
{
	int D = ((ch >= 'a' && ch<='z') || (ch>='A'&& ch <= 'Z') || (ch == '\''));
	return ((D==1)?TRUE:FALSE);
}

void process(string INP)
{
	enquire.clear();
	string WORD="";
	string::iterator SP = INP.begin();
	while (SP!=INP.end())
	{
		WORD = "";

		while (!isword(*SP) && SP != INP.end())
		{
			++SP;
		}

		if (SP == INP.end())
			break;

		while (SP != INP.end() && isword(*SP))
		{
			WORD += (*SP);
			SP++;
		}
		transform(WORD.begin(), WORD.end(), WORD.begin(), tolower);
		if (!isstopword(WORD))
		{
			enquire.push_back(WORD);
		}
		

	}
}

void SearchEach(string INP, hash_map<string, word, hash_compare<string, string_less> > &Index)
{
	process(INP);
	string str;
	if (enquire.empty())
	{
		cout << "invalid input" << endl;
	}
	vector<string>::iterator SP;
	map<string, vector<int>>::iterator MP;
	vector<int>::iterator AP;
	for (SP = enquire.begin(); SP != enquire.end(); ++SP)//search words one by one
	{
		str = *SP;
		if (Index.find(str) == Index.end())//if there is no such word
			cout << "No result for " << str << endl;
		else
		{
			cout << "the word "<<str<<" appear " << Index[str]->freq << " times :" << endl;
			for (MP = Index[str]->sour.begin(); MP != Index[str]->sour.end(); ++MP)//output files containing the word
			{
				cout << "In file " << MP->first << " at the ";
				for (AP = MP->second.begin(); AP != MP->second.end(); ++AP)//output the position the word exist in
				{
					cout << *AP<<" ";
				}
				cout << "position" << endl;
			}


		}
		cout << endl;
	}

	cout << "search finished" << endl;
}

void SearchAll(string INP, hash_map<string, word, hash_compare<string, string_less> > &Index)
{
	int pos;
	char ch;
	int num;
	bool flag=0;
	int reach = 0;
	vector<string> result;
	string filename;
	process(INP);
	int numE = enquire.size();
	string str;
	string voc="";
	ifstream fp;
	vector<string>::iterator SP;
	map<string, vector<int>>::iterator MP;
	vector<int>::iterator AP;
	int sui;


	if (enquire.empty())
	{
		cout << "invalid input" << endl;
	}

	SP = enquire.begin();
	str = *SP;
	if (Index.find(str) == Index.end())
		cout << "No result "<< endl;
	else//if the first word(which is not a stopword) in input exist in file
	{
		for (MP = Index[str]->sour.begin(); MP != Index[str]->sour.end(); ++MP)//search each files containing the word
		{
			filename = MP->first;
			fp.open("Shakespeare/" + filename, ios_base::in | ios_base::binary);
			num = 0;
			for (AP = MP->second.begin(); AP != MP->second.end(); ++AP)//output the position the word exist in
			{
				pos = *AP;
				if (num >= pos)
				{
					num = pos - 1;
					fp.seekg(sui);
				}
				if (!fp)
				{
					cout << "read file fail" << endl;
					continue;
				}
				fp.get(ch);
				reach = 0;
				while(!fp.eof() && result.size()<enquire.size())//collect the string that following the word same with the the first word(which is not a stopword) in input
				{
						voc = "";

						while (!isword(ch) && !fp.eof())
						{
							fp.get(ch);
						}

						if (!fp)
							break;

						while (isword(ch) && !fp.eof())
						{
							voc += ch;

							fp.get(ch);
						}
						transform(voc.begin(), voc.end(), voc.begin(), tolower);
                        num++;
						if (reach == 1 && voc == str)
						{
							reach++;
							sui = fp.tellg();
							sui -= str.size();
						}
                        if (num == pos)
							{
							    reach = 1;
							}
						
						if ((reach>0) && !isstopword(voc) )
						{							
							result.push_back(voc);
						}
					
				}
				if (!enquire.empty() && equal(enquire.begin(), enquire.end(), result.begin()))//judge if the string is the same with the input string
				{
					cout << "the string appear in " << filename << " at " << pos << " word " << endl;
					flag = 1;
				}

				result.clear();
			}
			fp.close();
		}
		if (flag == 0)
			cout << "No result" << endl;

	}
	cout << "search finished" << endl;
}


void EnterToContinue()
{
	cout << "请按ENTER键继续" << endl;
	while (cin.get() != '\n');
}

int main()
{
	cout << "stopword建立中..." << endl;
	collect_stopword();
	cout << "stopword建立完成" << endl;


	cout << "倒排索引建立中..." << endl;
	BuildIndex(Index);
	cout << "倒排索引建立完成" << endl;


	string INP;
	string mode;
	while (1)
	{
		cout << "请输入查找模式：  1. 词汇挨个查找      2. 全文查找     0. 退出" << endl;
		mode = "3";
		getline(cin, mode);
		
		if (!mode.compare("0"))
			break;
		else if (!mode.compare("1"))
		{
			cout << endl << "请输入需要查找的内容：" << endl;
			getline(cin, INP);
			SearchEach(INP, Index);
		}
		else if (!mode.compare("2"))
		{
			cout << endl << "请输入需要查找的内容：" << endl;
			getline(cin, INP);
			SearchAll(INP, Index);
		}
		else
		{
			cout << "输入有误，请重新输入" << endl;
		}

	}

}