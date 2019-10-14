#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <map>
#include <fstream>
#include <iomanip>
#include <queue>
#include <algorithm>



using namespace std;

class CFGrammar 
{
public:
	CFGrammar(vector<char> v, multimap<char, string> m);
	void CYK();
	string calculateTerminals(int i, int j, vector<vector<string>>& cykTable);
	bool isIn(char c, string str);
	int index(string str, vector<string>& strs);

	void printTerminals() { for (char ch : terminals) cout << ch << ","; }
	void printMap() { for (auto i : transitions) cout << i.first << "->" << i.second << endl; }

private:
	vector<char> terminals;
	multimap<char, string> transitions;
	void printTable(vector<vector<string>>&, int);
	void generateTree(vector<vector<string>>, int);


};

CFGrammar::CFGrammar(vector<char> v, multimap<char, string> m) : terminals(v), transitions(m) {}

void CFGrammar::CYK()
{
	// input de cadena
	string str;
	cout << "Escriba cadena: ";
	cin >> str;
	cout << "Algoritmo CYK: " << endl;

	// obtener tamanio de cadena
	int n = str.size();

	// mapa de transiciones
	multimap<char, string> t_map = transitions;

	// matriz shifteada (1...n)
	vector<vector<string>> mat(n + 1);
	for (int i = 1; i <= n; i++)
		mat[i] = vector<string>(n + 1);

	// initialize mat, by ChNF, one nonterminal is produced by one(or more) single terminal
	char t;
	for (int i = 1; i <= n; i++)
	{
		t = str[i - 1];
		for (auto trans : t_map)
			if (trans.second[1] == t)
				mat[i][i] += trans.first;
	}

	// algorithm DP
	int diag, i, j;
	for (diag = 1; diag <= n - 1; diag++)
		for (i = 1; i <= n - diag; i++)
		{
			j = i + diag;
			mat[i][j] = calculateTerminals(i, j, mat);
		}


	printTable(mat, n);
	if (isIn('S', mat[1][n]))
	{
		cout << "Cadena " + str + " pertenece a L(G)" << endl;
		generateTree(mat, n);
	}
	else cout << "Cadena " + str + " no pertenece a L(G)" << endl;
}

string CFGrammar::calculateTerminals(int i, int j, vector<vector<string>>& cykTable)
{
	// helper func which obtains the string of combining acceptable terminals
	string combination = "";
	string strTerminals = "";
	multimap<char, string> t_map = transitions;

	for (int k = i; k < j; k++)
		for (char cha : cykTable[i][k])
			for (char chb : cykTable[k + 1][j])
			{
				combination += cha;
				combination += chb;
				for (auto trans : t_map)
				{
					if (trans.second.compare(combination) == 0)
					{
						if (!isIn(trans.first, strTerminals)) strTerminals += trans.first;
					}
				}
				combination.clear();
			}
	return strTerminals;
}

bool CFGrammar::isIn(char c, string str)
{
	for (char ch : str) if (ch == c) return true;
	return false;
}

void CFGrammar::printTable(vector<vector<string>>& p, int n)
{
	// sort first

	for (int i = 1; i <= n; i++)
	{
		for (int j = 1; j <= n; j++)
		{
			if (p[i][j].size() > 1)
				sort(p[i][j].begin(), p[i][j].end());
			cout << setw(5) << p[i][j];
		}
		cout << endl;
	}
	cout << endl;
}

int CFGrammar::index(string str, vector<string>& strs)
{
	int c = 0;
	strs.push_back(str);
	for (string s : strs)
		if (s.compare(str) == 0)
			c++;
	return c;
}


void CFGrammar::generateTree(vector<vector<string>> p, int n)
{
	
	// a pair: name of terminal, pair cell i, j that was obtained
	typedef pair<pair<string,int>, pair<int, int>> cell;

	// queue of terminals to be processed
	queue<cell> remaining;

	// dec variables
	int i, j;
	string combination = "";
	string terminals = "";

	// get transitions
	multimap<char, string> t_map = transitions;
	
	// initialize
	// add S to current terminal
	cell current = make_pair(make_pair(p[1][n], 1), make_pair(1, n));
	// add to queue to process its children
	remaining.push(current);

	vector<string> nodes;
	nodes.push_back(current.first.first);
	// algorithm
	while (!remaining.empty())
	{
		cout << endl;
		current = remaining.front(); // terminal a procesar
		cout << current.first.first + to_string(current.first.second) << ": ";
		remaining.pop();
		// i, j position where is located in p (cykTable)
		i = current.second.first;
		j = current.second.second;
		if (i == j)
		{
			// find terminal
			for (auto trans : t_map)
			{
				if (trans.first == current.first.first[0])
				{
					// add terminal node;
					cell t = make_pair(make_pair(trans.second, -1), make_pair(i, j));
					if (trans.second[0] == '"')
						cout << t.first.first;
				}
			}
		}
		else
		{

			bool inserted = false;
			for (int k = i; k < j; k++)
			{
				for (char cha : p[i][k])
				{
					for (char chb : p[k + 1][j])
					{
						combination += cha;
						combination += chb;
						for (auto trans : t_map)
						{
							if (trans.second.compare(combination) == 0)
							{
								// c1, c2 are the index that is printed for each node
								// makes the drawing of tree easier
								int c1 = index(p[i][k], nodes);
								int c2 = index(p[k + 1][j], nodes);
								cell t1 = make_pair(make_pair(p[i][k], c1), make_pair(i, k));
								cell t2 = make_pair(make_pair(p[k+1][j], c2), make_pair(k + 1, j));
								remaining.push(t1);
								remaining.push(t2);
								
								cout << t1.first.first + to_string(t1.first.second) << " ";
								cout << t2.first.first + to_string(t2.first.second);
								inserted = true;
								break;
							}
						}
						combination.clear();
						if (inserted) break;
					}
					if (inserted) break;
				}
				if (inserted)
				{
					inserted = false;
					break;
				}
			}
		}
	}
	cout << endl;
}
#define ARCHIVO "fnch.txt"

int main()
{
	vector<char> t_terminals;
	ifstream inputFile;
	multimap<char, string> t_m;
	string line;
	
	inputFile.open(ARCHIVO);
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

		CFGrammar grammar(t_terminals, t_m);

		//grammar.printTerminals();

		//grammar.printMap();

		grammar.CYK();

		inputFile.close();
	}


	return 0;
}