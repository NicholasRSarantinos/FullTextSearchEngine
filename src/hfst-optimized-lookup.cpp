
#include "stdafx.h"
#include <iostream>

using namespace std;

//input
bool get_first_result_else_shorter;
bool kill_word_outside_of_dictionary;

#include "definitions.h"

string 
	base_work_directory_hfst = DEF_base_work_directory, 
	data_file_dictionary_directory_hfst = DEF_data_file_dictionary_directory;

#include "hfst-optimized-lookup.h"



class data_sorter_2
{
private:
	template <typename T1>
	static T1 item_max(T1 &x, T1 &y, bool &sort_desc)
	{
		if(sort_desc)
		{
			if(x > y) return x;
			else return y;
		}
		else
		{
			if(x < y) return x;
			else return y;
		}
	}

	template <typename T1, typename T2, typename T3>
	static void run_merge(T1 *&array_1, T2 *&array_2, T3 *&array_3, unsigned int left, unsigned int right, T1 *&scratch_1, T2 *&scratch_2, T3 *&scratch_3, bool &sort_desc)
	{
		/* base case: one element */
		if(right == left + 1)
			return;
		else
		{
			unsigned int i = 0;
			unsigned int length = right - left;
			unsigned int midpoint_distance = length/2;
			/* l and r are to the positions in the left and right subarrays */
			unsigned int l = left, r = left + midpoint_distance;

			/* sort each subarray */
			run_merge(array_1, array_2, array_3, left, left + midpoint_distance, scratch_1, scratch_2, scratch_3, sort_desc);
			run_merge(array_1, array_2, array_3, left + midpoint_distance, right, scratch_1, scratch_2, scratch_3, sort_desc);

			/* merge the arrays together using scratch for temporary storage */ 
			for(i = 0; i < length; i++)
			{
				/* Check to see if any elements remain in the left array; if so,
					* we check if there are any elements left in the right array; if
					* so, we compare them.  Otherwise, we know that the merge must
					* use take the element from the left array */
				if(l < left + midpoint_distance && (r == right || item_max(array_1[l], array_1[r], sort_desc) == array_1[l]))
				{
					scratch_1[i] = array_1[l];
					if(array_2 != nullptr) scratch_2[i] = array_2[l];
					if(array_3 != nullptr) scratch_3[i] = array_3[l];

					l++;
				}
				else
				{
					scratch_1[i] = array_1[r];
					if(array_2 != nullptr)  scratch_2[i] = array_2[r];
					if(array_3 != nullptr)  scratch_3[i] = array_3[r];

					r++;
				}
			}
			/* Copy the sorted subarray back to the input */
			for(i = left; i < right; i++)
			{
				array_1[i] = scratch_1[i - left];
				if(array_2 != nullptr) array_2[i] = scratch_2[i - left];
				if(array_3 != nullptr) array_3[i] = scratch_3[i - left];
			}
		}
	}

	template <typename T1, typename T2, typename T3>
	static void run_sort(T1 *array_1, T2 *array_2, T3 *array_3, unsigned int array_size, bool sort_desc)
	{
		T1* scratch_1 = new T1[array_size];
		T2* scratch_2 = (array_2 != nullptr) ? new T2[array_size] : nullptr;
		T3* scratch_3 = (array_3 != nullptr) ? new T3[array_size] : nullptr;

		unsigned int start_pos = 0;
		run_merge(array_1, array_2, array_3, start_pos, array_size, scratch_1, scratch_2, scratch_3, sort_desc);

		delete []scratch_1;
		if(scratch_2 != nullptr) delete []scratch_2;
		if(scratch_3 != nullptr) delete []scratch_3;
	}

public:
	template <typename T1, typename T2, typename T3>
	static void sort(T1 *array_1, T2 *array_2, T3 *array_3, unsigned int array_size, bool sort_desc = true)
	{
		run_sort(array_1, array_2, array_3, array_size, sort_desc);
	}

	template <typename T1, typename T2>
	static void sort(T1 *array_1, T2 *array_2, unsigned int array_size, bool sort_desc = true)
	{
		char* array_3 = nullptr;
		run_sort(array_1, array_2, array_3, array_size, sort_desc);
	}

	template <typename T1>
	static void sort(T1 *array_1, unsigned int array_size, bool sort_desc = true)
	{
		char* array_2 = nullptr;
		char* array_3 = nullptr;
		run_sort(array_1, array_2, array_3, array_size, sort_desc);
	}
} sort_arrays_2;



//
#ifdef DICTIONARY_CREATION

FILE * f = fopen((base_work_directory_hfst+data_file_dictionary_directory_hfst+"\\hfst-english-bnc\\english-bnc.hfstol").c_str(), "rb");
TransducerHeader header(f);
TransducerAlphabet alphabet(f, header.symbol_count());
TransducerW TransducerW_C(f, header, alphabet);

char * str;
char * old_str;
SymbolNumber * input_string;

//output
bool simplification_result;
string correct_word;

//double word_quality;

//catch the last 10.000 words to improve response speed
//char ***catched_words;
//int *catched_first_words_hashes;
//unsigned int catched_words_filled = 0, catched_words_max_size = 10000, next_catched_word_replace = 0;
//bool *catched_words_param;
//bool data_array_sorted = false;
//int *sort_positions;//sort sort_positions based on catched_words_hashes[0]

#else

int words_pairs_available;
char **word_pair_a, **word_pair_b;
int *word_pair_a_hashes;
char *hash_existance_check; 

#endif

void remove_everything_after_special_char_and_lower(string &input, char special_char = '<')
{
	//WE REMOVE EVERYTHING AFTER THE 1st < (if it exists). if on the owrd there is ANY special character (' ', '-', ''', etc) we drop the word
	int special_char_pos = -1;

	for(int i = 0; i < input.size(); i++)
	{
		if(input[i] >= 'A' && input[i] <= 'Z') input[i] += ('a'-'A');//just to be safe, if the input is not converted

		if(input[i] < 'a' || input[i] > 'z')//not a letter
		{
			if(input[i] == special_char)//great job, the word has no weird and unexpected special characters
			{
				special_char_pos = i;
				break;
			}
			else//the word has unexpected special characters, dop it
			{
				input = "";
				return;
			}
		}
	}

	if(special_char_pos != -1)
		input = input.substr(0, special_char_pos);
}

