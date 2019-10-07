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
public:
	CFGrammar(vector<char> v, multimap<char, string> m);
	void CYK();
	string calculateTerminals(int i, int j, vector<vector<string>>& cykTable);
	bool isIn(char c, string str);

	void printTerminals() { for (char ch : terminals) cout << ch << ","; }
	void printMap() { for (auto i : transitions) cout << i.first << "->" << i.second << endl; }

private:
	vector<char> terminals;
	multimap<char, string> transitions;
	void printTable(vector<vector<string>>, int);
	void generateTree(vector<vector<string>>, int);
	void printTree();


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

void CFGrammar::printTable(vector<vector<string>> p, int n)
{
	for (int i = 1; i <= n; i++)
	{
		for (int j = 1; j <= n; j++)
			cout << setw(5) << p[i][j];
		cout << endl;
	}
	cout << endl;
}

void CFGrammar::generateTree(vector<vector<string>> p, int n)
{
	
	// a pair: name of terminal, pair cell i, j that was obtained
	typedef pair<string, pair<int, int>> terminalCell;

	// queue of terminals to be processed
	queue<terminalCell> remaining;

	// dec variables
	int i, j;
	string combination = "";
	string terminals = "";

	// get transitions
	multimap<char, string> t_map = transitions;
	
	// initialize
	// add S to current terminal
	terminalCell current = make_pair(p[1][n], make_pair(1, n));
	// add to queue to process its children
	remaining.push(current);

	// algorithm
	while (!remaining.empty())
	{
		cout << endl;
		current = remaining.front(); // terminal a procesar
		cout << current.first << ": ";
		remaining.pop();
		// i, j position where is located in p (cykTable)
		i = current.second.first;
		j = current.second.second;
		if (i == j)
		{
			// find terminal
			for (auto trans : t_map)
			{
				if (trans.first == current.first[0])
				{
					// add terminal node;
					terminalCell t = make_pair(trans.second, make_pair(i, j));
					cout << t.first;
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
								terminalCell t1 = make_pair(p[i][k], make_pair(i, k));
								terminalCell t2 = make_pair(p[k+1][j], make_pair(k + 1, j));
								remaining.push(t1);
								remaining.push(t2);
								
								cout << t1.first << " ";
								cout << t2.first;
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

int main()
{
	vector<char> t_terminals;
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

		CFGrammar grammar(t_terminals, t_m);

		//grammar.printTerminals();

		//grammar.printMap();

		grammar.CYK();

		inputFile.close();
	}


	return 0;
}