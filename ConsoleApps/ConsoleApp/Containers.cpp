#include "Containers.h"
#include "CommonFunctions.h"

//https://docs.microsoft.com/it-it/cpp/standard-library/map-class?view=msvc-160
void TryMap()
{
	map <int, int> m1;

	map <int, int> ::iterator m1_Iter;
	map <int, int> ::const_iterator m1_cIter;
	typedef pair <int, int> Int_Pair;

	m1.insert(Int_Pair(0, 0));
	m1.insert(Int_Pair(1, 1));
	m1.insert(Int_Pair(2, 4));

	m1_cIter = m1.begin();
	cout << "The first element of m1 is " << m1_cIter->first << endl;

	m1_Iter = m1.begin();
	m1.erase(m1_Iter);

	// The following 2 lines would err because the iterator is const
	// m1_cIter = m1.begin ( );
	// m1.erase ( m1_cIter );

	m1_cIter = m1.begin();
	cout << "The first element of m1 is now " << m1_cIter->first << endl;

	//clear and size
	map<int, int> m2;
	map<int, int>::size_type i;
	typedef pair<int, int> Int_Pair;

	m2.insert(Int_Pair(1, 1));
	m2.insert(Int_Pair(2, 4));

	i = m2.size();
	cout << "The size of the map is initially "
		<< i << "." << endl;

	m2.clear();
	i = m2.size();
	cout << "The size of the map after clearing is "
		<< i << "." << endl;

	//count
	map<int, int> m3;
	map<int, int>::size_type i2;
	typedef pair<int, int> Int_Pair;

	m3.insert(Int_Pair(1, 1));
	m3.insert(Int_Pair(2, 1));
	m3.insert(Int_Pair(1, 4));
	m3.insert(Int_Pair(2, 1));

	// Keys must be unique in map, so duplicates are ignored
	i2 = m3.count(1);
	cout << "The number of elements in m3 with a sort key of 1 is: "
		<< i2 << "." << endl;

	i2 = m3.count(2);
	cout << "The number of elements in m3 with a sort key of 2 is: "
		<< i2 << "." << endl;

	i = m3.count(3);
	cout << "The number of elements in m3 with a sort key of 3 is: "
		<< i << "." << endl;

	//empty
	if (m1.empty())
		cout << "The map m1 is empty." << endl;
	else
		cout << "The map m1 is not empty." << endl;
}