void TransducerHeader::skip_hfst3_header(FILE * f)
{
    const char* header1 = "HFST";
    unsigned int header_loc = 0; // how much of the header has been found
    int c;
    for(header_loc = 0; header_loc < strlen(header1) + 1; header_loc++)
    {
	c = getc(f);
	if(c != header1[header_loc]) {
	    break;
	}
    }
    if(header_loc == strlen(header1) + 1) // we found it
    {
	unsigned short remaining_header_len;
	if (fread(&remaining_header_len,
		  sizeof(remaining_header_len), 1, f) != 1 ||
	    getc(f) != '\0') {
	    throw HeaderParsingException();
	}
	char * headervalue = new char[remaining_header_len];
	if (fread(headervalue, remaining_header_len, 1, f) != 1)
	{
	    throw HeaderParsingException();
	}
	if (headervalue[remaining_header_len - 1] != '\0') {
	    throw HeaderParsingException();
	}
	std::string header_tail(headervalue, remaining_header_len);
	size_t type_field = header_tail.find("type");
	if (type_field != std::string::npos) {
	    if (header_tail.find("HFST_OL") != type_field + 5 &&
		header_tail.find("HFST_OLW") != type_field + 5) {
		delete headervalue;
		throw HeaderParsingException();
	    }
	}
    } else // nope. put back what we've taken
    {
	ungetc(c, f); // first the non-matching character
	    for(int i = header_loc - 1; i>=0; i--) {
// then the characters that did match (if any)
		ungetc(header1[i], f);
	    }
    }
}

void TransducerAlphabet::get_next_symbol(FILE * f, SymbolNumber k)
{
  int byte;
  char * sym = line;
  while ( (byte = fgetc(f)) != 0 )
    {
      if (byte == EOF)
	{
	  std::cerr << "Could not parse transducer; wrong or corrupt file?" << std::endl;
	  exit(1);
	}
      *sym = byte;
      ++sym;
    }
  *sym = 0;
  if (strlen(line) >= 5 && line[0] == '@' && line[strlen(line) - 1] == '@' && line[2] == '.')
    { // a special symbol needs to be parsed
      std::string feat;
      std::string val;
      FlagDiacriticOperator op = P; // g++ worries about this falling through uninitialized
      switch (line[1]) {
      case 'P': op = P; break;
      case 'N': op = N; break;
      case 'R': op = R; break;
      case 'D': op = D; break;
      case 'C': op = C; break;
      case 'U': op = U; break;
      }
      char * c = line;
      // as long as we're working with utf-8, this should be ok
      for (c +=3; *c != '.' && *c != '@'; c++) { feat.append(c,1); }
      if (*c == '.')
	{
	  for (++c; *c != '@'; c++) { val.append(c,1); }
	}
      if (feature_bucket.count(feat) == 0)
	{
	  feature_bucket[feat] = feat_num;
	  ++feat_num;
	}
      if (value_bucket.count(val) == 0)
	{
	  value_bucket[val] = val_num;
	  ++val_num;
	}
      operations.push_back(FlagDiacriticOperation(op, feature_bucket[feat], value_bucket[val]));
      kt->operator[](k) = _strdup("");
      
#if OL_FULL_DEBUG
      std::cout << "symbol number " << k << " (flag) is \"" << line << "\"" << std::endl;
      kt->operator[](k) = strdup(line);
#endif
      
      return;
    }
  operations.push_back(FlagDiacriticOperation()); // dummy flag

#if OL_FULL_DEBUG
  std::cout << "symbol number " << k << " is \"" << line << "\"" << std::endl;
#endif
  
  kt->operator[](k) = _strdup(line);
}

void LetterTrie::add_string(const char * p, SymbolNumber symbol_key)
{
  if (*(p+1) == 0)
    {
      symbols[(unsigned char)(*p)] = symbol_key;
      return;
    }
  if (letters[(unsigned char)(*p)] == NULL)
    {
      letters[(unsigned char)(*p)] = new LetterTrie();
    }
  letters[(unsigned char)(*p)]->add_string(p+1,symbol_key);
}

SymbolNumber LetterTrie::find_key(char ** p)
{
  const char * old_p = *p;
  ++(*p);
  if (letters[(unsigned char)(*old_p)] == NULL)
    {
      return symbols[(unsigned char)(*old_p)];
    }
  SymbolNumber s = letters[(unsigned char)(*old_p)]->find_key(p);
  if (s == NO_SYMBOL_NUMBER)
    {
      --(*p);
      return symbols[(unsigned char)(*old_p)];
    }
  return s;
}

void Encoder::read_input_symbols(KeyTable * kt)
{
  for (SymbolNumber k = 0; k < number_of_input_symbols; ++k)
    {
#if DEBUG
      assert(kt->find(k) != kt->end());
#endif
      const char * p = kt->operator[](k);
      if ((strlen(p) == 1) && (unsigned char)(*p) <= 127)
	{
	  ascii_symbols[(unsigned char)(*p)] = k;
	}
      letters.add_string(p,k);
    }
}

SymbolNumber Encoder::find_key(char ** p)
{
  if (ascii_symbols[(unsigned char)(**p)] == NO_SYMBOL_NUMBER)
    {
      return letters.find_key(p);
    }
  SymbolNumber s = ascii_symbols[(unsigned char)(**p)];
  ++(*p);
  return s;
}

#ifdef DICTIONARY_CREATION

bool do_run_word_simplifier(char* word, int lenght, string &output_word)
{//lenght is size+1
	//first_result_else_shorter_param = first_result_else_shorter;

	if(lenght > 300 || lenght == 1) return false;//some kind of recursion error happens when the string is above 1005 characters, so avoid processing huge words
	
	//if(find_pair(word, lenght, output_word)) return true;//success
	
	strcpy(str, word);

    if (echoInputsFlag)
	{
	  //std::cout << str << std::endl;
	}
      int i = 0;
      SymbolNumber k = NO_SYMBOL_NUMBER;
      bool failed = false;
      for ( char ** Str = &str; **Str != 0; )
	{
	  k = TransducerW_C.find_next_key(Str);
#if OL_FULL_DEBUG
	  std::cout << "INPUT STRING ENTRY " << i << " IS " << k << std::endl;
#endif
	  if (k == NO_SYMBOL_NUMBER)
	    {
	      if (echoInputsFlag)
		{
		  //std::cout << std::endl;
		}
	      failed = true;
	      break;
	    }
	  input_string[i] = k;
	  ++i;
	}
      str = old_str;
      if (failed)
      	{ // tokenization failed
	  if (outputType == xerox)
	    {
	      //std::cout << str << "\t+?" << std::endl;
	      //std::cout << std::endl;
	    }
      	  simplification_result = false;
      	}
      input_string[i] = NO_SYMBOL_NUMBER;

      TransducerW_C.analyze(input_string);
      TransducerW_C.printAnalyses(std::string(str));

	  output_word = correct_word;
	  
	  //add_word_to_catch(word, lenght, output_word);//now, add the word to the catch

	  bool final_result = simplification_result;

	  simplification_result = false;
	  correct_word = "";

	  return final_result;
}

void runTransducer()
{
  input_string = (SymbolNumber*)(malloc(2000));
  for (int i = 0; i < 1000; ++i)
    {
      input_string[i] = NO_SYMBOL_NUMBER;
    }
  
  str = (char*)(malloc(MAX_IO_STRING*sizeof(char)));  
  *str = 0;
  old_str = str;
}


















