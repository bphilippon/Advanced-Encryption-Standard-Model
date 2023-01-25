#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
using namespace std;

bool parity(int num);
int rgfMul(int c, int mulNum);

int main()
{
	//PART A

	//Read plaintext in from the file
	ifstream input_text;
	string plaintext_file;
	cout << "Enter the name of the input plaintext file: ";
	cin >> plaintext_file;
	input_text.open(plaintext_file);
	if(!input_text.is_open())
	{
		return 1;	//EXIT_FAILURE
	}

	//Initialize values for scanning
	string plaintext;
	char tempChar;

	//Scan file by alpha char and create string of plaintext
	while(input_text.get(tempChar))
	{
		if(isalpha(tempChar))
			plaintext.push_back(tempChar);
	}

	//Read key from input file
	ifstream input_key;
	string keytext_file;
	cout << "Enter the name of the input key file: ";
	cin >> keytext_file;
	input_key.open(keytext_file);
	if(!input_key.is_open())
	{
		return 1;	//EXIT_FAILURE
	}

	//Initialize values for scanning
	string keytext;

	//Scan file by alpha char and create string of key
	while(input_key.get(tempChar))
	{
		if(isalpha(tempChar))
			keytext.push_back(tempChar);
	}

	//Initialize output file
	ofstream fout;
	string output_file;
	cout << "Enter the name of the output ciphertext file: ";
	cin >> output_file;
	fout.open(output_file);

	cout << "Preprocessing:\n" << plaintext << endl << endl;
        fout << "Preprocessing:\n" << plaintext << endl << endl;

	//Initialize ciphertext values
	string ciphertext;
	char plainChar;
	char keyChar;
	int j = 0;
	int alphaIndex;

	//PART B
	for(int i = 0; i < plaintext.size(); i++)
	{
		plainChar = plaintext[i];		//Collect both plain and key char
		if(j == keytext.size())
			j = 0;				//Check boundary condition
		keyChar = keytext[j];

		//Vigenere Cipher each character
		alphaIndex = (int(plainChar) - 65) + (int(keyChar) - 65);
		alphaIndex = (alphaIndex % 26) + 65;

		ciphertext.push_back(char(alphaIndex));	//Push char to cipher

		j++;
	}

	cout << "Substitution:\n" << ciphertext << endl << endl;
        fout << "Substitution:\n" << ciphertext << endl << endl;

	//PART C

	//Define remaining pad length and pad ciphertext
	int padlen = 16 - (ciphertext.size() % 16);
	ciphertext.append(padlen, 'A');
	int lineBreak = 0;

	//Initialize cipher vector
	vector<int> tempVec;
	vector<int>::iterator iter;
	vector<vector<int>> cipherVec;

	cout << "Padding:" << endl;
	fout << "Padding:" << endl;

	//Store and display cipher post-padding
	for(int i = 0; i < ciphertext.size(); i += 4)
	{
		for(int j = 0; j < 4; j++)				//Divide ciphertext into 4 char groups
			tempVec.push_back(int(ciphertext[i + j]));
		if(lineBreak == 4)					//Line breaks every 4 lines
		{
			cout << endl;
			fout << endl;
			lineBreak = 0;
		}

		for(iter = tempVec.begin(); iter < tempVec.end(); iter++)
		{
			cout << char(*iter);
			fout << char(*iter);
		}
		cout << endl;
		fout << endl;

		lineBreak++;
		cipherVec.push_back(tempVec);	//Push groupings of 4 into cipher vector
		tempVec.clear();
	}
	cout << endl;
	fout << endl;

	//PART D

	int shift = 0;
	int tempNum;

	//Perform row shifting
	vector<vector<int>>::iterator row;
	for(int i = 0; i < cipherVec.size(); i++)
	{
		for(int j = 0; j < shift; j++)		//Pop first element, push to back
		{
			tempNum = cipherVec[i][0];
			cipherVec[i].erase(cipherVec[i].begin());
			cipherVec[i].push_back(tempNum);
		}

		if(shift < 3)				//Shift by line break
			shift++;
		else
			shift = 0;
	}

	cout << "ShiftRows:" << endl;
        fout << "ShiftRows:" << endl;

	//Print row shifting
	lineBreak = 0;
	for(int i = 0; i < cipherVec.size(); i++)
	{
		if(lineBreak == 4)		//Line breaks every 4 lines
                {
                        cout << endl;
			fout << endl;
                        lineBreak = 0;
                }

		for(iter = cipherVec[i].begin(); iter < cipherVec[i].end(); iter++)
		{
			cout << char(*iter);
			fout << char(*iter);
		}
		cout << endl;
		fout << endl;

		lineBreak++;
	}
	cout << endl;
	fout << endl;

	//PART E

	//Perform parity

	bool checkParity;
	int mask = 128;
	for(int i = 0; i < cipherVec.size(); i++)
	{
		for(j = 0; j < cipherVec[0].size(); j++)
		{
			checkParity = parity(cipherVec[i][j]); 	//Saves odd or even parity
			if(checkParity)
				cipherVec[i][j] |= mask;	//Makes MSG 1 if odd parity
		}
	}

	cout << "Parity:" << endl;
	fout << "Parity:" << endl;

        for(int i = 0; i < cipherVec.size(); i++)
        {
                for(int j = 0; j < cipherVec[0].size(); j++)
		{
                        cout << hex << cipherVec[i][j] << " ";	//Print hex values
			fout << hex << cipherVec[i][j] << " ";
		}
                cout << endl;
		fout << endl;
        }
	cout << endl;
	fout << endl;

	//PART F

	//Perform mix columns

	int c0;		//Declares values for column values
	int c1;
	int c2;
	int c3;
	for(int col = 0; col < cipherVec[0].size(); col++)
	{
		for(int row = 0; row < cipherVec.size(); row +=4)
		{
			//Initializes column values
			c0 = cipherVec[row][col];
			c1 = cipherVec[row + 1][col];
			c2 = cipherVec[row + 2][col];
			c3 = cipherVec[row + 3][col];

			//Perform Rijndael Galois algorithm
			cipherVec[row][col] = rgfMul(c0, 2) ^ rgfMul(c1, 3) ^ c2 ^ c3;
			cipherVec[row + 1][col] = c0 ^ rgfMul(c1, 2) ^ rgfMul(c2, 3) ^ c3;
			cipherVec[row + 2][col] = c0 ^ c1 ^ rgfMul(c2, 2) ^ rgfMul(c3, 3);
			cipherVec[row + 3][col] = rgfMul(c0, 3) ^ c1 ^ c2 ^ rgfMul(c3, 2);
		}
	}

	cout << "MixColumns:" << endl;
	fout << "MixColumns:" << endl;

	for(int i = 0; i < cipherVec.size(); i++)
        {
                for(int j = 0; j < cipherVec[0].size(); j++)
                {
                        cout << hex << cipherVec[i][j] << " ";	//Print hex values
                        fout << hex << cipherVec[i][j] << " ";
                }
                cout << endl;
                fout << endl;
        }
        cout << endl;
	fout << endl;

	input_text.close();
	input_key.close();
	fout.close();

	return 0;
}

//Returns the parity of the integer
bool parity(int num)
{
	bool parity = 0;
	while(num)
	{
		parity = !parity;
		num = num & (num - 1);
	}
	return parity;
}

//Performs Rijndael Galois algorithm on value in column
int rgfMul(int c, int mulNum)
{
	int result;
	switch(mulNum)	//Only 2 and 3 are  acceptable
	{
		case 2:
		{
			result = c << 1;
			break;
		}
		case 3:
		{
			result = (c << 1) ^ c;
			break;
		}
		default:
			break;
	}

	if(c & 128)			//If MSB of c is 1
		result ^= 27;
	if(result >= 256)		//If resultant is out of range
		result -= 256;

	return result;
}
