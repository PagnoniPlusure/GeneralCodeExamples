#include"PlaceHolderSubs.h"

void SubstitutePHs(EExcel::RangePtr ptrRange)
{
	vector<tuple<string, string>> placeHolders;

	placeHolders.push_back(std::make_tuple("<title>", "LEONARDO ELECTRONICS REPORT SAMPLE"));
	placeHolders.push_back(std::make_tuple("<docNumber>", "PG10004.08.2237TR"));
	placeHolders.push_back(std::make_tuple("<docRev>", "02.00"));
	placeHolders.push_back(std::make_tuple("<docDate>", "25/06/2021"));
	placeHolders.push_back(std::make_tuple("<cageCode>", "A1615"));
	placeHolders.push_back(std::make_tuple("<customer>", "PD&CM"));
	placeHolders.push_back(std::make_tuple("<projName>", "REPORT TCU"));
	placeHolders.push_back(std::make_tuple("<system>", ""));
	placeHolders.push_back(std::make_tuple("<subSystem>", ""));
	placeHolders.push_back(std::make_tuple("<cdrl>", "N/A"));
	placeHolders.push_back(std::make_tuple("<contractId>", ""));
	placeHolders.push_back(std::make_tuple("<unclassPages>", "10"));
	placeHolders.push_back(std::make_tuple("<unclassConPages>", ""));
	placeHolders.push_back(std::make_tuple("<restrPages>", ""));
	placeHolders.push_back(std::make_tuple("<confidPages>", ""));
	placeHolders.push_back(std::make_tuple("<secretPages>", ""));
	placeHolders.push_back(std::make_tuple("<totPages>", "10"));
	placeHolders.push_back(std::make_tuple("<telAuthor>", "+39 06 41503294"));
	placeHolders.push_back(std::make_tuple("<emailAuthor>", "stefano.quartucci@leonardocompany.com"));
	placeHolders.push_back(std::make_tuple("<author>", "Stefano Quartucci"));

	for (auto&& tuple : placeHolders)
	{
		string key, value;
		std::tie(key, value) = tuple;

		std::cout << key << " : " << value << std::endl;

		ptrRange->Replace(key.c_str(), value.c_str());
	}
}