void load_dictionary(vector<string> &word_in_duictionary)
{
	FILE * f = fopen((base_work_directory_hfst+data_file_dictionary_directory_hfst+"\\hunspell-en_US-2015.05.18\\en_US.dic").c_str(), "rb");
	fseek(f, 0L, SEEK_END);
	unsigned long long sz = ftell(f);
	fseek(f, 0L, SEEK_SET);

	char* buffer = new char[sz+1];
	fread(buffer, 1, sz, f);
	buffer[sz] = 0;//top be sure there is a '/0' at the end
	fclose(f);

	int word_size = 0, word_start_pos = 0;

	for(int i = 0; i < sz; i++)
	{
		if(buffer[i] == '\n')
		{
			if(word_size != 0)
			{
				string word_now = string(&buffer[word_start_pos], word_size);

				remove_everything_after_special_char_and_lower(word_now, '/');

				if(word_now.size() > 0)//great, now save it
				{
					//cout << word_now << "\n";
					word_in_duictionary.push_back(word_now);
				}

				word_size = 0;
				word_start_pos = 0;
			}
		}
		else
		{
			if(word_size == 0) word_start_pos = i;//we start a new word
			word_size++;
		}
	}

	free(buffer);
}

void get_full_word_list()
{
	cout << "0. CREATING ENGLISH SYNONYM DICTIONARY\n";

	vector<string> word_in_duictionary;

	load_dictionary(word_in_duictionary);
	
	FILE * f = fopen((base_work_directory_hfst+data_file_dictionary_directory_hfst+"\\hfst-english-bnc\\src\\raw_counts.txt").c_str(), "rb");
	fseek(f, 0L, SEEK_END);
	unsigned long long sz = ftell(f);
	fseek(f, 0L, SEEK_SET);

	char* buffer = new char[sz+1];
	fread(buffer, 1, sz, f);
	buffer[sz] = 0;//top be sure there is a '/0' at the end
	fclose(f);

	for (int i = 0; i < sz; i++)
	{
		if(buffer[i] >= 'A' && buffer[i] <= 'Z') buffer[i] += ('a'-'A');//make letter lower case
		if(buffer[i] < 'a' || buffer[i] > 'z') buffer[i] = ' ';//not a letter, replace it with ' '
	}

	//cout << string(buffer, sz) << "\n";

	int total_array_size_output = 0;
	int current_array_size = 10000;
	int current_pos = 0;
	int *word_hashes_a = (int*)malloc(sizeof(int)*current_array_size);
	int *word_positions = (int*)malloc(sizeof(int)*current_array_size);

	char **word_relationships_a = (char**)malloc(sizeof(char*)*current_array_size);
	char **word_relationships_b = (char**)malloc(sizeof(char*)*current_array_size);

	int word_size = 0, word_start_pos = 0;;
	for(int i = 0; i < sz; i++)
	{
		//if(i % 10000 == 0) cout << i << "/" << sz << "\n";

		if(buffer[i] == ' ')
		{
			if(word_size != 0)
			{
				
				string
					word_now = string(&buffer[word_start_pos], word_size),
					final_result = word_now,
					buffer_word;

				while(do_run_word_simplifier((char*)final_result.c_str(), final_result.size()+1, buffer_word))
				{
					remove_everything_after_special_char_and_lower(buffer_word);//kill the special characters of the result (if any)

					if(buffer_word == final_result)
						break;

					final_result = buffer_word;
				}
				
				if(word_now != final_result)//store the relationship
				{
					//TODO CHECK IF THE final_result EXISTS IN THE DICTIONARY
					//1st check if the final_result exists in the dictionary and if yes, store the relationship

					int item_hash = hash_djb2(word_now.c_str());

					bool item_found = false;

					for(int i = 0; i < current_pos; i++)
						if(word_hashes_a[i] == item_hash && strcmp(word_now.c_str(), word_relationships_a[i]) == 0)
						{
							item_found = true;
							break;
						}

						if(!item_found)
						{
							bool in_dictionary = false;

							if(!kill_word_outside_of_dictionary)
								in_dictionary = true;
							else
							{
								for(int i = 0; i < word_in_duictionary.size(); i++)
									if(word_in_duictionary[i] == final_result)
									{
										in_dictionary = true;
										break;
									}
							}

							if(in_dictionary)
							{
								word_relationships_a[current_pos] = (char*)malloc(word_now.size()+1);
								strcpy(word_relationships_a[current_pos], word_now.c_str());

								word_relationships_b[current_pos] = (char*)malloc(final_result.size()+1);
								strcpy(word_relationships_b[current_pos], final_result.c_str());

								total_array_size_output += word_now.size()+1+final_result.size()+1;

								word_positions[current_pos] = current_pos;
								word_hashes_a[current_pos] = hash_djb2(word_now.c_str());

								current_pos++;

								if(current_pos == current_array_size)
								{
									current_array_size *= 2;
									word_hashes_a = (int*)realloc(word_hashes_a, sizeof(int)*current_array_size);
									word_positions = (int*)realloc(word_positions, sizeof(int)*current_array_size);

									word_relationships_a = (char**)realloc(word_relationships_a, sizeof(char*)*current_array_size);
									word_relationships_b = (char**)realloc(word_relationships_b, sizeof(char*)*current_array_size);
								}
							}
						}
				}

				word_size = 0;
				word_start_pos = 0;
			}
		}
		else
		{
			if(word_size == 0) word_start_pos = i;//we start a new word
			word_size++;
		}
	}

	//done, sort the data
	sort_arrays_2.sort(word_hashes_a, word_positions, current_pos);

	//great, now save the data
	int j = 0;
	char* ready_words_list = (char*)malloc(total_array_size_output);
	for(int i = 0; i < current_pos; i++)
	{
		int k = 0;
		while(word_relationships_a[word_positions[i]][k] != 0) ready_words_list[j++] = word_relationships_a[word_positions[i]][k++];
		ready_words_list[j++] = 0;

		k = 0;
		while(word_relationships_b[word_positions[i]][k] != 0) ready_words_list[j++] = word_relationships_b[word_positions[i]][k++];
		ready_words_list[j++] = 0;
	}

	if(j != total_array_size_output) cout << "SIZE_ERROR!";
	FILE * fd = fopen((base_work_directory_hfst+data_file_dictionary_directory_hfst+"\\output_simple_word_list").c_str(), "wb");
	fwrite(ready_words_list, 1, total_array_size_output, fd);
	fclose(fd);

	//save the word size file
	fd = fopen((base_work_directory_hfst+data_file_dictionary_directory_hfst+"\\output_simple_word_list_pairs_no").c_str(), "wb");
	fwrite(&current_pos, 1, sizeof(int), fd);
	fclose(fd);

	free(ready_words_list);

	cout << "0. DICTIONARY READY.\n\n";
}



















void word_simplifier_setup(bool get_first_result_else_shorter_in, bool kill_word_outside_of_dictionary_in)
{
	get_first_result_else_shorter = get_first_result_else_shorter_in;
	kill_word_outside_of_dictionary = kill_word_outside_of_dictionary_in;

	fclose(f);

  if (header.probe_flag(Has_unweighted_input_epsilon_cycles) ||
      header.probe_flag(Has_input_epsilon_cycles))
    {
      std::cerr << "!! Warning: transducer has epsilon cycles                  !!\n"
		<< "!! This is currently not handled - if they are encountered !!\n"
		<< "!! program *will* segfault.                                !!\n";
    }

	runTransducer();

	get_full_word_list();
}

