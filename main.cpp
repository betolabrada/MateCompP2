#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <map>
#include <fstream>
#include <iomanip>
#include <queue>


using namespace std;

class CFGrammar 
{
	vector<char> _terminals;
	vector<char> _nonterminals;
	multimap<char, string> _m;

public:
	void setTerminals(vector<char> terminals) { _terminals = terminals; }
	void setMap(multimap<char, string> m) { _m = m; }
	void printTerminals() { for (auto i : _terminals ) cout << i << ", "; }
	void printMap() { for (auto i : _m) cout << i.first << "->" << i.second << endl; }
	int getTerminalsNum() { return _terminals.size(); }
	multimap<char, string> getMap() { return _m; }


};

void printTable(vector<vector<string>> p, int n)
{
	cout << endl;

	for (int i = 1; i <= n; i++)
	{
		for (int j = 1; j <= n; j++)
		{
			cout << setw(5) << p[i][j];
		}
		cout << endl;
	}
}

bool isIn(char c, string str)
{
	for (char ch : str) if (ch == c) return true;
	return false;
}


string calculateTerminals(int i, int j, vector<vector<string>> mat, CFGrammar grammar)
{
	string combination = "";
	string terminals = "";
	multimap<char, string> t_map = grammar.getMap();

	for (int k = i; k < j; k++)
		for (char cha : mat[i][k])
			for (char chb : mat[k + 1][j])
			{
				combination += cha;
				combination += chb;
				for (auto trans : t_map)
				{
					if (trans.second.compare(combination) == 0)
					{
						if (!isIn(trans.first, terminals)) terminals += trans.first;
					}
				}
				combination.clear();
			}
	return terminals;
}
// construct tree (graph)
void createGraph(vector<vector<string>> mat, CFGrammar grammar)
{
	
}

void CYK(string str, CFGrammar grammar)
{
	// inicializar matriz nxn donde n es el numero de caracteres en string;
	int n = str.size();
	// matriz
	vector<vector<string>> mat(n+1);
	for (int i = 1; i <= n; i++)
	{
		mat[i] = vector<string>(n+1);
	}

	// initialize, by ChNF one nonterminal is produced by one terminal
	int i, j, diag;
	multimap<char, string> t_map = grammar.getMap();
	for (i = 1; i <= n; i++) {
		char t = str[i-1];
		for (auto trans : t_map)
		{
			if (trans.second[1] == t)
			{
				mat[i][i] += trans.first;
			}
		}
	}

	// algorithm
	for (diag = 1; diag <= n-1; diag++)
	{
		for (i = 1; i <= n - diag; i++)
		{
			j = i + diag;
			mat[i][j] = calculateTerminals(i, j, mat, grammar);
		}
	}
	printTable(mat, n);
	if (isIn('S', mat[1][n])) cout << "Cadena " + str + " pertenece a L(G)";
	else cout << "Cadena " + str + " no pertenece a L(G)";

	//// create graph
	//ofstream outputFile;
	//outputFile.open("graph.txt");
	//outputFile << grammar.getTerminalsNum() << endl;
	//queue<string> remaining;
	//remaining.push(mat[1][n]);
	//outputFile << mat[1][n]  << " ";
	//while (!remaining.empty)
	//{
	//	int diag = n - 1;
	//	string combination = "";
	//	string terminals = "";
	//	multimap<char, string> t_map = grammar.getMap();

	//	for (int k = i; k < j; k++)
	//	{
	//		if (mat[i][k].size() == 1 && mat[k + 1][j].size() == 1)
	//		{
	//			combination += mat[i][k] + mat[k + 1][j];
	//			for (auto trans : t_map)
	//			{
	//				if (trans.second.compare(combination) == 0)
	//				{
	//					if (!isIn(trans.first, terminals)) terminals += trans.first;
	//				}
	//			}

	//		}
	//		combination.clear();
	//	}
	//}
	
	//outputFile.close();
}



void traverse(vector<vector<char>> mat)
{

}


int main()
{
	vector<char> t_terminals;
	CFGrammar grammar;
	ifstream inputFile;
	multimap<char, string> t_m;
	string line;
	
	inputFile.open("fnch.txt");
	if (!inputFile)
	{
		cerr << "failed.";
		exit(1);
	}
	else
	{
		cout << "Gramatica en FNCh: " << endl;
		while (getline(inputFile, line))
		{
			cout << line << endl;
			char nont = line[0];
			string t = "";
			int pos = 3; // initial position of all productions
			while (line[pos] != '\0')
			{
				if (line[pos] == '|') 
				{
					pos++;
					t_m.insert(make_pair(nont, t));
					t.clear();
				}
				t += line[pos];
				pos++;
			}
			t_m.insert(make_pair(nont, t));
			t_terminals.push_back(nont);

			
		}
		grammar.setTerminals(t_terminals);
		//grammar.printTerminals();
		cout << endl;

		grammar.setMap(t_m);
		//grammar.printMap();

		string cadena;
		cout << "Escriba cadena: ";
		cin >> cadena;
		cout << "Algoritmo CYK: " << endl;
		CYK(cadena, grammar);

		inputFile.close();
	}


	return 0;
}