#else

int now_word_analyzer_get_word_pos_from_hash_pos(int hash_pos, const char* Word)
{
	int 
		start_pos = hash_pos,
		end_pos = hash_pos;

	while(start_pos >= 0 && word_pair_a_hashes[start_pos] == word_pair_a_hashes[hash_pos]) start_pos--;
	start_pos++;

	while(end_pos < words_pairs_available && word_pair_a_hashes[end_pos] == word_pair_a_hashes[hash_pos]) end_pos++;
	end_pos--;

	for(int i = start_pos; i <= end_pos; i++)
		if(strcmp(word_pair_a[i], Word) == 0/* && catched_words_param[sort_positions[i]] == first_result_else_shorter_param*/)
			return i;

	return -1;
}

int now_word_analyzer_binary_search_added_strings_pos(int Hash, const char* Word)
{
	int low = 0, high = words_pairs_available-1, mid;

	while(low <= high)
	{
		mid = (low + high)/2;

		if(word_pair_a_hashes[mid] == Hash)
		{
			if(strcmp(word_pair_a[mid], Word) == 0)
				return mid;
			else
				return now_word_analyzer_get_word_pos_from_hash_pos(mid, Word);
		}
		else if(word_pair_a_hashes[mid] > Hash)
			low = mid+1; 
		else
			high = mid-1;
	}

	return -1;
}

bool do_run_word_simplifier(char* word, int lenght, string &output_word)
{
	int three_byte_item_hash = hash_djb2(word);
	((char*)(&three_byte_item_hash))[3] = 0;//make it a 3 byte int

	if(hash_existance_check[three_byte_item_hash] == 1)//if we have an item with that hash
	{
		int pos_in_array = now_word_analyzer_binary_search_added_strings_pos(hash_djb2(word), word);

		if(pos_in_array == -1)
			return false;//word doesn't exist
		else
		{
			output_word = string(word_pair_b[pos_in_array], strlen(word_pair_b[pos_in_array]));
			return true;
		}
	}
	else return false;//word doesn't exist
}

void word_simplifier_setup(bool get_first_result_else_shorter_in, bool kill_word_outside_of_dictionary_in)
{
	//actuall run mode, define this useless function, since we don't setup anything

	string word_pair_file = base_work_directory_hfst+data_file_dictionary_directory_hfst+"\\output_simple_word_list";

	if(word_pair_file == ""){cout << "word_simplifier_setup NO INPUT FILE!\n"; cin.get(); cin.get();}

	FILE * f = fopen(word_pair_file.c_str(), "rb");
	fseek(f, 0L, SEEK_END);
	unsigned long long sz = ftell(f);
	fseek(f, 0L, SEEK_SET);

	char* buffer = new char[sz+1];
	fread(buffer, 1, sz, f);
	buffer[sz] = 0;//top be sure there is a '/0' at the end
	fclose(f);

	//great we got all the word data

	//unsigned int pairs_no = 0;
	f = fopen((word_pair_file+"_pairs_no").c_str(), "rb");
	fread(&words_pairs_available, 1, sizeof(int), f);
	fclose(f);

	//we got how many pairs there are too

	word_pair_a = (char**)malloc(sizeof(char*)*words_pairs_available);
	word_pair_b = (char**)malloc(sizeof(char*)*words_pairs_available);

	word_pair_a_hashes = (int*)malloc(sizeof(int)*words_pairs_available);

	hash_existance_check = (char*)calloc(16777215, 1);//max 3 byte int size, 16.7MB. USE CALLOC IS EXTREMELY IMPORTANT HERE

	int pos_x = 0, i_a = 0, i_b = 0;

	for(int i = 0; i_b < words_pairs_available; i++)
	{
		int len = strlen(&buffer[pos_x]);

		if(i % 2 == 0)
		{
			word_pair_a[i_a] = (char*)malloc(sizeof(char)*(len+1));
			strcpy(word_pair_a[i_a], &buffer[pos_x]);
			i_a++;
		}
		else
		{
			word_pair_b[i_b] = (char*)malloc(sizeof(char)*(len+1));
			strcpy(word_pair_b[i_b], &buffer[pos_x]);
			i_b++;
		}

		pos_x += len+1;
	}

	for(int i = 0; i < words_pairs_available; i++)
	{
		word_pair_a_hashes[i] = hash_djb2(word_pair_a[i]);

		unsigned int three_byte_hash = word_pair_a_hashes[i];//copy the 4 byte hash
		((char*)(&three_byte_hash))[3] = 0;//make it a 3 byte hash by setting the last byte to 0
		hash_existance_check[three_byte_hash] = 1;//item exists
	}
}

#endif

/**
 * BEGIN old transducer.cc
 */

bool TransducerFd::PushState(FlagDiacriticOperation op)
{ // try to alter the flag diacritic state stack
  switch (op.Operation()) {
  case P: // positive set
    statestack.push_back(statestack.back());
    statestack.back()[op.Feature()] = op.Value();
    return true;
  case N: // negative set (literally, in this implementation)
    statestack.push_back(statestack.back());
    statestack.back()[op.Feature()] = -1*op.Value();
    return true;
  case R: // require
    if (op.Value() == 0) // empty require
      {
	if (statestack.back()[op.Feature()] == 0)
	  {
	    return false;
	  }
	else
	  {
	    statestack.push_back(statestack.back());
	    return true;
	  }
      }
    if (statestack.back()[op.Feature()] == op.Value())
      {
	statestack.push_back(statestack.back());
	return true;
      }
    return false;
  case D: // disallow
        if (op.Value() == 0) // empty disallow
      {
	if (statestack.back()[op.Feature()] != 0)
	  {
	    return false;
	  }
	else
	  {
	    statestack.push_back(statestack.back());
	    return true;
	  }
      }
    if (statestack.back()[op.Feature()] == op.Value()) // nonempty disallow
      {
	return false;
      }
    statestack.push_back(statestack.back());
    return true;
  case C: // clear
    statestack.push_back(statestack.back());
    statestack.back()[op.Feature()] = 0;
    return true;
  case U: // unification
    if (statestack.back()[op.Feature()] == 0 || // if the feature is unset or
	statestack.back()[op.Feature()] == op.Value() || // the feature is at this value already or
	(statestack.back()[op.Feature()] < 0 &&
	 (statestack.back()[op.Feature()] * -1 != op.Value())) // the feature is negatively set to something else
	)
      {
	statestack.push_back(statestack.back());
	statestack.back()[op.Feature()] = op.Value();
	return true;
      }
    return false;
  }
  throw; // for the compiler's peace of mind
}

bool TransitionIndex::matches(SymbolNumber s)
{
  
  if (input_symbol == NO_SYMBOL_NUMBER)
    {
      return false;
    }
  if (s == NO_SYMBOL_NUMBER)
    {
      return true;
    }
  return input_symbol == s;
}

bool Transition::matches(SymbolNumber s)
{
  
  if (input_symbol == NO_SYMBOL_NUMBER)
    {
      return false;
    }
  if (s == NO_SYMBOL_NUMBER)
    {
      return true;
    }
  return input_symbol == s;
}


void IndexTableReader::get_index_vector(void)
{
  for (size_t i = 0;
       i < number_of_table_entries;
       ++i)
    {
      size_t j = i * TransitionIndex::SIZE;
      SymbolNumber * input = (SymbolNumber*)(TableIndices + j);
      TransitionTableIndex * index = 
	(TransitionTableIndex*)(TableIndices + j + sizeof(SymbolNumber));
      indices.push_back(new TransitionIndex(*input,*index));
    }
}

void TransitionTableReader::Set(TransitionTableIndex pos)
{
  if (pos >= TRANSITION_TARGET_TABLE_START)
    {
      position = pos - TRANSITION_TARGET_TABLE_START;
    }
  else
    {
      position = pos;
    }
}

void TransitionTableReader::get_transition_vector(void)
{
  for (size_t i = 0; i < number_of_table_entries; ++i)
    {
      size_t j = i * Transition::SIZE;
      SymbolNumber * input = (SymbolNumber*)(TableTransitions + j);
      SymbolNumber * output = 
	(SymbolNumber*)(TableTransitions + j + sizeof(SymbolNumber));
      TransitionTableIndex * target = 
       (TransitionTableIndex*)(TableTransitions + j + 2 * sizeof(SymbolNumber));
      transitions.push_back(new Transition(*input,
					   *output,
					   *target));
      
    }
}

bool TransitionTableReader::Matches(SymbolNumber s)
{
  Transition * t = transitions[position];
  return t->matches(s);
}

bool TransitionTableReader::get_finality(TransitionTableIndex i)
{
  if (i >= TRANSITION_TARGET_TABLE_START) 
    {
      return transitions[i - TRANSITION_TARGET_TABLE_START]->final();
    }
  else
    {
      return transitions[i]->final();
    }
}

void Transducer::set_symbol_table(void)
{
  for(KeyTable::iterator it = keys->begin();
      it != keys->end();
      ++it)
    {
      const char * key_name =
	it->second;

      symbol_table.push_back(key_name);
    }
}

void Transducer::try_epsilon_transitions(SymbolNumber * input_symbol,
					 SymbolNumber * output_symbol,
					 SymbolNumber * original_output_string,
					 TransitionTableIndex i)
{
#if OL_FULL_DEBUG
  std::cout << "try_epsilon_transitions " << i << std::endl;
#endif
  while (transitions[i]->get_input() == 0)
    {
      *output_symbol = transitions[i]->get_output();
      get_analyses(input_symbol,
		   output_symbol+1,
		   original_output_string,
		   transitions[i]->target());
      ++i;
    }
}

void TransducerFd::try_epsilon_transitions(SymbolNumber * input_symbol,
					 SymbolNumber * output_symbol,
					 SymbolNumber * original_output_string,
					 TransitionTableIndex i)
{
#if OL_FULL_DEBUG
  std::cout << "try_epsilon_transitions " << i << std::endl;
#endif
  
  while (true)
    {
    if (transitions[i]->get_input() == 0) // epsilon
	{
	  *output_symbol = transitions[i]->get_output();
	  get_analyses(input_symbol,
		       output_symbol+1,
		       original_output_string,
		       transitions[i]->target());
	  ++i;
	} else if (transitions[i]->get_input() != NO_SYMBOL_NUMBER &&
		   operations[transitions[i]->get_input()].isFlag())
	{
	  if (PushState(operations[transitions[i]->get_input()]))
	    {
#if OL_FULL_DEBUG
	      std::cout << "flag diacritic " <<
		symbol_table[transitions[i]->get_input()] << " allowed\n";
#endif
	      // flag diacritic allowed
	      *output_symbol = transitions[i]->get_output();
	      get_analyses(input_symbol,
			   output_symbol+1,
			   original_output_string,
			   transitions[i]->target());
	      statestack.pop_back();
	    }
	  else
	    {
#if OL_FULL_DEBUG
	      std::cout << "flag diacritic " <<
		symbol_table[transitions[i]->get_input()] << " disallowed\n";
#endif
	    }
	  ++i;
	} else
	{
	  return;
	}
    }
}

void Transducer::try_epsilon_indices(SymbolNumber * input_symbol,
				     SymbolNumber * output_symbol,
				     SymbolNumber * original_output_string,
				     TransitionTableIndex i)
{
#if OL_FULL_DEBUG
  std::cout << "try_epsilon_indices " << i << std::endl;
#endif
  if (indices[i]->get_input() == 0)
    {
      try_epsilon_transitions(input_symbol,
			      output_symbol,
			      original_output_string,
			      indices[i]->target() - 
			      TRANSITION_TARGET_TABLE_START);
    }
}

void Transducer::find_transitions(SymbolNumber input,
				    SymbolNumber * input_symbol,
				    SymbolNumber * output_symbol,
				    SymbolNumber * original_output_string,
				    TransitionTableIndex i)
{
#if OL_FULL_DEBUG
  std::cout << "find_transitions " << i << "\t" << transitions[i]->get_input() << std::endl;
#endif

  while (transitions[i]->get_input() != NO_SYMBOL_NUMBER)
    {
      if (transitions[i]->get_input() == input)
	{
	  
	  *output_symbol = transitions[i]->get_output();
	  get_analyses(input_symbol,
		       output_symbol+1,
		       original_output_string,
		       transitions[i]->target());
	}
      else
	{
	  return;
	}
      ++i;
    }
}

void Transducer::find_index(SymbolNumber input,
			    SymbolNumber * input_symbol,
			    SymbolNumber * output_symbol,
			    SymbolNumber * original_output_string,
			    TransitionTableIndex i)
{
#if OL_FULL_DEBUG
  std::cout << "find_index " << i << "\t" << indices[i+input]->get_input() << std::endl;
#endif
  if (indices[i+input]->get_input() == input)
    {
      find_transitions(input,
		       input_symbol,
		       output_symbol,
		       original_output_string,
		       indices[i+input]->target() - 
		       TRANSITION_TARGET_TABLE_START);
    }
}

void Transducer::note_analysis(SymbolNumber * whole_output_string)
{
  if (beFast)
    {
      for (SymbolNumber * num = whole_output_string; *num != NO_SYMBOL_NUMBER; ++num)
	{
	  std::cout << symbol_table[*num];
	}
      std::cout << std::endl;
    } else
    {
      std::string str = "";
      for (SymbolNumber * num = whole_output_string; *num != NO_SYMBOL_NUMBER; ++num)
	{
	  str.append(symbol_table[*num]);
	}
      display_vector.push_back(str);
    }
}

void TransducerUniq::note_analysis(SymbolNumber * whole_output_string)
{
  std::string str = "";
  for (SymbolNumber * num = whole_output_string; *num != NO_SYMBOL_NUMBER; ++num)
    {
      str.append(symbol_table[*num]);
    }
  display_vector.insert(str);
}

void TransducerFdUniq::note_analysis(SymbolNumber * whole_output_string)
{
  std::string str = "";
  for (SymbolNumber * num = whole_output_string; *num != NO_SYMBOL_NUMBER; ++num)
    {
      str.append(symbol_table[*num]);
    }
  display_vector.insert(str);
}

void Transducer::get_analyses(SymbolNumber * input_symbol,
			      SymbolNumber * output_symbol,
			      SymbolNumber * original_output_string,
			      TransitionTableIndex i)
{
#if OL_FULL_DEBUG
  std::cout << "get_analyses " << i << std::endl;
#endif
  if (i >= TRANSITION_TARGET_TABLE_START )
    {
      i -= TRANSITION_TARGET_TABLE_START;
      
      try_epsilon_transitions(input_symbol,
			      output_symbol,
			      original_output_string,
			      i+1);

#if OL_FULL_DEBUG
      std::cout << "Testing input string on transition side, " << *input_symbol << " at pointer" << std::endl;
#endif

      // input-string ended.
      if (*input_symbol == NO_SYMBOL_NUMBER)
	{
	  *output_symbol = NO_SYMBOL_NUMBER;
	  if (final_transition(i))
	    {
	      note_analysis(original_output_string);
	    }
	  return;
	}
      
      SymbolNumber input = *input_symbol;
      ++input_symbol;
      
      find_transitions(input,
		       input_symbol,
		       output_symbol,
		       original_output_string,
		       i+1);
    }
  else
    {
      
      try_epsilon_indices(input_symbol,
			  output_symbol,
			  original_output_string,
			  i+1);
      
#if OL_FULL_DEBUG
      std::cout << "Testing input string on index side, " << *input_symbol << " at pointer" << std::endl;
#endif
      
      if (*input_symbol == NO_SYMBOL_NUMBER)
	{ // input-string ended.
	  *output_symbol = NO_SYMBOL_NUMBER;
	  if (final_index(i))
	    {
	      note_analysis(original_output_string);
	    }
	  return;
	}
      
      SymbolNumber input = *input_symbol;
      ++input_symbol;

      find_index(input,
		 input_symbol,
		 output_symbol,
		 original_output_string,
		 i+1);
    }
  *output_symbol = NO_SYMBOL_NUMBER;
}

void Transducer::printAnalyses(std::string prepend)
{
  if (!beFast)
    {
      if (outputType == xerox && display_vector.size() == 0)
	{
	  std::cout << prepend << "\t+?" << std::endl;
	  std::cout << std::endl;
	  return;
	}
      int i = 0;
      DisplayVector::iterator it = display_vector.begin();
      while ( (it != display_vector.end()) && i < maxAnalyses )
	{
	  if (outputType == xerox)
	    {
	      std::cout << prepend << "\t";
	    }
	  std::cout << *it << std::endl;
	  ++it;
	  ++i;
	}
      display_vector.clear(); // purge the display vector
      std::cout << std::endl;
    }
}

void TransducerUniq::printAnalyses(std::string prepend)
{
  if (outputType == xerox && display_vector.size() == 0)
    {
      std::cout << prepend << "\t+?" << std::endl;
      std::cout << std::endl;
      return;
    }
  int i = 0;
  DisplaySet::iterator it = display_vector.begin();
  while ( (it != display_vector.end()) && i < maxAnalyses)
    {
      if (outputType == xerox)
	{
	  std::cout << prepend << "\t";
	}
      std::cout << *it << std::endl;
      ++it;
      ++i;
    }
  display_vector.clear(); // purge the display set
  std::cout << std::endl;
}

void TransducerFdUniq::printAnalyses(std::string prepend)
{
  if (outputType == xerox && display_vector.size() == 0)
    {
      std::cout << prepend << "\t+?" << std::endl;
      std::cout << std::endl;
      return;
    }
  int i = 0;
  DisplaySet::iterator it = display_vector.begin();
  while ( (it != display_vector.end()) && i < maxAnalyses)
    {
      if (outputType == xerox)
	{
	  std::cout << prepend << "\t";
	}
      std::cout << *it << std::endl;
      ++it;
      ++i;
    }
  display_vector.clear(); // purge the display set
  std::cout << std::endl;
}

/**
 * BEGIN old transducer-weighted.cc
 */

bool TransitionWIndex::matches(SymbolNumber s)
{
  
  if (input_symbol == NO_SYMBOL_NUMBER)
    {
      return false;
    }
  if (s == NO_SYMBOL_NUMBER)
    {
      return true;
    }
  return input_symbol == s;
}

bool TransitionW::matches(SymbolNumber s)
{
  
  if (input_symbol == NO_SYMBOL_NUMBER)
    {
      return false;
    }
  if (s == NO_SYMBOL_NUMBER)
    {
      return true;
    }
  return input_symbol == s;
}

bool TransducerWFd::PushState(FlagDiacriticOperation op)
{
  switch (op.Operation()) {
  case P: // positive set
    statestack.push_back(statestack.back());
    statestack.back()[op.Feature()] = op.Value();
    return true;
  case N: // negative set (literally, in this implementation)
    statestack.push_back(statestack.back());
    statestack.back()[op.Feature()] = -1*op.Value();
    return true;
  case R: // require
    if (op.Value() == 0) // empty require
      {
	if (statestack.back()[op.Feature()] == 0)
	  {
	    return false;
	  }
	statestack.push_back(statestack.back());
	return true;
      }
    if (statestack.back()[op.Feature()] == op.Value())
      {
	statestack.push_back(statestack.back());
	return true;
      }
    return false;
  case D: // disallow
    if (op.Value() == 0) // empty disallow
      {
	if (statestack.back()[op.Feature()] != 0)
	  {
	    return false;
	  }
	else
	  {
	    statestack.push_back(statestack.back());
	    return true;
	  }
      }
    if (statestack.back()[op.Feature()] == op.Value()) // nonempty disallow
      {
	return false;
      }
    statestack.push_back(statestack.back());
    return true;
  case C: // clear
    statestack.push_back(statestack.back());
    statestack.back()[op.Feature()] = 0;
    return true;
  case U: // unification
    if (statestack.back()[op.Feature()] == 0 || // if the feature is unset or
	statestack.back()[op.Feature()] == op.Value() || // the feature is at this value already or
	(statestack.back()[op.Feature()] < 0 &&
	 (statestack.back()[op.Feature()] * -1 != op.Value())) // the feature is negatively set to something else
	)
      {
	statestack.push_back(statestack.back());
	statestack.back()[op.Feature()] = op.Value();
	return true;
      }
    return false;
  }
  throw; // for the compiler's peace of mind
}

void IndexTableReaderW::get_index_vector(void)
{
  for (size_t i = 0;
       i < number_of_table_entries;
       ++i)
    {
      size_t j = i * TransitionWIndex::SIZE;
      SymbolNumber * input = (SymbolNumber*)(TableIndices + j);
      TransitionTableIndex * index = 
	(TransitionTableIndex*)(TableIndices + j + sizeof(SymbolNumber));
      indices.push_back(new TransitionWIndex(*input,*index));
    }
}

void TransitionTableReaderW::Set(TransitionTableIndex pos)
{
  if (pos >= TRANSITION_TARGET_TABLE_START)
    {
      position = pos - TRANSITION_TARGET_TABLE_START;
    }
  else
    {
      position = pos;
    }
}

void TransitionTableReaderW::get_transition_vector(void)
{
  for (size_t i = 0; i < number_of_table_entries; ++i)
    {
      size_t j = i * TransitionW::SIZE;
      SymbolNumber * input = (SymbolNumber*)(TableTransitions + j);
      SymbolNumber * output = 
	(SymbolNumber*)(TableTransitions + j + sizeof(SymbolNumber));
      TransitionTableIndex * target = 
	(TransitionTableIndex*)(TableTransitions + j + 2 * sizeof(SymbolNumber));
      Weight * weight =
	(Weight*)(TableTransitions + j + 2 * sizeof(SymbolNumber) + sizeof(TransitionTableIndex));
      transitions.push_back(new TransitionW(*input,
					    *output,
					    *target,
					    *weight));
      
    }
  transitions.push_back(new TransitionW());
  transitions.push_back(new TransitionW());
}

bool TransitionTableReaderW::Matches(SymbolNumber s)
{
  TransitionW * t = transitions[position];
  return t->matches(s);
}

bool TransitionTableReaderW::get_finality(TransitionTableIndex i)
{
  if (i >= TRANSITION_TARGET_TABLE_START) 
    {
      return transitions[i - TRANSITION_TARGET_TABLE_START]->final();
    }
  else
    {
      return transitions[i]->final();
    }
}


void TransducerW::set_symbol_table(void)
{
  for(KeyTable::iterator it = keys->begin();
      it != keys->end();
      ++it)
    {
      const char * key_name =
	it->second;

      symbol_table.push_back(key_name);
    }
}

void TransducerW::try_epsilon_transitions(SymbolNumber * input_symbol,
					  SymbolNumber * output_symbol,
					  SymbolNumber * 
					  original_output_string,
					  TransitionTableIndex i)
{
#if OL_FULL_DEBUG
  std::cerr << "try epsilon transitions " << i << " " << current_weight << std::endl;
#endif

  if (transitions.size() <= i) 
    {
      return;
    }

  while ((transitions[i] != NULL) && (transitions[i]->get_input() == 0))
    {
      *output_symbol = transitions[i]->get_output();
      current_weight += transitions[i]->get_weight();
      get_analyses(input_symbol,
		   output_symbol+1,
		   original_output_string,
		   transitions[i]->target());
      current_weight -= transitions[i]->get_weight();
      ++i;
    }
  *output_symbol = NO_SYMBOL_NUMBER;
}

void TransducerWFd::try_epsilon_transitions(SymbolNumber * input_symbol,
					    SymbolNumber * output_symbol,
					    SymbolNumber * 
					    original_output_string,
					    TransitionTableIndex i)
{
  if (transitions.size() <= i)
    { return; }
  
  while (true)
    {
    if (transitions[i]->get_input() == 0) // epsilon
	{
	  *output_symbol = transitions[i]->get_output();
	  current_weight += transitions[i]->get_weight();
	  get_analyses(input_symbol,
		       output_symbol+1,
		       original_output_string,
		       transitions[i]->target());
	  current_weight -= transitions[i]->get_weight();
	  ++i;
	} else if (transitions[i]->get_input() != NO_SYMBOL_NUMBER &&
		   operations[transitions[i]->get_input()].isFlag())
	{
	    if (PushState(operations[transitions[i]->get_input()]))
	    {
#if OL_FULL_DEBUG
	      std::cout << "flag diacritic " <<
		symbol_table[transitions[i]->get_input()] << " allowed\n";
#endif
	      // flag diacritic allowed
	      *output_symbol = transitions[i]->get_output();
	      current_weight += transitions[i]->get_weight();
	      get_analyses(input_symbol,
			   output_symbol+1,
			   original_output_string,
			   transitions[i]->target());
	      current_weight -= transitions[i]->get_weight();
	      statestack.pop_back();
	    }
	  else
	    {
#if OL_FULL_DEBUG
	      std::cout << "flag diacritic " <<
		symbol_table[transitions[i]->get_input()] << " disallowed\n";
#endif
	    }
	  ++i;
	} else
	{
	  return;
	}
    }
}

void TransducerW::try_epsilon_indices(SymbolNumber * input_symbol,
				      SymbolNumber * output_symbol,
				      SymbolNumber * original_output_string,
				      TransitionTableIndex i)
{
#if OL_FULL_DEBUG
  std::cerr << "try indices " << i << " " << current_weight << std::endl;
#endif
  if (indices[i]->get_input() == 0)
    {
      try_epsilon_transitions(input_symbol,
			      output_symbol,
			      original_output_string,
			      indices[i]->target() - 
			      TRANSITION_TARGET_TABLE_START);
    }
}

void TransducerW::find_transitions(SymbolNumber input,
				   SymbolNumber * input_symbol,
				   SymbolNumber * output_symbol,
				   SymbolNumber * original_output_string,
				   TransitionTableIndex i)
{
#if OL_FULL_DEBUG
  std::cerr << "find transitions " << i << " " << current_weight << std::endl;
#endif

  if (transitions.size() <= i) 
    {
      return;
    }
  while (transitions[i]->get_input() != NO_SYMBOL_NUMBER)
    {
      
      if (transitions[i]->get_input() == input)
	{
	  current_weight += transitions[i]->get_weight();
	  *output_symbol = transitions[i]->get_output();
	  get_analyses(input_symbol,
		       output_symbol+1,
		       original_output_string,
		       transitions[i]->target());
	  current_weight -= transitions[i]->get_weight();
	}
      else
	{
	  return;
	}
      ++i;
    }
  
}

void TransducerW::find_index(SymbolNumber input,
			     SymbolNumber * input_symbol,
			     SymbolNumber * output_symbol,
			     SymbolNumber * original_output_string,
			     TransitionTableIndex i)
{
#if OL_FULL_DEBUG
  std::cerr << "find index " << i << " " << current_weight << std::endl;
#endif
  if (indices.size() <= i) 
    {
      return;
    }
  
  if (indices[i+input]->get_input() == input)
    {
      
      find_transitions(input,
		       input_symbol,
		       output_symbol,
		       original_output_string,
		       indices[i+input]->target() - 
		       TRANSITION_TARGET_TABLE_START);
    }
}

void TransducerW::note_analysis(SymbolNumber * whole_output_string)
{
  std::string str = "";
  for (SymbolNumber * num = whole_output_string;
       *num != NO_SYMBOL_NUMBER;
       ++num)
    {
      str.append(symbol_table[*num]);
    }
  display_map.insert(std::pair<Weight, std::string>(current_weight, str));
}

void TransducerWUniq::note_analysis(SymbolNumber * whole_output_string)
{
  std::string str = "";
  for (SymbolNumber * num = whole_output_string;
       *num != NO_SYMBOL_NUMBER;
       ++num)
    {
      str.append(symbol_table[*num]);
    }
  if ((display_map.count(str) == 0) || (display_map[str] > current_weight))
    { // if there isn't an entry yet or we've found a lower weight
      display_map.insert(std::pair<std::string, Weight>(str, current_weight));
    }
}

void TransducerWFdUniq::note_analysis(SymbolNumber * whole_output_string)
{
  std::string str = "";
  for (SymbolNumber * num = whole_output_string;
       *num != NO_SYMBOL_NUMBER;
       ++num)
    {
      str.append(symbol_table[*num]);
    }
  if ((display_map.count(str) == 0) || (display_map[str] > current_weight))
    { // if there isn't an entry yet or we've found a lower weight
      display_map.insert(std::pair<std::string, Weight>(str, current_weight));
    }
}

#ifdef DICTIONARY_CREATION
void TransducerW::printAnalyses(std::string prepend)
{
  if (outputType == xerox && display_map.size() == 0)
    {
  //    std::cout << prepend << "\t+?" << std::endl;
 //     std::cout << std::endl;
		simplification_result = false;
		return;
    }
  int i = 0;
  DisplayMultiMap::iterator it = display_map.begin();

  while ( (it != display_map.end()) && (i < maxAnalyses))
    {
      if (outputType == xerox)
	{
	  //std::cout << prepend << "\t";
	}
      //std::cout << (*it).second << "\n";

	  if(get_first_result_else_shorter)//we are done here
	  {
		  correct_word = (*it).second;
		  remove_everything_after_special_char_and_lower(correct_word);

		  break;
	  }
	  else
	  {
		  string new_word = (*it).second;
		  remove_everything_after_special_char_and_lower(new_word);

		  if(new_word.size() > 0 && (new_word.size() < correct_word.size() || correct_word.size() == 0))
			  correct_word = new_word;
	  }

	//  correct_word = (*it).second;
	  //we got the 1st, taht's enough, quit now
    //  if (displayWeightsFlag)//WE MAY NEED THIS IN THE FUTURE. THE LOWER THE NUMBER THE MROE CORRECT THE SUGGESTION IS
	{
	//  word_quality = (*it).first;
	//	cout << (*it).first << "\n";
	}
	//break;
      //std::cout << std::endl;
      ++it;
      ++i;
    }
  display_map.clear();

  if(correct_word.size() == 0)
	  simplification_result = false;
  else
	  simplification_result = true;

  //std::cout << std::endl;
}
#endif

void TransducerWUniq::printAnalyses(std::string prepend)
{
  if (outputType == xerox && display_map.size() == 0)
    {
      std::cout << prepend << "\t+?" << std::endl;
      std::cout << std::endl;
      return;
    }
  int i = 0;
  std::multimap<Weight, std::string> weight_sorted_map;
  DisplayMap::iterator it = display_map.begin();
  while (it != display_map.end())
    {
      weight_sorted_map.insert(std::pair<Weight, std::string>((*it).second, (*it).first));
      ++it;
    }
  std::multimap<Weight, std::string>::iterator display_it = weight_sorted_map.begin();
  while ( (display_it != weight_sorted_map.end()) && (i < maxAnalyses))
    {
      if (outputType == xerox)
	{
	  std::cout << prepend << "\t";
	}
      std::cout << (*display_it).second;
      if (displayWeightsFlag)
	{
	  std::cout << '\t' << (*display_it).first;
	}
      std::cout << std::endl;
      ++display_it;
      ++i;
    }
  display_map.clear();
  std::cout << std::endl;
}

void TransducerWFdUniq::printAnalyses(std::string prepend)
{
  if (outputType == xerox && display_map.size() == 0)
    {
      std::cout << prepend << "\t+?" << std::endl;
      std::cout << std::endl;
      return;
    }
  int i = 0;
  std::multimap<Weight, std::string> weight_sorted_map;
  DisplayMap::iterator it;
  for (it = display_map.begin(); it != display_map.end(); it++)
    {
      weight_sorted_map.insert(std::pair<Weight, std::string>((*it).second, (*it).first));
    }
  std::multimap<Weight, std::string>::iterator display_it;
  for (display_it = weight_sorted_map.begin();
       display_it != weight_sorted_map.end(), i < maxAnalyses;
       display_it++, i++)
    {
      if (outputType == xerox)
	{
	  std::cout << prepend << "\t";
	}
      std::cout << (*display_it).second;
      if (displayWeightsFlag)
	{
	  std::cout << '\t' << (*display_it).first;
	}
      std::cout << std::endl;
    }
  display_map.clear();
  std::cout << std::endl;
}

void TransducerW::get_analyses(SymbolNumber * input_symbol,
			       SymbolNumber * output_symbol,
			       SymbolNumber * original_output_string,
			       TransitionTableIndex i)
{
#if OL_FULL_DEBUG
  std::cerr << "get analyses " << i << " " << current_weight << std::endl;
#endif
  if (i >= TRANSITION_TARGET_TABLE_START )
    {
      i -= TRANSITION_TARGET_TABLE_START;
      
      try_epsilon_transitions(input_symbol,
			      output_symbol,
			      original_output_string,
			      i+1);
      
      // input-string ended.
      if (*input_symbol == NO_SYMBOL_NUMBER)
	{
	  *output_symbol = NO_SYMBOL_NUMBER;
	  if (transitions.size() <= i) 
	    {
	      return;
	    }
	  if (final_transition(i))
	    {
	      current_weight += get_final_transition_weight(i);
	      note_analysis(original_output_string);
	      current_weight -= get_final_transition_weight(i);
	    }
	  return;
	}
      
      SymbolNumber input = *input_symbol;
      ++input_symbol;
      
      
      find_transitions(input,
		       input_symbol,
		       output_symbol,
		       original_output_string,
		       i+1);
    }
  else
    {
      
      try_epsilon_indices(input_symbol,
			  output_symbol,
			  original_output_string,
			  i+1);
      // input-string ended.
      if (*input_symbol == NO_SYMBOL_NUMBER)
	{
	  *output_symbol = NO_SYMBOL_NUMBER;
	  if (final_index(i))
	    {
	      current_weight += get_final_index_weight(i);
	      note_analysis(original_output_string);
	      current_weight -= get_final_index_weight(i);
	    }
	  return;
	}
      
      SymbolNumber input = *input_symbol;
      ++input_symbol;
      
      find_index(input,
		 input_symbol,
		 output_symbol,
		 original_output_string,
		 i+1);
    }
}
