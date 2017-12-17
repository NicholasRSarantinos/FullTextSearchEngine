#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <ctime>
#include <direct.h>
#include <stdio.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>
#include <map>
#include <fcntl.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <share.h>
#include <vector>
#include <thread>
#include <sstream>
#include <unordered_map>
#include <boost/filesystem.hpp>
#include <time.h>
#include <boost/thread.hpp>
#include "avltree.h"
#include "Code203_Tree.h"
#include "libtree.h"
#include <boost/intrusive/avl_set.hpp>
#include <boost/intrusive/avltree_algorithms.hpp>
#include <boost/intrusive/set.hpp>
#include <boost/intrusive/unordered_set.hpp>
#include "avl.h"
#include <boost/crc.hpp>
#include <boost/functional/hash.hpp>
#include "boost/range/algorithm/sort.hpp"
#include "tbb/parallel_sort.h"

static const bool SUBREDDIT_ID_USE = false;
#include <stdio.h>
#include <stdlib.h>      
#include <ctype.h>       
#include "definitions.h"
#include "word_freq.h"

using namespace std;
using namespace rapidjson;

string base_work_directory = DEF_base_work_directory;

string data_file_dictionary_directory = DEF_data_file_dictionary_directory;

string data_file_merged_directory = DEF_data_file_merged_directory;

int part_creation_threads_num = THREADS_TO_USE;

extern const string data_file_folder_list[];
extern const string data_file_list[];

extern const int items_parts_total_number;

extern const bool get_first_result_else_shorter_const;
extern const bool kill_word_outside_of_dictionary_const;

void create_file_and_fill(string filename, void* data, unsigned long long size)
{
	ofstream out_file(filename, ios::out | ios::trunc | ios::binary);
	if(size > 0) out_file.write((char*)data, size);
	out_file.close();
}

string read_json_body(char* input)
{
	Document document;
	document.Parse(input);

	return document["body"].GetString();
}

size_t get_file_size(string name)
{
	ifstream file(name.c_str(), ios::binary | ios::in);

	if(!file.is_open())
	{
		cout << "INPUT FILE DOESN'T EXIST " << name << "\n";
		cin.get();
	}

	file.seekg (0, file.end);
	size_t length = file.tellg();
	file.seekg (0, file.beg);

	file.close();

	return length;
}
#include <iostream>
#include <map>

using namespace std;

unsigned int max_write_buffer_size = 10*1024*1024;
char *write_buffer = new char[max_write_buffer_size];
unsigned int current_pos = 0;

extern "C" {
void do_start(int argc, char **argv, char **write_buffer, unsigned int *max_write_buffer_size, unsigned int *current_pos);
}
struct MyHandler {

	bool found_body;
	char* result;
	MyHandler() {found_body = false; result = 0;}
	

    bool Null() {  return true; }
    bool Bool(bool b) {  return true; }
    bool Int(int i) {  return true; }
    bool Uint(unsigned u) {  return true; }
    bool Int64(int64_t i) {  return true; }
    bool Uint64(uint64_t u) {  return true; }
    bool Double(double d) {  return true; }
    bool String(const char* str, SizeType length, bool copy) { 

		if(found_body)
		{
			result = new char[length+1];
			strcpy(result, (char*)str);
			found_body = false;
		}
        
        return true;
    }
    bool StartObject() {  return true; }
    bool Key(const char* str, SizeType length, bool copy) { 

		if(length == 4 && strcmp(str, "body") == 0)
		{
			if(found_body) cout << "JSON HAS 2 BODIES!!!!\n";
			found_body = true;
		}
        
        return true;
    }
    bool EndObject(SizeType memberCount) {  return true; }
    bool StartArray() {  return true; }
    bool EndArray(SizeType elementCount) {  return true; }
};

char* fast_read_json_body(char* json) {
    
    MyHandler handler;
    Reader reader;
    StringStream ss(json);
    reader.Parse(ss, handler);

	if(handler.result != 0) return handler.result;
	else { cout << "JSON ERROR!\n"; cin.get(); return ""; }
}

class get_item_score
{
private:
	static inline unsigned int get_word_value_perc(const unsigned int &main_unique_words, const unsigned int &main_avg_word_rank, const unsigned int &current_total_word_rank)
	{
		if(main_unique_words == 0 || main_avg_word_rank == 0) return 0;
		
		

		return 250.0*current_total_word_rank/(main_avg_word_rank*main_unique_words);

		
	}

	static inline unsigned int get_word_value_perc_of_input(const unsigned int &input_sentence_word_value_sum, const unsigned int &main_avg_word_rank, const unsigned int &main_unique_words)
	{
		if(main_avg_word_rank == 0 || main_unique_words == 0 || input_sentence_word_value_sum == 0) return 0;

		unsigned int total_rank = main_avg_word_rank*main_unique_words;

		unsigned long long word_value_perc_of_input = 0;
		if(input_sentence_word_value_sum < total_rank)
			word_value_perc_of_input = 250.0*input_sentence_word_value_sum/total_rank;
		else
			word_value_perc_of_input = 250.0*total_rank/input_sentence_word_value_sum;

		return word_value_perc_of_input;
	}
	static inline unsigned int get_word_existing_number_match_perc(const unsigned int &main_unique_words, const unsigned int &current_unique_words)
	{
		if(main_unique_words == 0) return 0;

		return 250.0*current_unique_words/main_unique_words;
	}
	
	static inline unsigned int get_unqiue_words_number_match_rate(const unsigned int &unique_words_num_in_sentence, const unsigned int &main_unique_words)
	{

		if(main_unique_words == 0 || unique_words_num_in_sentence == 0) return 0;

		unsigned long long word_total_number_match_perc = 0;
		if(unique_words_num_in_sentence < main_unique_words)
			word_total_number_match_perc = 250.0*unique_words_num_in_sentence/main_unique_words;
		else
			word_total_number_match_perc = 250.0*main_unique_words/unique_words_num_in_sentence;

		return word_total_number_match_perc;
	}

public:
	static inline unsigned char get_final_score(
		const unsigned int &unique_words_num_in_sentence, const unsigned int &input_sentence_word_value_sum,
		unsigned int main_unique_words_item, unsigned int current_unique_words_item,
		unsigned int main_avg_word_rank_item, unsigned int current_total_word_rank_item,
		bool return_everything = false
		)
	{
		

		unsigned int
			min_final = 70		*2.5;

		unsigned int
			a_min_perc = 50			*2.5,
			a0_min_perc = 50		*2.5,
			b_min_perc = 30			*2.5,
			c_min_perc = 50			*2.5;
		
		
		
		

		

		
		
		
		

		if(return_everything)
		{
			min_final = 0;
			a_min_perc = 0;
			a0_min_perc = 0;
			b_min_perc = 0;
			c_min_perc = 0;
		}

		unsigned int b = get_word_existing_number_match_perc(main_unique_words_item, current_unique_words_item);

		if(b >= b_min_perc) {

			unsigned int a = get_word_value_perc(main_unique_words_item, main_avg_word_rank_item, current_total_word_rank_item);

			if(a >= a_min_perc) {

				unsigned int c = get_unqiue_words_number_match_rate(unique_words_num_in_sentence, main_unique_words_item);

				if(c >= c_min_perc) {
					
					unsigned int a0 = get_word_value_perc_of_input(input_sentence_word_value_sum, main_avg_word_rank_item, main_unique_words_item);

					if(a0 >= a0_min_perc)
					{
						double
							a_mul = 2.5,
							a0_mul = 1,
							b_mul = 0.1,
							c_mul = 0.1;
						unsigned char now_final = ((unsigned int)a*a_mul+b*b_mul+c*c_mul+a0*a0_mul)/(a_mul+b_mul+c_mul+a0_mul);
						return ((now_final >= min_final) ? now_final : 0);
						
						
						
					}
					else return 0;
				} else return 0;
			} else return 0;
		} else return 0;
	}
};
class dual_sort
{
public:
	template <class T, class T2>
	struct helper_type {
		public:
			typedef std::tuple<typename iterator_traits<T>::value_type, typename iterator_traits<T2>::value_type> value_type;
			typedef std::tuple<typename iterator_traits<T>::value_type&, typename iterator_traits<T2>::value_type&> ref_type;
	};

	template <typename T1, typename T2>
	class dual_iterator : public boost::iterator_facade<dual_iterator<T1, T2>,
														typename helper_type<T1, T2>::value_type,
														boost::random_access_traversal_tag,
														typename helper_type<T1, T2>::ref_type> {
	public:
		explicit dual_iterator(T1 iter1, T2 iter2) : mIter1(iter1), mIter2(iter2) {}
		typedef typename iterator_traits<T1>::difference_type difference_type;
	private:
		void increment() { ++mIter1; ++mIter2; }
		void decrement() { --mIter1; --mIter2; }
		bool equal(dual_iterator const& other) const { return mIter1 == other.mIter1; }
		typename helper_type<T1, T2>::ref_type dereference() const { return (typename helper_type<T1, T2>::ref_type(*mIter1, *mIter2)); }
		difference_type distance_to(dual_iterator const& other) const { return other.mIter1 - mIter1; }
		void advance(difference_type n) { mIter1 += n; mIter2 += n; }

		T1 mIter1;
		T2 mIter2;
		friend class boost::iterator_core_access;
	};

	template <typename T1, typename T2>
	dual_iterator<T1, T2> make_iter(T1 t1, T2 t2) { return dual_iterator<T1, T2>(t1, t2); }

	template <class T1, class T2> struct iter_comp_asc {
		typedef typename helper_type<T1, T2>::value_type T;
		bool operator()(const T& t1, const T& t2) const { return get<0>(t1) < get<0>(t2); }
	};

	template <class T1, class T2> iter_comp_asc<T1, T2> make_comp_asc(T1 t1, T2 t2) { return iter_comp_asc<T1, T2>(); }

	template <class T1, class T2> struct iter_comp_desc {
		typedef typename helper_type<T1, T2>::value_type T;
		bool operator()(const T& t1, const T& t2) const { return get<0>(t1) > get<0>(t2); }
	};

	template <class T1, class T2> iter_comp_desc<T1, T2> make_comp_desc(T1 t1, T2 t2) { return iter_comp_desc<T1, T2>(); }

	

	template <class T1, class T2> struct iter_comp_asc_str {
		typedef typename helper_type<T1, T2>::value_type T;
		bool operator()(const T& t1, const T& t2) const { return strcmp((char*)get<0>(t1), (char*)get<0>(t2)) == -1; }
	};

	template <class T1, class T2> iter_comp_asc_str<T1, T2> make_comp_asc_str(T1 t1, T2 t2) { return iter_comp_asc_str<T1, T2>(); }

	template <class T1, class T2> struct iter_comp_desc_str {
		typedef typename helper_type<T1, T2>::value_type T;
		bool operator()(const T& t1, const T& t2) const { return strcmp((char*)get<0>(t1), (char*)get<0>(t2)) == 1; }
	};

	template <class T1, class T2> iter_comp_desc_str<T1, T2> make_comp_desc_str(T1 t1, T2 t2) { return iter_comp_desc_str<T1, T2>(); }

} do_dual_sort;

class triple_sort
{
public:
	template <class T1, class T2, class T3>
	struct helper_type {
		public:
			typedef std::tuple<typename iterator_traits<T1>::value_type, typename iterator_traits<T2>::value_type, typename iterator_traits<T3>::value_type> value_type;
			typedef std::tuple<typename iterator_traits<T1>::value_type&, typename iterator_traits<T2>::value_type&, typename iterator_traits<T3>::value_type&> ref_type;
	};

	template <typename T1, typename T2, typename T3>
	class dual_iterator : public boost::iterator_facade<dual_iterator<T1, T2, T3>,
														typename helper_type<T1, T2, T3>::value_type,
														boost::random_access_traversal_tag,
														typename helper_type<T1, T2, T3>::ref_type> {
	public:
		explicit dual_iterator(T1 iter1, T2 iter2, T3 iter3) : mIter1(iter1), mIter2(iter2), mIter3(iter3) {}
		typedef typename iterator_traits<T1>::difference_type difference_type;
	private:
		void increment() { ++mIter1; ++mIter2; ++mIter3; }
		void decrement() { --mIter1; --mIter2; --mIter3; }
		bool equal(dual_iterator const& other) const { return mIter1 == other.mIter1; }
		typename helper_type<T1, T2, T3>::ref_type dereference() const { return (typename helper_type<T1, T2, T3>::ref_type(*mIter1, *mIter2, *mIter3)); }
		difference_type distance_to(dual_iterator const& other) const { return other.mIter1 - mIter1; }
		void advance(difference_type n) { mIter1 += n; mIter2 += n; mIter3 += n; }

		T1 mIter1;
		T2 mIter2;
		T3 mIter3;
		friend class boost::iterator_core_access;
	};

	template <typename T1, typename T2, typename T3>
	dual_iterator<T1, T2, T3> make_iter(T1 t1, T2 t2, T3 t3) { return dual_iterator<T1, T2, T3>(t1, t2, t3); }

	template <class T1, class T2, class T3> struct iter_comp_asc {
		typedef typename helper_type<T1, T2, T3>::value_type T;
		bool operator()(const T& t1, const T& t2) const { return get<0>(t1) < get<0>(t2); }
	};

	template <class T1, class T2, class T3> iter_comp_asc<T1, T2, T3> make_comp_asc(T1 t1, T2 t2, T3 t3) { return iter_comp_asc<T1, T2, T3>(); }

	template <class T1, class T2, class T3> struct iter_comp_desc {
		typedef typename helper_type<T1, T2, T3>::value_type T;
		bool operator()(const T& t1, const T& t2) const { return get<0>(t1) > get<0>(t2); }
	};

	template <class T1, class T2, class T3> iter_comp_desc<T1, T2, T3> make_comp_desc(T1 t1, T2 t2, T3 t3) { return iter_comp_desc<T1, T2, T3>(); }

	

	template <class T1, class T2, class T3> struct iter_comp_asc_str {
		typedef typename helper_type<T1, T2, T3>::value_type T;
		bool operator()(const T& t1, const T& t2) const { return strcmp((char*)get<0>(t1), (char*)get<0>(t2)) == -1; }
	};

	template <class T1, class T2, class T3> iter_comp_asc_str<T1, T2, T3> make_comp_asc_str(T1 t1, T2 t2, T3 t3) { return iter_comp_asc_str<T1, T2, T3>(); }

	template <class T1, class T2, class T3> struct iter_comp_desc_str {
		typedef typename helper_type<T1, T2, T3>::value_type T;
		bool operator()(const T& t1, const T& t2) const { return strcmp((char*)get<0>(t1), (char*)get<0>(t2)) == 1; }
	};

	template <class T1, class T2, class T3> iter_comp_desc_str<T1, T2, T3> make_comp_desc_str(T1 t1, T2 t2, T3 t3) { return iter_comp_desc_str<T1, T2, T3>(); }

} do_triple_sort;

class single_sort
{
public:
	template<class T>
	class iter_comp_asc
	{
	public:
		template<class T>
		bool operator()(T const &a, T const &b) const { return a < b; }
	};

	template <class T> iter_comp_asc<T> make_comp_asc(T t) { return iter_comp_asc<T>(); }

	template<class T>
	class iter_comp_desc
	{
	public:
		template<class T>
		bool operator()(T const &a, T const &b) const { return a > b; }
	};

	template <class T> iter_comp_desc<T> make_comp_desc(T t) { return iter_comp_desc<T>(); }
	
	

	template<class T>
	class iter_comp_asc_str
	{
	public:
		template<class T>
		bool operator()(T const &a, T const &b) const { return strcmp((char*)a, (char*)b) == -1; }
	};

	template <class T> iter_comp_asc_str<T> make_comp_asc_str(T t) { return iter_comp_asc_str<T>(); }

	template<class T>
	class iter_comp_desc_str
	{
	public:
		template<class T>
		bool operator()(T const &a, T const &b) const { return strcmp((char*)a, (char*)b) == 1; }
	};

	template <class T> iter_comp_desc_str<T> make_comp_desc_str(T t) { return iter_comp_desc_str<T>(); }
	
} do_single_sort;

class blaze_data_sorter
{
public:
	template <typename T1, typename T2, typename T3>
	static void sort(T1 *array_1, T2 *array_2, T3 *array_3, unsigned long long array_size, bool sort_desc = true)
	{
		if(array_size <= 1) return;

		if(sort_desc)
		{
			if(typeid(T1) == typeid(char*) || typeid(T1) == typeid(const char*))
			{
				std::sort(do_triple_sort.make_iter(array_1, array_2, array_3), 
				do_triple_sort.make_iter(array_1+array_size, array_2+array_size, array_3+array_size),
				do_triple_sort.make_comp_desc_str(array_1, array_2, array_3));
			}
			else
			{
				std::sort(do_triple_sort.make_iter(array_1, array_2, array_3), 
				do_triple_sort.make_iter(array_1+array_size, array_2+array_size, array_3+array_size),
				do_triple_sort.make_comp_desc(array_1, array_2, array_3));
			}
		}
		else
		{
			if(typeid(T1) == typeid(char*) || typeid(T1) == typeid(const char*))
			{
				std::sort(do_triple_sort.make_iter(array_1, array_2, array_3), 
				do_triple_sort.make_iter(array_1+array_size, array_2+array_size, array_3+array_size),
				do_triple_sort.make_comp_asc_str(array_1, array_2, array_3));
			}
			else
			{
				std::sort(do_triple_sort.make_iter(array_1, array_2, array_3), 
				do_triple_sort.make_iter(array_1+array_size, array_2+array_size, array_3+array_size),
				do_triple_sort.make_comp_asc(array_1, array_2, array_3));
			}
		}
	}
	
	template <typename T1, typename T2>
	static void sort(T1 *array_1, T2 *array_2, unsigned long long array_size, bool sort_desc = true)
	{
		if(array_size <= 1) return;

		if(sort_desc)
		{
			
			if(typeid(T1) == typeid(char*) || typeid(T1) == typeid(const char*))
			{
				std::sort(do_dual_sort.make_iter(array_1, array_2), 
				do_dual_sort.make_iter(array_1+array_size, array_2+array_size),
				do_dual_sort.make_comp_desc_str(array_1, array_2));
			}
			else
			{
				std::sort(do_dual_sort.make_iter(array_1, array_2), 
				do_dual_sort.make_iter(array_1+array_size, array_2+array_size),
				do_dual_sort.make_comp_desc(array_1, array_2));
			}
		}
		else
		{
			if(typeid(T1) == typeid(char*) || typeid(T1) == typeid(const char*))
			{
				std::sort(do_dual_sort.make_iter(array_1, array_2), 
				do_dual_sort.make_iter(array_1+array_size, array_2+array_size),
				do_dual_sort.make_comp_asc_str(array_1, array_2));
			}
			else
			{
				std::sort(do_dual_sort.make_iter(array_1, array_2), 
				do_dual_sort.make_iter(array_1+array_size, array_2+array_size),
				do_dual_sort.make_comp_asc(array_1, array_2));
			}
		}
	}
	
	template <typename T1>
	static void sort(T1 *array_1, unsigned long long array_size, bool sort_desc = true)
	{
		if(array_size <= 1) return;

		if(sort_desc)
		{
			if(typeid(T1) == typeid(char*) || typeid(T1) == typeid(const char*))
			{
				tbb::parallel_sort(array_1, array_1+array_size, 
				do_single_sort.make_comp_desc_str(array_1));
			}
			else
			{
				tbb::parallel_sort(array_1, array_1+array_size, 
				do_single_sort.make_comp_desc(array_1));
			}
		}
		else
		{
			if(typeid(T1) == typeid(char*) || typeid(T1) == typeid(const char*))
			{
				tbb::parallel_sort(array_1, array_1+array_size, 
				do_single_sort.make_comp_asc_str(array_1));
			}
			else
			{
				tbb::parallel_sort(array_1, array_1+array_size, 
				do_single_sort.make_comp_asc(array_1));
			}
		}
	}

	template <typename T1>
	static void reverse(T1 *array_1, unsigned long long array_size)
	{
		if(array_size > 1)
			for(unsigned long long i = 0, j = array_size-1; i < array_size/2; i++, j--)
			{
				T1 temp = array_1[i];
				array_1[i] = array_1[j];
				array_1[j] = temp;
			}
	}
} sort_arrays;

class synced_iofstream
{
	static const unsigned int max_tickets_num = 50;
	int using_it_id;
	unsigned int users_num;
	char *ticket_priorities;
	

	bool 
		
		looking_for_ticket_started;

	int get_new_ticket(bool high_priority)
	{
		while(looking_for_ticket_started) boost::this_thread::sleep(boost::posix_time::milliseconds(1));

		looking_for_ticket_started = true;

		if(users_num == max_tickets_num) {cout << "ERROR: TICKETS SOLD OUT, THIS IS A BUG LOL (SRS)\n"; cin.get();}

		for(unsigned int i = 0; i < max_tickets_num; i++)
		{
			if(ticket_priorities[i] == 0)
			{
				ticket_priorities[i] = high_priority ? 2 : 1;
				users_num++;
				return i;
			}
		}
	}

public:
	synced_iofstream()
	{
		using_it_id = -1;
		users_num = 0;
		ticket_priorities = new char[max_tickets_num]();
		
		looking_for_ticket_started = false;
	}

	void wait_for_access(bool high_priority)
	{
		int ticket = get_new_ticket(high_priority);

		looking_for_ticket_started = false;

		while(using_it_id != -1) boost::this_thread::sleep(boost::posix_time::milliseconds(1));

		using_it_id = ticket;
	}

	void free_access()
	{
		users_num--;
		ticket_priorities[using_it_id] = 0;
		using_it_id = -1;
	}
};

class data_sorter
{
private:
	template <typename T1>
	static T1 item_max(T1 &x, T1 &y, bool &sort_desc)
	{
		if((sort_desc && x > y) || (!sort_desc && x < y)) return x;
		else return y;
	}

	static char* item_max(char* &x, char* &y, bool &sort_desc)
	{
		if((sort_desc && strcmp(x, y) == 1) || (!sort_desc && strcmp(x, y) == -1)) return x;
		else return y;
	}

	template <typename T1>
	static bool equal(T1 x, T1 y)
	{
		return x == y;
	}

	static bool equal(char* x, char* y)
	{
		return strcmp(x, y) == 0;
	}

	template <typename T1, typename T2, typename T3>
	static void run_merge(T1 *&array_1, T2 *&array_2, T3 *&array_3, unsigned long long left, unsigned long long right, T1 *&scratch_1, T2 *&scratch_2, T3 *&scratch_3, bool &sort_desc)
	{
		
		if(right == left + 1)
			return;
		else
		{
			unsigned long long 
				i = 0,
				length = right - left,
				midpoint_distance = length/2,
				l = left, r = left + midpoint_distance;
			

			
			run_merge(array_1, array_2, array_3, left, left + midpoint_distance, scratch_1, scratch_2, scratch_3, sort_desc);
			run_merge(array_1, array_2, array_3, left + midpoint_distance, right, scratch_1, scratch_2, scratch_3, sort_desc);

			 
			for(i = 0; i < length; i++)
			{
				
				if(l < left + midpoint_distance && (r == right || equal(item_max(array_1[l], array_1[r], sort_desc), array_1[l])))
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
			
			for(i = left; i < right; i++)
			{
				array_1[i] = scratch_1[i - left];
				if(array_2 != nullptr) array_2[i] = scratch_2[i - left];
				if(array_3 != nullptr) array_3[i] = scratch_3[i - left];
			}
		}
	}

	template <typename T1, typename T2, typename T3>
	static void run_sort(T1 *array_1, T2 *array_2, T3 *array_3, unsigned long long array_size, bool sort_desc)
	{
		if(array_size > 1)
		{
			T1* scratch_1 = new T1[array_size];
			T2* scratch_2 = (array_2 != nullptr) ? new T2[array_size] : nullptr;
			T3* scratch_3 = (array_3 != nullptr) ? new T3[array_size] : nullptr;

			unsigned long long start_pos = 0;
			run_merge(array_1, array_2, array_3, start_pos, array_size, scratch_1, scratch_2, scratch_3, sort_desc);

			delete []scratch_1;
			if(scratch_2 != nullptr) delete []scratch_2;
			if(scratch_3 != nullptr) delete []scratch_3;
		}
	}

public:
	template <typename T1, typename T2, typename T3>
	static void sort(T1 *array_1, T2 *array_2, T3 *array_3, unsigned long long array_size, bool sort_desc = true)
	{
		run_sort(array_1, array_2, array_3, array_size, sort_desc);
	}

	template <typename T1, typename T2>
	static void sort(T1 *array_1, T2 *array_2, unsigned long long array_size, bool sort_desc = true)
	{
		char* array_3 = nullptr;
		run_sort(array_1, array_2, array_3, array_size, sort_desc);
	}

	template <typename T1>
	static void sort(T1 *array_1, unsigned long long array_size, bool sort_desc = true)
	{
		char* array_2 = nullptr;
		char* array_3 = nullptr;
		run_sort(array_1, array_2, array_3, array_size, sort_desc);
	}

	template <typename T1>
	static void reverse(T1 *array_1, unsigned long long array_size)
	{
		if(array_size > 1)
			for(unsigned long long i = 0, j = array_size-1; i < array_size/2; i++, j--)
			{
				T1 temp = array_1[i];
				array_1[i] = array_1[j];
				array_1[j] = temp;
			}
	}

} old_sort_arrays;

void sort_positions_based_on_hashes_and_then_words(int *hashes, char **words, unsigned int *positions, unsigned int items_no)
{
	sort_arrays.sort(hashes, words, positions, items_no);

	for(unsigned int i = 1, same = 1; i < items_no; i++, same = 1)
	{
		while(hashes[i] == hashes[i-1] && i < items_no)
		{
			same++;
			i++;
		}

		if(same > 1) sort_arrays.sort(&words[i-same], &hashes[i-same], &positions[i-same], same);
	}
}

get_item_score item_scorer;

class synonym_holder
{
private:
	char *data;
	unsigned int size;
public:
	synonym_holder()
	{
		size = 1;
	}

	void add(char *input, string &word)
	{
		--size;
		char *buff = new char[strlen(input)+1+size+word.size()+1];
		memcpy(buff, data, size);
		delete []data;
		data = buff;

		unsigned int pos = 0;
		bool now_space = false;

		memcpy(&data[size], word.c_str(), word.size());
		size += word.size();
		data[size++] = ' ';

		while(input[pos] != 0)
		{
			if(input[pos] >= 'A' && input[pos] <= 'Z') input[pos] += ('a'-'A');

			if((input[pos] >= 'a' && input[pos] <= 'z') || (input[pos] >= '0' && input[pos] <= '9') || input[pos] == '.' || input[pos] == '-' || input[pos] == '\'' || input[pos] == '_')
			{
				now_space = false;
				data[size++] = input[pos];
			}
			else if(input[pos] == ' ' || input[pos] == '\n')
			{
				if(!now_space)
				{
					data[size++] = ' ';
					now_space = true;
				}
			}
			else if(input[pos] != '=' && input[pos] != '>' && input[pos] != ',')
			{cout << "INPUT ERROR![ " << input[pos] << " ]\n\n\n";cin.get();cout << input << "\n";}

			pos++;
		}

		data[size++] = 0;
	}

	bool exists(char *input)
	{
		return strstr(data, input) != NULL;
	}

	bool exists(synonym_holder &input)
	{
		unsigned int pos = 0;

		char *buff = new char[input.size+1];
		unsigned int buff_pos = 0;

		while(input.data[pos] != 0)
		{
			while(input.data[pos] != ' ' && input.data[pos] != 0)
			{
				buff[buff_pos++] = input.data[pos];
				pos++;
			}
			pos++;

			buff[buff_pos++] = 0;

			if(exists(buff))
			{
				delete []buff;
				return true;
			}

			buff_pos = 0;
		}

		delete []buff;
		return false;
	}

	~synonym_holder()
	{
		
		delete []data;
	}
};

void find_synonyms(string &search_for, string &option, synonym_holder& s)
{
	current_pos = 0;

	unsigned int var_count = 3;
	char ** variables = new char*[var_count+1];
	

	const char* program_name = "word_net_project_x";
	variables[0] = new char[strlen(program_name)+1];
	strcpy(variables[0], program_name);

	variables[1] = new char[search_for.size()+1];
	strcpy(variables[1], search_for.c_str());

	variables[2] = new char[option.size()+1];
	strcpy(variables[2], option.c_str());

	variables[3] = 0;

	do_start(var_count, variables, &write_buffer, &max_write_buffer_size, &current_pos);

	delete [] variables[0];
	delete [] variables[1];
	delete [] variables[2];

	delete [] variables;

	s.add(write_buffer, search_for);
}

void find_a_synonym(synonym_holder &s_1, char *word)
{
	string options[] = {"-synsn", "-synsv"};

	for(int i = 0; i < sizeof(options)/sizeof(options[0]); i++)
		find_synonyms(string(word), options[i], s_1);
}

bool kill_comment(char** search_for_1, int size_1, char** search_for_2, int size_2)
{
	synonym_holder s_1;
	
	for(int j = 0; j < size_1; j++)
		find_a_synonym(s_1, search_for_1[j]);

	for(int i = 0; i < size_2; i++)
	{
		if(!s_1.exists(search_for_2[i]))
		{
			synonym_holder s_x;
			find_a_synonym(s_x, search_for_2[i]);

			if(!s_1.exists(s_x))
			{cout << "\t\t" << search_for_2[i] << " - "; return true;}
			else
				{}
		}
		else
			{}
	}

	return false;
}

template <typename BIG_TYPE, typename SMALL_TYPE>
class var_byte_array
{
private:
	SMALL_TYPE* main_array;
	unsigned int main_items_no;

	BIG_TYPE* exceptions_array;
	unsigned int* exceptions_array_pos;
	unsigned int exceptions_no;

	SMALL_TYPE max_common_value;

	
	
	unsigned int max_exceptions_no;
	

	BIG_TYPE binary_search(unsigned int pos)
	{
		int low = 0, high = exceptions_no-1, mid;

		while(low <= high)
		{
			mid = (low + high)/2;

			if(exceptions_array_pos[mid] == pos)
				return exceptions_array[mid];
			else if(exceptions_array_pos[mid] < pos)
				low = mid+1; 
			else
				high = mid-1;
		}

		cout << "NOT EXPECTED BS! " << pos << "\n";
		cin.get();

		return 0;
	}

	unsigned int linear_search(unsigned int pos)
	{
		for(int i = 0; i < exceptions_no; i++)
			if(exceptions_array_pos[i] == pos)
				return i;

		cout << "NOT EXPECTED LS! " << pos << "\n";
		cin.get();

		return 0;
	}

public:
	var_byte_array()
	{
		if(sizeof(SMALL_TYPE) >= sizeof(BIG_TYPE))
		{
			cout << "TYPE ERROR\n";
			cin.get();
		}
	}

	void init(BIG_TYPE* input_array, unsigned int input_array_size)
	{
		

		if(input_array_size == 0) {cout << "NO SIZE!\n"; cin.get();}

		main_items_no = input_array_size;

		max_common_value = std::numeric_limits<SMALL_TYPE>::max()-1;

		main_array = new SMALL_TYPE[input_array_size];

		exceptions_no = 0;

		for(int i = 0; i < input_array_size; i++)
			if(input_array[i] > max_common_value)
				exceptions_no++;

		exceptions_array = new BIG_TYPE[exceptions_no];
		exceptions_array_pos = new unsigned int[exceptions_no];
		unsigned int ex_pos_now = 0;

		for(int i = 0; i < input_array_size; i++)
		{
			if(input_array[i] > max_common_value)
			{
				main_array[i] = max_common_value+1;
				exceptions_array[ex_pos_now] = input_array[i];
				exceptions_array_pos[ex_pos_now] = i;
				ex_pos_now++;
			}
			else
				main_array[i] = input_array[i];
		}

		delete []input_array;

		max_exceptions_no = exceptions_no;

		
		
		
	}

	void max_init(unsigned int max_exceptions_no_in, unsigned int input_array_size)
	{
		
		
		max_common_value = std::numeric_limits<SMALL_TYPE>::max()-1;
		max_exceptions_no = max_exceptions_no_in;

		
		
		
		
		
		
		

		main_items_no = input_array_size;

		main_array = new SMALL_TYPE[main_items_no];
		exceptions_array_pos = new unsigned int[max_exceptions_no];
		exceptions_array = new BIG_TYPE[max_exceptions_no];

		
		
		

		memset_to_0();
	}

	
	template <typename TEST_TYPE>
	void max_init(TEST_TYPE *in_arr, unsigned int input_array_size)
	{
		
		
		max_common_value = std::numeric_limits<SMALL_TYPE>::max()-1;
		max_exceptions_no = 0;

		
		for(unsigned int i = 0; i < input_array_size; i++)
			if(in_arr[i] > max_common_value)
				max_exceptions_no++;
		
		
		

		main_items_no = input_array_size;

		main_array = new SMALL_TYPE[main_items_no];
		exceptions_array_pos = new unsigned int[max_exceptions_no];
		exceptions_array = new BIG_TYPE[max_exceptions_no];

		
		
		

		memset_to_0();
	}

	void add(const unsigned int &pos, const BIG_TYPE &value = 1)
	{
		if(pos >= main_items_no) cout << "POS TOO BIG!!!\n";

		if(main_array[pos]+value <= max_common_value)
			main_array[pos] += value;
		else if(main_array[pos] > max_common_value)
		{
			
			exceptions_array[linear_search(pos)] += value;
		}
		else
		{
			
			if(exceptions_no == max_exceptions_no)
			{
				cout << "LIMIT EXTENDED\n";
				cin.get();
			}

			exceptions_array[exceptions_no] = main_array[pos]+value;
			exceptions_array_pos[exceptions_no] = pos;
			exceptions_no++;

			main_array[pos] = max_common_value+1;
		}

		
		
		

			
		
		

		
		
	}

	void finish_adding()
	{
		
		if(exceptions_no > 0)
			sort_arrays.sort(exceptions_array_pos, exceptions_array, exceptions_no, false);
			

		
		
		
			
			
			
		

		
	}

	void memset_to_0()
	{
		exceptions_no = 0;

		memset(main_array, 0, sizeof(SMALL_TYPE)*main_items_no);
		memset(exceptions_array, 0, sizeof(BIG_TYPE)*max_exceptions_no);
		memset(exceptions_array_pos, 0, sizeof(int)*max_exceptions_no);
	}
	

	const BIG_TYPE operator[](const unsigned int &pos)
	{
		if(pos > main_items_no)
			{cout << "ERROR NO SUCH POS!\n"; cin.get();}
		else if(main_array[pos] <= max_common_value)
			return main_array[pos];
		else
			return binary_search(pos);
	}

	

	~var_byte_array()
	{
		delete []main_array;
		delete []exceptions_array;
		delete []exceptions_array_pos;
	}
};

class on_the_fly_top_items_array
{
	unsigned int 
		*main_value,
		*another_value,

		items_no_max,
		items_no_now,

		min_item,
		min_i_pos;

	inline void update_min(const unsigned int &main_value_, const unsigned int &array_pos)
	{
		if(main_value_ < min_item || min_item == 0)
		{
			min_item = main_value_;
			min_i_pos = array_pos;
		}
	}

public:
	void init(unsigned int items_no)
	{
		items_no_max = items_no;

		main_value = new unsigned int[items_no_max];
		another_value = new unsigned int[items_no_max];

		items_no_now = 0;
		min_item = 0;
		min_i_pos = 0;
	}

	void add_item(const unsigned int &main_value_, const unsigned int &another_value_)
	{
		
		if(main_value_ == 0) return;
		
		if(items_no_now < items_no_max)
		{
			main_value[items_no_now] = main_value_;
			another_value[items_no_now] = another_value_;
			update_min(main_value_, items_no_now);
			items_no_now++;
		}
		else if(main_value_ > min_item)
		{
			main_value[min_i_pos] = min_item = main_value_;
			another_value[min_i_pos] = another_value_;

			for(unsigned int i = 0; i < items_no_now; i++) update_min(main_value[i], i);
		}
	}

	bool get_one(unsigned int &main_value_, unsigned int &another_value_, const unsigned int &pos)
	{
		if(pos >= items_no_now)
			return false;

		main_value_ = main_value[pos];
		another_value_ = another_value[pos];
		return true;
	}

	void get_items_sorted(unsigned int *&main_value_, unsigned int *&another_value_, unsigned int &items_no_now_)
	{
		if(items_no_now > 1)
			sort_arrays.sort(main_value, another_value, items_no_now);
		
			

			
		
		main_value_ = main_value;
		another_value_ = another_value;
		items_no_now_ = items_no_now;
	}

	void clear()
	{
		delete []main_value;
		delete []another_value;
	}
};

class virtual_data_file
{
	vector<unsigned long long> 
		file_sizes,
		start_pos,
		seek_positions;

	vector<unique_ptr<ifstream>> file_streams;
	
	unsigned long long 
		total_file_size,
		external_seek_pos;

	unsigned int total_files_number;

	int find_stream_id_based_on_start_pos(unsigned long long now_start_pos)
	{
		for(int i = 0; i < total_files_number; i++)
			if(start_pos[i] <= now_start_pos && start_pos[i]+file_sizes[i] > now_start_pos)
				return i;

		cout << "HEAVY ERROR REQUESTING POS ( " << now_start_pos << " ) DOESN'T EXIST ( MAX IS " << (start_pos[total_files_number-1]+file_sizes[total_files_number-1]-1) << " ) !\n";
		cin.get();
		cin.get();

		return 0;
	}

	void do_read(char* output, unsigned long long length, unsigned long long now_start_pos)
	{
		bool first_round = true;

		unsigned long long 
			bytes_buff = 0,
			data_pos = 0,
			bytes_remaining = length,
			read_until = now_start_pos+length;

		int file_id = find_stream_id_based_on_start_pos(now_start_pos);

		while(bytes_remaining > 0)
		{
			unsigned long long file_reaches_till = file_sizes[file_id]+start_pos[file_id];

			bytes_remaining = (read_until < file_reaches_till ? 0 : read_until-file_reaches_till);

			unsigned long long 
				length_to_read_now = (first_round ? (length-bytes_remaining) : (bytes_buff > file_sizes[file_id] ? file_sizes[file_id] : bytes_buff)),
				file_now_start_from = (first_round ? now_start_pos-start_pos[file_id] : 0);

			

			if(seek_positions[file_id] != file_now_start_from)
				file_streams[file_id]->seekg(file_now_start_from, ios::beg);

			file_streams[file_id]->read((char*)(&output[data_pos]), length_to_read_now);
			seek_positions[file_id] = file_now_start_from+length_to_read_now;

			if(seek_positions[file_id] == file_sizes[file_id])
			{
				file_streams[file_id]->seekg(0, ios::beg);
				seek_positions[file_id] = 0;
			}
			if(seek_positions[file_id] > file_sizes[file_id]) {cout << "HEAVY ERROR!\n"; cin.get(); cin.get();}

			if(file_streams[file_id]->eof())
			{
				cout << "FILE #" << file_id << " | START FROM " << file_now_start_from << " | LENGTH " << length_to_read_now << " | LATER " << bytes_remaining << " [FILESIZE IS " << file_sizes[file_id] << "] [SEEK POS IS " << seek_positions[file_id] << "] ROUND " << first_round << " | INPUT LENGHT " << length << "\n";
				cout << "CLEARING THE FILE. NOT EXPECTED\n"; file_streams[file_id]->clear(); cin.get(); cin.get();
			}
			
			if(length_to_read_now != file_streams[file_id]->gcount()) cout << "WE ASKED FOR " << length << " AND WE GOT " << file_streams[file_id]->gcount() << "\n";

			data_pos += length_to_read_now;
			bytes_buff = bytes_remaining;
			file_id++;

			first_round = false;
		}

		external_seek_pos = now_start_pos+length;
		if(external_seek_pos == total_file_size) external_seek_pos = 0;
	}

	void do_init(string *files_array = NULL, unsigned int total_files_number_in = items_parts_total_number)
	{
		total_files_number = total_files_number_in;
		unsigned long long size_ = 0;

		for(int i = 0; i < total_files_number; i++)
		{
			string now = files_array == NULL ? (base_work_directory+data_file_folder_list[i]+"\\"+data_file_list[i]) : files_array[i];
			
			unsigned long long file_size_now = get_file_size(now);

			file_sizes.push_back(file_size_now);
			start_pos.push_back(size_);
			seek_positions.push_back(0);

			size_ += file_size_now;
			unique_ptr<ifstream> fs_now(new ifstream(now, ios::in | ios::binary));

			if(!fs_now->is_open())
			{
				cout << "INPUT FILE DOESN'T EXIST " << now << "\n";
				cin.get();
			}

			file_streams.push_back(move(fs_now));
		}

		total_file_size = size_;
		external_seek_pos = 0;
	}

public:
	void init()
	{
		do_init();
	}

	void init(string *files_array, unsigned int total_files_number_in)
	{
		do_init(files_array, total_files_number_in);
	}

	void kill()
	{
		for(int i = 0; i < total_files_number; i++)
			file_streams[i]->close();
	}

	void read_data(void* output, unsigned long long length, unsigned long long now_start_pos = ULLONG_MAX)
	{
		if(length == 0) {cout << "ERROR, LENGHT CAN'T BE 0!\n"; cin.get();}
		else if((now_start_pos == ULLONG_MAX ? external_seek_pos : now_start_pos)+length > total_file_size) {cout << "ERROR, YOU ARE ASKING FOR MORE BYTES THAN EXIST\n"; cin.get();}
		else do_read((char*)output, length, (now_start_pos == ULLONG_MAX ? external_seek_pos : now_start_pos));
	}

	unsigned long long get_size()
	{
		return total_file_size;
	}
};

virtual_data_file main_data_file;

class ifstream_continous_buffered
{
private:
	unique_ptr<ifstream> in_file;
	virtual_data_file *virtual_file;
	bool used_virtual_file;

	char* data_buffer;

	unsigned long long 
		bytes_left,
		bytes_now,

		buffer_size,
		buffer_pos;

	bool last_read;

	char read_mode;

	void read_bytes_from_source()
	{
		if(used_virtual_file)
			virtual_file->read_data(data_buffer, bytes_now);
		else
			in_file->read(data_buffer, bytes_now);
	}

	void fill_buffer()
	{
		if(buffer_pos != bytes_now) { cout << "FIRST USE THE BUFFER AND THEN READ MORE\n"; cin.get(); }

		if(!last_read)
		{
			last_read = bytes_left <= buffer_size;
			bytes_now = last_read ? bytes_left : buffer_size;
			
			read_bytes_from_source();

	
		
			buffer_pos = 0;

			bytes_left -= bytes_now;

			
		}
	}

	int read_line(string &output)
	{
		

		unsigned int 
			start_pos = buffer_pos, 
			data_size = 0;

		while(buffer_pos < bytes_now && data_buffer[buffer_pos] != '\n')
		{
			data_size++;
			buffer_pos++;
		}
		if(data_size > 0)
			output += string(&data_buffer[start_pos], data_size);
		else if(output.size() > 1024*1024*10)
			cout << "NOT AN ERROR (so we don't stop, BUT THIS LINE IS HUGE!!!!! (> 10MB) " << output.size() << "\n";

		if(buffer_pos < bytes_now && data_buffer[buffer_pos] == '\n')
			buffer_pos++;
		else if(buffer_pos == bytes_now && data_buffer[buffer_pos-1] != '\n')
		{
			

			if(!last_read)
				return 1;
			else
			{

				output = "";
				return -1;
			}
		}

		return 0;
	}

	void init()
	{
		read_mode = 0;
		buffer_pos = 0;
		bytes_now = 0;
		last_read = false;
		data_buffer = new char[buffer_size];
		fill_buffer();
	}
public:
	ifstream_continous_buffered(const char *filename, unsigned int buffer_size = 1024*1024*4) : buffer_size(buffer_size), used_virtual_file(false)
	{
		if(buffer_size == 0) { cout << "buffer_size CAN'T BE 0!!!!\n"; buffer_size = 1024*1024*4; }

		unique_ptr<ifstream> in_file_buff(new ifstream(filename, ios::in | ios::binary));

		if(!in_file_buff->is_open())
		{
			cout << "INPUT FILE DOESN'T EXIST " << filename << "\n";
			cin.get();
		}

		in_file = move(in_file_buff);

		bytes_left = get_file_size(filename);
		init();
	}

	ifstream_continous_buffered(virtual_data_file &data_file_in, unsigned int buffer_size = 1024*1024*4) : buffer_size(buffer_size), used_virtual_file(true)
	{
		if(buffer_size == 0) { cout << "buffer_size CAN'T BE 0!!!!\n"; buffer_size = 1024*1024*4; }

		virtual_file = &data_file_in;

		bytes_left = virtual_file->get_size();
		init();
	}

	~ifstream_continous_buffered()
	{
		

		
		
		

		if(!used_virtual_file)
			in_file->close();

		delete [] data_buffer;
	}

	unsigned int read_bytes(void *out, unsigned long long bytes_to_read)
	{
		
		if(read_mode == 1) { cout << "MUST BE USED ONLY TO EITHER READ LINES OR EITHER READ BYTES\n"; cin.get(); }
		if(bytes_to_read == 0) { cout << "ASKING FOR 0 BYTES\n"; cin.get(); }

		read_mode = 2;

		if(buffer_pos == bytes_now && last_read)
			return 0;

		unsigned long long left_to_read = bytes_to_read;

		while(left_to_read > 0)
		{
			if(buffer_pos == bytes_now)
				if(last_read) break;
				else fill_buffer();

			unsigned long long bytes_to_read_from_buffer_now = (bytes_now-buffer_pos >= left_to_read) ? left_to_read : (bytes_now-buffer_pos);

			memcpy(&((char*)out)[bytes_to_read-left_to_read], &data_buffer[buffer_pos], bytes_to_read_from_buffer_now);
			buffer_pos += bytes_to_read_from_buffer_now;
			left_to_read -= bytes_to_read_from_buffer_now;
		}

		return bytes_to_read-left_to_read;
	}

	bool read_a_line(string &output)
	{
		if(read_mode == 2) { cout << "MUST BE USED ONLY TO EITHER READ LINES OR EITHER READ BYTES\n"; cin.get(); }

		read_mode = 1;
		output = "";

		if(buffer_pos == bytes_now && last_read)
			return false;

		int read_result = 1;

		while(read_result == 1)
		{
			read_result = read_line(output);

			
			

			if(read_result == 1)
			{
				if(buffer_pos != bytes_now || last_read) { cout << "NOT EXPECTED BUG!!!!!\n"; cin.get(); }
				fill_buffer();
			}
		}

		
		
			
		
		

		if(buffer_pos == bytes_now && !last_read)
			fill_buffer();

		return read_result == 0;
	}
};

class ofstream_with_buffer
{
private:
	unique_ptr<ofstream> out_file;

	char* data_buffer;

	unsigned long long 
		buffer_size,
		data_pos;

	void flush_now()
	{
		if(data_pos > 0)
		{
			out_file->write((const char*)data_buffer, data_pos);
			data_pos = 0;
		}
	}

public:
	ofstream_with_buffer(const char *filename, unsigned int buffer_size = 1024*1024*4) : buffer_size(buffer_size)
	{
		if(buffer_size == 0) { cout << "buffer_size CAN'T BE 0!!!!\n"; buffer_size = 1024*1024*4; }

		unique_ptr<ofstream> out_file_buff(new ofstream(filename, ios::out | ios::trunc | ios::binary));
		out_file = move(out_file_buff);

		data_buffer = new char[buffer_size];
		data_pos = 0;
	}

	void write(void* data, unsigned long long size)
	{
		if(size == 0) {cout << "CAN'T WRITE 0 BYTES!!!!!!!!!\n";cin.get();}

		if(size+data_pos > buffer_size)
			flush_now();

		if(size > buffer_size)
			out_file->write((const char*)data, size);
		else
		{
			memcpy(&data_buffer[data_pos], data, size);
			data_pos += size;
		}
	}

	void flush_and_close()
	{
		flush_now();
		out_file->close();
		delete [] data_buffer;
	}
};

class compressed_ofstream_with_buffer : public ofstream_with_buffer
{
private:
	char* to_compress;
	unsigned long long to_compress_size;

public:
	compressed_ofstream_with_buffer(const char *filename, unsigned int buffer_size = 1024*1024*4) : ofstream_with_buffer(filename, buffer_size)
	{
		
	}

	void write(void* data, unsigned long long size)
	{
		
	}
};

class reddit_id_class
{
private:
	static void base36_decode(unsigned long long &input, string &output)
	{
		const char base_36[37] = "0123456789abcdefghijklmnopqrstuvwxyz";
		char buffer[14] = {0};
		unsigned int offset = sizeof(buffer);

		buffer[--offset] = '\0';
		do {
			buffer[--offset] = base_36[input % 36];
		} while (input /= 36);

		output = string(&buffer[offset]);
	}

public:
	static void encode_id(string id, unsigned long long &int_id, char &category)
	{
		
	
		

		
		if(id.size() < 4 || id.size() > 16 || id[0] != 't' || id[2] != '_' || id[1] < '1' || id[1] > '8' || id[1] == '7') {cout << "REDDIT ID ERROR!\n"; cin.get();}

		for(int i = 0; i < id.size(); i++) if(id[i] >= 'A' && id[i] <= 'Z') id[i] += ('a'-'A');

		category = id[1]-'0';

		int_id = _strtoui64((id.c_str()+3), NULL, 36);
	}

	static void decode_id(unsigned long long &int_id, char &category, string &id)
	{
		string output;
		base36_decode(int_id, output);

		id = "t"+to_string(category)+"_"+output;
	}
} reddit_id;

int hash_djb2(const char*Word)
{
	unsigned int hash = 5381;

	for(unsigned int c = 0; Word[c] != 0; c++)
		hash = ((hash << 5) + hash) + (unsigned char)Word[c];

	return (hash & 0xFFFFFFFF);
}

struct cmp_char {
	bool operator()(const char* s1, const char* s2) const {
		return (strcmp(s1, s2) == 0);
	}
};

struct hash_char {
	size_t operator()(const char* str) const {
		return hash_djb2(str);
	}
};

class temporal_word_storage
{
	
	char *data;
	unsigned int size, chars_now;
	int prev_word_start_pos;

	void increase_data_size()
	{
		char *new_data = new char[(unsigned int)(size*1.5)];
		memcpy(new_data, data, size);
		delete []data;
		size *= 1.5;
		data = new_data;
	}

	void set_times_found(int *hashes)
	{
		int prev_found = 0;
		times_found[0] = 1;
		unsigned int unique = 1;

		for(unsigned int i = 1; i < words_now; i++)
		{
			if(hashes[prev_found] == hashes[i] && strcmp(words[prev_found], words[i]) == 0)
			{
				if(first_pos[i] < first_pos[prev_found]) first_pos[prev_found] = first_pos[i];
				first_pos[i] = UINT32_MAX;
				times_found[prev_found]++;
			}
			else
			{
				unique++;
				prev_found = i;
				times_found[i] = 1;
			}
		}

		delete []hashes;

		

		sort_arrays.sort(first_pos, words, times_found, words_now, false);

		for(unsigned int i = 0; i < words_now; i++)
		{
			if(first_pos[i] == UINT32_MAX) break;
			first_pos[i] = i;
		}

		words_now = unique;
	}

public:
	
	char **words;
	int *times_found;
	unsigned int *first_pos, words_now;
	

	void create(unsigned int expected_size)
	{
		if(expected_size < 20) expected_size = 20;

		size = expected_size;
		data = new char[size];
		chars_now = 0;
		words_now = 0;
		prev_word_start_pos = 0;
	}

	

	inline void operator<<(const char &in)
	{
		data[chars_now++] = in;
		if(size == chars_now) increase_data_size();

		if(in == '\0')
		{
			if(chars_now == 1 || (chars_now >= 2 && data[chars_now-2] == '\0'))
				chars_now--;
			else
			{
				words_now++;

				unsigned int word_len = chars_now-prev_word_start_pos-1;

				char *word = &data[prev_word_start_pos];
				prev_word_start_pos = chars_now;

				string new_word;
				if(do_run_word_simplifier(word, word_len, new_word))
				{
	

					unsigned int new_word_len = new_word.size();
					while(size <= (chars_now-word_len)+new_word_len) increase_data_size();

					strcpy(word, new_word.c_str());
					chars_now = chars_now-word_len+new_word_len;
					prev_word_start_pos = chars_now;
				}
			}
		}
	}

	void prepare_data()
	{
		if(words_now > 0)
		{
			int *hashes = new int[words_now];
			words = new char*[words_now];
			times_found = new int[words_now]();
			first_pos = new unsigned int[words_now];

			words[0] = &data[0];
			hashes[0] = hash_djb2(words[0]);
			first_pos[0] = 0;

			for(unsigned int i = 0, j = 1; i < chars_now-1; i++)
			{
				if(data[i] == '\0')
				{
					first_pos[j] = j;
					words[j] = &data[i+1];
					hashes[j] = hash_djb2(words[j]);
					j++;
				}
			}

			sort_positions_based_on_hashes_and_then_words(hashes, words, first_pos, words_now);
			set_times_found(hashes);
		}
	}

	void clear()
	{
		delete []data;

		if(words_now > 0)
		{
			delete []words;
			delete []times_found;
			delete []first_pos;
		}
	}
};

class word_stoarge_index_mode
{
private:
	class map_data
	{
	public:
		unsigned int 
			first_pos,
			times_found;

		map_data() : first_pos(0), times_found(0) { }

		map_data(unsigned int first_pos, unsigned int times_found) : 
			first_pos(first_pos), times_found(times_found) { }
	};

	template <typename T>
	void sort_based_on_pos(T *&arr, unsigned int *positions, unsigned int size)
	{
		T *new_arr = new T[size];

		for(unsigned int i = 0; i < size; i++)
			new_arr[i] = arr[positions[i]];

		delete []arr;
		arr = new_arr;
	}

	unsigned int words_stored;
	unordered_map<char*, map_data, hash_char, cmp_char> tree;
	typedef unordered_map<char*, map_data, hash_char, cmp_char>::iterator map_it;

public:
	word_stoarge_index_mode()
	{
		words_stored = 0;
	}

	void clear()
	{
		words_stored = 0;

		
		
		

		tree.clear();
	}

	unsigned int size()
	{
		return words_stored;
	}

	unsigned int add_and_get_id(char* word)
	{
		
		unsigned int word_len = strlen(word);

		string new_word;
		if(do_run_word_simplifier(word, word_len, new_word))
		{
			word = (char*)new_word.c_str();
			word_len = new_word.size();
		}

		auto search = tree.find(word);

		if(search != tree.end())
		{
			search->second.times_found++;
			return search->second.first_pos;
		}
		else
		{
			char* new_word_ = new char[word_len+1];
			strcpy(new_word_, word);

			tree[new_word_] = map_data(words_stored++, 1);
			return tree[new_word_].first_pos;
		}
	}

	void add_every_word_once_and_update_first_pos(word_stoarge_index_mode &input)
	{
		for(map_it it = (input.tree).begin(); it != (input.tree).end(); ++it)
		{
			(it->second).first_pos = add_and_get_id(it->first);
			
			
			delete []it->first;
			
		}
	}

	void get_first_positions(unsigned int *first_positions)
	{
		unsigned int i = 0;

		for(map_it it = tree.begin(); it != tree.end(); ++it)
			first_positions[i++] = it->second.first_pos;
	}

	void get_all_and_clear(char **&words, unsigned int *&first_positions, unsigned int *&times_found, unsigned int *&word_length, unsigned int &items_no)
	{
		items_no = words_stored;

		words = new char*[items_no];
		first_positions = new unsigned int[items_no];
		int *hashes = new int[items_no];

		times_found = new unsigned int[items_no];
		word_length = new unsigned int[items_no];

		unsigned int *positions = new unsigned int[items_no];

		unsigned int i = 0;
		for(auto it = tree.begin(); it != tree.end(); ++it)
		{
			words[i] = it->first;
			hashes[i] = hash_djb2(words[i]);
			first_positions[i] = it->second.first_pos;
			times_found[i] = it->second.times_found;
			word_length[i] = strlen(words[i]);
			positions[i] = i;

			i++;
		}

		if(i != words_stored) {cout << "NOT EXPECTED SIZE ERROR\n"; cin.get();}

		clear();

		
		
		
		

		
		
		

		
		
			sort_positions_based_on_hashes_and_then_words(hashes, words, positions, items_no);
	

	
			sort_based_on_pos(first_positions, positions, items_no);
			sort_based_on_pos(times_found, positions, items_no);
			sort_based_on_pos(word_length, positions, items_no);

			delete []positions;
		

		delete []hashes;
	}
};

class relationship_maker
{
private:
	string work_directory;

	class data_holder
	{
	public:
		unsigned int 
			*comment_ids,
			*word_ids,
			id_pairs_no;
			
	} data_hold[2];

	unsigned long long
		buffer_items;

	unsigned int 
		files_created,
		comment_now,
		comments_with_more_than_254_items;

		/

	class single_relationship_part
	{
	private:
		ifstream_continous_buffered file;

		unsigned int 
			current_word_id,
			current_comments_num,
			comments_got;

		bool finished;

		void load_next()
		{
			if(comments_got == current_comments_num)
			{
				if(file.read_bytes(&current_word_id, sizeof(int)) != sizeof(int) || file.read_bytes(&current_comments_num, sizeof(int)) != sizeof(int))
				{
					current_word_id = 0;
					current_comments_num = 0;
					finished = true;
				}

				

				comments_got = 0;
			}
		}

	public:
		single_relationship_part(const string &path) : file(path.c_str()), 
			current_word_id(0), current_comments_num(0), comments_got(0)
		{
			finished = false;
			load_next();
		}

		bool read_next_comment_if_word_same(unsigned int word_id_now, unsigned int &comment)
		{
			if(finished) return false;
			
			if(word_id_now != current_word_id) return false;
			else 
			{
				if(file.read_bytes(&comment, sizeof(int)) != sizeof(int))
				{
					cout << "NOT EPXEXCTED TO BE HERE\n";
					cin.get();
					finished = true;
					return false;
				}
				else
				{
					comments_got++;
					load_next();
					return true;
				}
			}
		}
	};

	

	void write_data_func()
	{
			

			

			sort_arrays.sort(data_hold[data_second].word_ids, data_hold[data_second].comment_ids, data_hold[data_second].id_pairs_no, false);

			
	
	
			
			ofstream_with_buffer out_buff((work_directory+"\\"+DEF_relationship_buffer_directory+"\\index___0___relationship_buffer_"+to_string((files_created-1))).c_str(), 1024*1024*4);
			
			

			
			unsigned long long i = 0, word_id_pos = 0, comment_id_pos = 0;
			
			

			while(word_id_pos < data_hold[data_second].id_pairs_no)
			{
				

				unsigned int 
					word_id_now = data_hold[data_second].word_ids[word_id_pos++],
					comment_ids_count = 1;

				to_write_buffer[i++] = word_id_now;

				while(word_id_now == data_hold[data_second].word_ids[word_id_pos])
				{
					comment_ids_count++;
					word_id_pos++;

					if(word_id_pos == data_hold[data_second].id_pairs_no) break;
				}
			
				
				

				

				to_write_buffer[i++] = comment_ids_count;
			
				for(unsigned int j = 0; j < comment_ids_count; j++)
					to_write_buffer[j+i] = data_hold[data_second].comment_ids[comment_id_pos++];

				sort_arrays.sort(&to_write_buffer[i], comment_ids_count, false);

				i += comment_ids_count;

				out_buff.write(to_write_buffer, sizeof(int)*i);
				i = 0;

				if(word_id_pos != comment_id_pos) cout << "POS DOESN'T MATCH\n";
			}

		

		
		
		

		

			out_buff.flush_and_close();

	
			thread_finished = true;
			
	}
	
	void write_data(bool start_bg_thread = true)
	{
		while(!thread_finished) boost::this_thread::sleep(boost::posix_time::milliseconds(1));

		if(data_hold[data_main].id_pairs_no > 0)
		{
			files_created++;
			flip_switches();

			thread_finished = false;

			if(!start_bg_thread) write_data_func();
			else
			{
				boost::thread(&relationship_maker::write_data_func, this);
			}
		}

		
	}

public:
	relationship_maker(const string &work_directory, unsigned long long buffer_size = 1024*1024*1024) 
		: work_directory(work_directory), comment_now(0), files_created(0), buffer_items(buffer_size/8), comments_with_more_than_254_items(0),
		thread_finished(true), data_main(0), data_second(1)
	{
		

		for(unsigned int k = 0; k < 2; k++)
		{
			
			data_hold[k].id_pairs_no = 0;
			data_hold[k].word_ids = new unsigned int[buffer_items];
			data_hold[k].comment_ids = new unsigned int[buffer_items];
		}

		to_write_buffer = new unsigned int[2+buffer_items];

		_mkdir((work_directory+"\\"+DEF_relationship_buffer_directory).c_str());
	}

	void add_words(word_stoarge_index_mode &storage)
	{
		unsigned int items = storage.size();

		if(items > 0)
		{
			if(items > buffer_items) {cout << "UNEXPECTED DATA BUFFER CAN'T HOLD EVER A COMMENT!\n"; cin.get();}
			if(data_hold[data_main].id_pairs_no+items > buffer_items) write_data();

			storage.get_first_positions(&data_hold[data_main].word_ids[data_hold[data_main].id_pairs_no]);
			for(unsigned int i = 0; i < items; i++) data_hold[data_main].comment_ids[data_hold[data_main].id_pairs_no++] = comment_now;
		}

		
		

		comment_now++;

		if(items > 254)
			comments_with_more_than_254_items++;

		

		
	}

	
	
		
	

	void finalize(unsigned int *word_ranks, unsigned int *first_time_positions, unsigned int max_word_rank, unsigned int items_no)
	{
		
		write_data(false);

		delete []to_write_buffer;
		for(unsigned int k = 0; k < 2; k++)
		{
			delete []data_hold[k].comment_ids;
			delete []data_hold[k].word_ids;
		}

		unsigned int *final_positions = new unsigned int[items_no];
		for(unsigned int i = 0; i < items_no; i++) final_positions[first_time_positions[i]] = i;

		ofstream_with_buffer 
			actual_relationships((work_directory+"index___0___actual_relationships").c_str()),
			actual_relationships_start_pos((work_directory+"index___0___actual_relationships_start_pos").c_str()),
			total_word_rank_in_sentence((work_directory+"index___0___total_word_rank_in_sentence").c_str());

		single_relationship_part **files_array = new single_relationship_part*[files_created];
		for(int i = 0; i < files_created; i++) files_array[i] = new single_relationship_part((work_directory+"\\"+DEF_relationship_buffer_directory+"\\index___0___relationship_buffer_"+to_string(i)).c_str());

		unsigned int *values = new unsigned int[files_created];
		bool *values_valid = new bool[files_created]();

		unsigned long long total_pos = 0;

		var_byte_array<unsigned int, unsigned short> current_total_word_rank;
		var_byte_array<unsigned short, unsigned char> number_of_items;

		
		current_total_word_rank.max_init(comments_with_more_than_254_items, comment_now);
		number_of_items.max_init(comments_with_more_than_254_items, comment_now);

		

		for(unsigned int i = 0; i < items_no; i++)
		{
			actual_relationships_start_pos.write(&total_pos, sizeof(long long));

			
	
	
	
	
	

			while(true)
			{
				bool at_least_one_valid = false;

				unsigned int 
					lowest = 0,
					lowest_pos = 0;

				for(unsigned int j = 0; j < files_created; j++)
				{
					if(!values_valid[j]) values_valid[j] = files_array[j]->read_next_comment_if_word_same(i, values[j]);

					if(values_valid[j])
					{
						
						if(!at_least_one_valid)
						{
							at_least_one_valid = true;
							lowest = values[j];
							lowest_pos = j;
						}
						else if(values[j] < lowest)
						{
							lowest = values[j];
							lowest_pos = j;
						}
					}
				}

				if(!at_least_one_valid) break;

				
				values_valid[lowest_pos] = false;
		
		

				
				actual_relationships.write(&lowest, sizeof(int));
				
				
				
				
				
				
				
				
				
				
				
				
				unsigned int value_to_add = (unsigned long long)WORD_RANK_MAX*word_ranks[final_positions[i]]/max_word_rank;

				
				current_total_word_rank.add(lowest, value_to_add);
				number_of_items.add(lowest);
				total_pos += sizeof(int);

				
			}

		
		
		
		
		

		

			
			
		
				
			
		}
		
		number_of_items.finish_adding();
		current_total_word_rank.finish_adding();

		for(unsigned int i = 0; i < comment_now; i++)
		{
			AVG_WORD_RANK_TYPE ptr = 0;

			unsigned int 
				items_num_ = number_of_items[i],
				total_word_rank_ = current_total_word_rank[i];

			if(items_num_ > 0)
			{
				ptr = total_word_rank_/items_num_;
				if(ptr == 0) ptr++;
			}

			

			total_word_rank_in_sentence.write(&ptr, sizeof(AVG_WORD_RANK_TYPE));
		}
		
		actual_relationships.flush_and_close();
		actual_relationships_start_pos.flush_and_close();
		total_word_rank_in_sentence.flush_and_close();

		for(int i = 0; i < files_created; i++)
			delete files_array[i];

		delete []files_array;
		delete []values_valid;
		delete []values;

		for(int i = 0; i < files_created; i++)
			if(remove((work_directory+"\\"+DEF_relationship_buffer_directory+"\\index___0___relationship_buffer_"+to_string(i)).c_str()) != 0) cout << "REMOVE ERROR!\n";

		if(_rmdir((work_directory+"\\"+DEF_relationship_buffer_directory).c_str()) != 0) cout << "REMOVE ERROR!\n";

		delete []final_positions;
		
	}
};

void extract_words(string &input_in, char *comment_buffer, word_stoarge_index_mode &storage)
{
	int input_size = input_in.size();

	
	char *input = fast_read_json_body((char*)input_in.c_str());

	
	
	
	

	
	

	int 
		i = 0,
		res_pos = 0;

	while(input[i] != 0)
	{
		if(input[i] >= 'A' && input[i] <= 'Z') input[i] += ('a'-'A');

		if(res_pos == 8092*10-1 || input[i] < 'a' || input[i] > 'z')
		{
			comment_buffer[res_pos++] = 0;
			if(res_pos > 1) storage.add_and_get_id(comment_buffer);
			res_pos = 0;

			i++;
			if(input[i] >= 'A' && input[i] <= 'Z') input[i] += ('a'-'A');

			while(input[i] != 0 && (input[i] < 'a' || input[i] > 'z'))
			{
				i++;
				if(input[i] >= 'A' && input[i] <= 'Z') input[i] += ('a'-'A');
			}

			if(input[i] == 0) break;
		}

		comment_buffer[res_pos++] = input[i++];
	}

	delete []input;

	if(res_pos != 0)
	{
		comment_buffer[res_pos < 8092*10-1 ? res_pos++ : (--res_pos)++] = 0;
		if(res_pos > 1) storage.add_and_get_id(comment_buffer);
	}
}

class threaded_bg_line_reader
{
private:
	unsigned int 
		input_file_buffer,
		items_to_buffer,
		*comments_found,
		comments_we_have_used;

	string **comments_list;
	
	char data_switch, read_switch;

	bool *is_last_piece;
	bool user_waiting;
	bool next_ready;

	bool thread_alive;

	ifstream_continous_buffered *input_data_file;

	void flip_switches()
	{
		comments_we_have_used = 0;
		swap(data_switch, read_switch);
	}

	void fill_data()
	{
		comments_found[data_switch] = 0;

		for(unsigned int i = 0; i < items_to_buffer; i++)
		{
			if(input_data_file->read_a_line(comments_list[data_switch][comments_found[data_switch]]))
				comments_found[data_switch]++;
			else
			{
				is_last_piece[data_switch] = true;
				break;
			}
		}
	}

	void worker()
	{
		while(!is_last_piece[read_switch])
		{
			fill_data();

			while(!user_waiting) boost::this_thread::sleep(boost::posix_time::milliseconds(1));

			flip_switches();
			user_waiting = false;
			next_ready = true;
		}

		thread_alive = false;
	}

public:
	threaded_bg_line_reader(unsigned int input_file_buffer = 4*1024*1024, unsigned int items_to_buffer = 50000) : input_file_buffer(input_file_buffer), items_to_buffer(items_to_buffer)
	{
		comments_found = new unsigned int[2]();

		data_switch = 0;
		read_switch = 1;
		comments_we_have_used = 0;
		next_ready = false;
		user_waiting = false;

		is_last_piece = new bool[2]();
		comments_list = new string*[2];
		for(unsigned int i = 0; i < 2; i++) comments_list[i] = new string[items_to_buffer];
		
		main_data_file.init();
		input_data_file = new ifstream_continous_buffered(main_data_file, input_file_buffer);
		
		fill_data();
		flip_switches();

		boost::thread(&threaded_bg_line_reader::worker, this);
		thread_alive = true;
	}

	~threaded_bg_line_reader()
	{
		while(thread_alive) boost::this_thread::sleep(boost::posix_time::milliseconds(1));

		for(unsigned int i = 0; i < 2; i++) delete []comments_list[i];
		delete []comments_list;

		delete []comments_found;
		delete []is_last_piece;
		
		

		delete input_data_file;
		main_data_file.kill();
	}

	void read_a_block(string *&block_ptr, unsigned int &block_items_num, bool &last_block)
	{
		if(is_last_piece[read_switch] && comments_found[read_switch] == comments_we_have_used)
		{
			block_ptr = NULL;
			block_items_num = 0;
			last_block = true;
			return;
		}
		else if(comments_found[read_switch] == comments_we_have_used)
		{
			
	
			user_waiting = true;
			while(!next_ready) boost::this_thread::sleep(boost::posix_time::milliseconds(1));
			next_ready = false;

			

	

		}

		if(is_last_piece[read_switch] && comments_found[read_switch] == comments_we_have_used)
		{
			block_ptr = NULL;
			block_items_num = 0;
			last_block = true;
			return;
		}
		else
		{
			block_ptr = comments_list[read_switch];
			block_items_num = comments_found[read_switch];
			last_block = is_last_piece[read_switch];

			comments_we_have_used = comments_found[read_switch];
			return;
		}
	}

	
	bool read_next_line(string *&out_str_ptr)
	{
		if(is_last_piece[read_switch] && comments_found[read_switch] == comments_we_have_used)
		{
			out_str_ptr = NULL;
			return false;
		}
		else if(comments_found[read_switch] == comments_we_have_used)
		{
			user_waiting = true;
			while(!next_ready) boost::this_thread::sleep(boost::posix_time::milliseconds(1));
			next_ready = false;
		}

		if(is_last_piece[read_switch] && comments_found[read_switch] == comments_we_have_used)
		{
			out_str_ptr = NULL;
			return false;
		}
		else
		{
			out_str_ptr = &comments_list[read_switch][comments_we_have_used++];
			return true;
		}
	}
};

class threaded_data_reader
{
private:

	class data_holder
	{
	public:
		unsigned int *str_comments_list_sizes;
		word_stoarge_index_mode **storage;

		bool 
			*thread_ready,
			file_finished;

		unsigned int 
			comments_now_found,
			comment_sending;

	} data_hold[2];

	char** comment_buffer;
	string *str_comments_list;

	unsigned int buffer_items_no, threads_num, bg_, user_;
	threaded_bg_line_reader data_reader;

	bool 
		read_more_complete,
		kill_all_threads,
		threads_started,
		user_waiting_for_new_data;

	void do_work(unsigned int number)
	{
		for(unsigned int i = number; i < data_hold[bg_].comments_now_found; i += threads_num)
		{
			
			
			data_hold[bg_].str_comments_list_sizes[i] = str_comments_list[i].size();
			extract_words(str_comments_list[i], comment_buffer[number], *data_hold[bg_].storage[i]);
		}
		
		data_hold[bg_].thread_ready[number] = true;
	}

	void wait_for_all_threads()
	{
		while(true)
		{
			bool exit = true;

			for(unsigned int i = 0; i < threads_num; i++)
				if(!data_hold[bg_].thread_ready[i])
					exit = false;

			if(exit) break;

			boost::this_thread::sleep(boost::posix_time::milliseconds(1));
		}
	}

	void worker_thread(unsigned int number)
	{
		while(!threads_started)
			boost::this_thread::sleep(boost::posix_time::milliseconds(1));
		
		while(true)
		{
			do_work(number);
			
			while(data_hold[bg_].thread_ready[number])
			{
				if(kill_all_threads)
					return;
				else
					boost::this_thread::sleep(boost::posix_time::milliseconds(1));
			}
		}
	}

	void worker_thread_boss(unsigned int number)
	{
		while(true)
		{
			data_hold[bg_].comments_now_found = 0;
			data_hold[bg_].comment_sending = 0;
			data_hold[bg_].file_finished = false;
			
			data_reader.read_a_block(str_comments_list, data_hold[bg_].comments_now_found, data_hold[bg_].file_finished);
			if(data_hold[bg_].comments_now_found > buffer_items_no) { cout << "ERROR! THE ITEMS_NO BUFFER OF threaded_bg_line_reader NEEDS TO BE <= TO THE ITEMS_NO BUFFER OF index_creator\n"; cin.get(); }
			
			for(unsigned int i = 0; i < threads_num; i++)
				data_hold[bg_].thread_ready[i] = false;
			
			if(!threads_started) threads_started = true;
			do_work(number);
			
			wait_for_all_threads();
			
			while(!user_waiting_for_new_data)
				boost::this_thread::sleep(boost::posix_time::milliseconds(1));
			
			bool last_read = data_hold[bg_].file_finished;

			if(last_read) kill_all_threads = true;
			
			swap(bg_, user_);
			user_waiting_for_new_data = false;
			read_more_complete = true;
			
			while(read_more_complete)
				boost::this_thread::sleep(boost::posix_time::milliseconds(1));

			if(last_read) break;
		}
	}
	
	void start_next()
	{
		user_waiting_for_new_data = true;
		
		while(!read_more_complete)
			boost::this_thread::sleep(boost::posix_time::milliseconds(1));
		
		read_more_complete = false;
	}
	
public:
	threaded_data_reader(unsigned int buffer_items_no = 50000) : buffer_items_no(buffer_items_no), threads_num(THREADS_TO_USE-1 > 0 ? THREADS_TO_USE-1 : 1), 
		bg_(0), user_(1), kill_all_threads(false), read_more_complete(false), threads_started(false), user_waiting_for_new_data(false)
	{
		comment_buffer = new char*[threads_num];
		for(unsigned int i = 0; i < threads_num; i++) comment_buffer[i] = new char[8092*10];
		
		for(unsigned int k = 0; k < 2; k++)
		{
			data_hold[k].storage = new word_stoarge_index_mode*[buffer_items_no];
			data_hold[k].thread_ready = new bool[threads_num]();
			data_hold[k].str_comments_list_sizes = new unsigned int[buffer_items_no];

			for(unsigned int i = 0; i < buffer_items_no; i++) data_hold[k].storage[i] = new word_stoarge_index_mode;
		}
		
		for(unsigned int i = 0; i < threads_num-1; i++)
			boost::thread(&threaded_data_reader::worker_thread, this, i);
		
		boost::thread(&threaded_data_reader::worker_thread_boss, this, threads_num-1);
		
		start_next();
	}
	
	~threaded_data_reader()
	{
		for(unsigned int i = 0; i < threads_num; i++) delete []comment_buffer[i];
		delete comment_buffer;

		for(unsigned int k = 0; k < 2; k++)
		{
			for(unsigned int i = 0; i < buffer_items_no; i++) delete data_hold[k].storage[i];

			delete []data_hold[k].str_comments_list_sizes;
			delete data_hold[k].storage;
			delete []data_hold[k].thread_ready;
		}
	}

	bool get_next(word_stoarge_index_mode *&out_storage, unsigned int &str_size)
	{
		if(data_hold[user_].comments_now_found == data_hold[user_].comment_sending)
		{
			if(!data_hold[user_].file_finished)
			{
				start_next();
				return get_next(out_storage, str_size);
			}
			else
				return false;
		}

		out_storage = data_hold[user_].storage[data_hold[user_].comment_sending];
		str_size = data_hold[user_].str_comments_list_sizes[data_hold[user_].comment_sending];
		data_hold[user_].comment_sending++;
		return true;
	}
};

class index_creator
{
private:

	void generate_word_ranks(unsigned int *times_found_in, unsigned int items_no, unsigned int *&word_ranks, unsigned int &max_word_rank)
	{
		unsigned int 
			*times_found = new unsigned int[items_no],
			*sort_positions = new unsigned int[items_no];

		memcpy(times_found, times_found_in, sizeof(int)*items_no);
		for(unsigned int i = 0; i < items_no; i++) sort_positions[i] = i;

		sort_arrays.sort(times_found, sort_positions, items_no);

		word_ranks = new unsigned int[items_no];

		unsigned int prev_times = times_found[0], rank_now = 1;

		for(unsigned int i = 0; i < items_no; i++)
		{
			if(times_found[i] < prev_times)
			{
				prev_times = times_found[i];
				rank_now++;
			}

			word_ranks[i] = rank_now;
		}

		max_word_rank = rank_now;

		sort_arrays.sort(sort_positions, word_ranks, items_no, false);

		delete []times_found;
		delete []sort_positions;
	}

	bool used;

public:
	index_creator() : used(false) { }

	void run()
	{
		std::clock_t time_now = clock();
		used = true;
		string work_directory = base_work_directory+data_file_merged_directory;
		_mkdir((work_directory).c_str());
		work_directory += "\\";

		main_data_file.init();
		ifstream_continous_buffered input_data_file(main_data_file);
		
		string current_comment;
		char* comment_buffer = new char[8092*10];

		word_stoarge_index_mode 
			total_storage,
			*temp_storage;

		ofstream_with_buffer 
			sentence_start_positions(string(work_directory+"index___0___sentace_start_postions").c_str()),
			unique_words_in_sentence(string(work_directory+"index___0___unique_words_in_sentence").c_str()),
			added_strings_signle(string(work_directory+"index___0___added_strings_signle").c_str());

		relationship_maker relationships(work_directory);

		unsigned long long char_counter = 0;
		sentence_start_positions.write(&char_counter, sizeof(unsigned long long));

		
		unsigned int comment_size;
		threaded_data_reader data_reader;

		while(data_reader.get_next(temp_storage, comment_size))
		
		{
			char_counter += comment_size+1;
			
			

			
			sentence_start_positions.write(&char_counter, sizeof(unsigned long long));

			if(temp_storage->size() > USHRT_MAX) {cout << "NOT EXPECTED ERROR WITH SHORT SIZE\n"; cin.get();}
			unsigned short unique_words_found = temp_storage->size();
			unique_words_in_sentence.write(&unique_words_found, sizeof(short));
			
			total_storage.add_every_word_once_and_update_first_pos(*temp_storage);

			relationships.add_words(*temp_storage);
			
			temp_storage->clear();
			

		}
		
		cout << ( std::clock() - time_now ) / (double) CLOCKS_PER_SEC << "\n";

		sentence_start_positions.flush_and_close();
		unique_words_in_sentence.flush_and_close();

		char **words;

		unsigned int 
			*first_time_positions,
			*times_found,
			*word_length,
			items_no,
			
			*word_ranks,
			max_word_rank;

		total_storage.get_all_and_clear(words, first_time_positions, times_found, word_length, items_no);

		create_file_and_fill(string(work_directory+"index___0___times_found").c_str(), times_found, items_no*sizeof(int));

		for(unsigned int i = 0; i < items_no; i++) added_strings_signle.write(words[i], word_length[i]+1);
		added_strings_signle.flush_and_close();

		generate_word_ranks(times_found, items_no, word_ranks, max_word_rank);

		create_file_and_fill(string(work_directory+"index___0___word_ranks").c_str(), word_ranks, items_no*sizeof(int));
		create_file_and_fill(string(work_directory+"index___0___max_word_rank").c_str(), &max_word_rank, sizeof(int));
		create_file_and_fill(string(work_directory+"index___0___subreddit_ids").c_str(), NULL, 0);
		create_file_and_fill(string(work_directory+"index___0___actual_relationships_pos_converter"), first_time_positions, sizeof(int)*items_no);

		relationships.finalize(word_ranks, first_time_positions, max_word_rank, items_no);

		for(unsigned int i = 0; i < items_no; i++) delete []words[i];
		delete []words;
		delete []first_time_positions;
		delete []times_found;
		delete []word_length;
		delete []word_ranks;
		delete []comment_buffer;

	}

	~index_creator()
	{
		if(used) main_data_file.kill();
	}
};

class compare_old_with_new_format
{
	string new_dir, old_dir;

	void compare_files(string filename)
	{
		string filename_to_show = filename;
		while(filename_to_show.size() < 50) filename_to_show += " ";

		cout << "COMPARING\t\t" << filename_to_show;

		ifstream_continous_buffered 
			file_a((new_dir+"\\"+filename).c_str()),
			file_b((old_dir+"\\"+filename).c_str());

		while(true)
		{
			unsigned char 
				cmp_a = 0,
				cmp_b = 0;

			unsigned long long 
				a_bytes = file_a.read_bytes(&cmp_a, 1),
				b_bytes = file_b.read_bytes(&cmp_b, 1);
			
			bool break_now = true;

			if(a_bytes == 0 && b_bytes == 0) cout << "OKAY\n";
			else if(a_bytes == 0 || b_bytes == 0) cout << "FAIL not the same size\n";
			else if(cmp_a != cmp_b) cout << "FAIL different values\n";
			else break_now = false;

			if(break_now) break;
		}
	}

	template <typename T>
	bool compare_arrays(T *a, T *b, unsigned long long size, unsigned int *first_to_second_pos_conveter)
	{
		for(unsigned long long i = 0; i < size; i++)
		{
			if(b[i] != a[first_to_second_pos_conveter[i]])
			{
				cout << "a[i] " << a[i] << " b[i] " << b[i] << " pos_conv[i] " << first_to_second_pos_conveter[i] << " a[pos_conv[i]] " << a[first_to_second_pos_conveter[i]] << " b[pos_conv[i]] " << b[first_to_second_pos_conveter[i]] << "\n";

				
		
				return false;
			}
		}

		return true;
	}
	
	

	void read_strings_from_file(char* data, unsigned int file_size, int items_no, char **&added_strings)
	{
		added_strings = (char**)malloc(sizeof(char*)*items_no);

		int index_count = 0, char_pos = 0, chars_now = 0;
		char *temp_buff = (char*)malloc(8092*10);

		while(char_pos < file_size)
		{
			if(chars_now == 8092*10-1)
				data[char_pos] = 0;

			if(data[char_pos] == 0)
			{
				
				temp_buff[chars_now] = 0;
				added_strings[index_count] = (char*)malloc(chars_now+1);
				
				strcpy(added_strings[index_count], temp_buff);
				
				

				index_count++;
				chars_now = 0;
			}
			else
			{
				temp_buff[chars_now] = data[char_pos];
				chars_now++;
			}

			char_pos++;
		}

		
		
		
		
		

		free(temp_buff);
	}
	
	void read_file(string name, void* result, int size)
	{
		
		
			

			ifstream file(name, ios::in | ios::binary);

			if(!file.is_open())
			{
				cout << "INPUT FILE DOESN'T EXIST " << name << "\n";
				cin.get();
			}

			file.read((char*)result, size);
			file.close();
		
		
		
	}

	int binary_search_added_strings_pos(const string &input, string *in_array, unsigned int size)
	{
		int low = 0, high = size-1, mid;

		while(low <= high)
		{
			mid = (low + high)/2;

			if(strcmp(in_array[mid].c_str(), input.c_str()) == 0)
				return mid;
			else if(strcmp(in_array[mid].c_str(), input.c_str()) == -1)
				low = mid+1; 
			else
				high = mid-1;
		}
		return -1;
	}
	
	void get_converter_and_cmp_strings(unsigned int *&pos_conveter_from_first_to_second)
	{
		unsigned int size = get_file_size(new_dir+"\\index___0___added_strings_signle");

		if(size != get_file_size(old_dir+"\\index___0___added_strings_signle")) cout << "index___0___added_strings_signle NOT THE SAME, sizes donesn't match\n";

		char
			*first = new char[size],
			*second = new char[size];

		read_file(new_dir+"\\index___0___added_strings_signle", first, size);
		read_file(old_dir+"\\index___0___added_strings_signle", second, size);

		pos_conveter_from_first_to_second = new unsigned int[size];

		char 
			**first_str,
			**second_str;

		unsigned int words_number_a = get_file_size(new_dir+"\\index___0___times_found");
		if(get_file_size(old_dir+"\\index___0___times_found") != words_number_a) cout << "index___0___times_found NOT THE SAME, sizes donesn't match\n";
		if(words_number_a % 4 != 0) cout << "TIMES FOUND SIZES NOT EQUAL OR NOT % 4 == 0\n";
		words_number_a /= 4;

		read_strings_from_file((char*)first, size, words_number_a, first_str);
		read_strings_from_file((char*)second, size, words_number_a, second_str);

		string 
			*first_str_s = new string[words_number_a],
			*second_str_s = new string[words_number_a];

		for(unsigned int i = 0; i < words_number_a; i++)
		{
			first_str_s[i] = string(first_str[i]);
			second_str_s[i] = string(second_str[i]);
		}

		unordered_map<string, unsigned int> map_x;

		for(unsigned int i = 0; i < words_number_a; i++)
			map_x[second_str_s[i]] = i;

		for(unsigned int i = 0; i < words_number_a; i++)
		{
			auto search = map_x.find(first_str_s[i]);
			if(search != map_x.end())
				pos_conveter_from_first_to_second[i] = search->second;
			else
				cout << "index___0___added_strings_signle NOT FOUND ERROR\n";
		}

		for(unsigned int i = 0; i < words_number_a; i++)
		{
			if(strcmp(first_str[pos_conveter_from_first_to_second[i]], second_str[i]) != 0)
				cout << "NOT SAME\n";
		}

		string filename = "index___0___added_strings_signle";

		string filename_to_show = filename;
		while(filename_to_show.size() < 50) filename_to_show += " ";

		cout << "COMPARING\t\t" << filename_to_show << "OKAY\n";
	}

	unsigned long long get_filezies_if_sizes_same(string file_a, string file_b)
	{
		bool r = get_file_size(file_a) == get_file_size(file_b);
		if(get_file_size(file_a) == 0) r = false;
		if(!r) {cout << "FILESIZES NOT SAME |" << file_a << "|" << file_b << "|\n"; return 0;}
		else return get_file_size(file_a);
	}

	bool mem_equal(unsigned int* mem_a, unsigned int* mem_b, unsigned long long mem_a_size, unsigned long long mem_b_size, unsigned int i)
	{
		
		if(mem_a_size != mem_b_size) {
			
			

			
			

			cout << i << " not same | size " << mem_a_size << " | size " << mem_b_size << "\n";
			return false;
		}
		
		for(unsigned long long i = 0; i < mem_a_size; i++)
		{
			if(mem_a[i] != mem_b[i])
			{
				cout << mem_a[i] << " " << mem_b[i] << "\n";
				
				return false;
			}

			mem_a[i] = UINT_MAX;
			mem_b[i] = UINT_MAX;
		}

		
		
		return true;
	}

	bool compare_actual_relationships()
	{
		
		
		unsigned long long
			actual_relationships_items = get_filezies_if_sizes_same(old_dir+"\\index___0___actual_relationships", new_dir+"\\index___0___actual_relationships")/sizeof(int),
			actual_relationships_pos_converter_items = get_filezies_if_sizes_same(old_dir+"\\index___0___actual_relationships_pos_converter", new_dir+"\\index___0___actual_relationships_pos_converter")/sizeof(int),
			actual_relationships_start_pos_items = get_filezies_if_sizes_same(old_dir+"\\index___0___actual_relationships_start_pos", new_dir+"\\index___0___actual_relationships_start_pos")/sizeof(long long),
			times_found_items = get_filezies_if_sizes_same(old_dir+"\\index___0___times_found", new_dir+"\\index___0___times_found")/sizeof(int);

		unsigned int
			*actual_relationships_old = new unsigned int[actual_relationships_items],
			*actual_relationships_new = new unsigned int[actual_relationships_items],

			*actual_relationships_pos_converter_old = new unsigned int[actual_relationships_pos_converter_items],
			*actual_relationships_pos_converter_new = new unsigned int[actual_relationships_pos_converter_items],

			*times_found_old = new unsigned int[times_found_items],
			*times_found_new = new unsigned int[times_found_items];

		unsigned long long
			*actual_relationships_start_pos_old = new unsigned long long[actual_relationships_start_pos_items],
			*actual_relationships_start_pos_new = new unsigned long long[actual_relationships_start_pos_items];

		read_file(old_dir+"\\index___0___actual_relationships", actual_relationships_old, actual_relationships_items*sizeof(int));
		read_file(new_dir+"\\index___0___actual_relationships", actual_relationships_new, actual_relationships_items*sizeof(int));

		read_file(old_dir+"\\index___0___actual_relationships_pos_converter", actual_relationships_pos_converter_old, actual_relationships_pos_converter_items*sizeof(int));
		read_file(new_dir+"\\index___0___actual_relationships_pos_converter", actual_relationships_pos_converter_new, actual_relationships_pos_converter_items*sizeof(int));
		
		read_file(old_dir+"\\index___0___times_found", times_found_old, times_found_items*sizeof(int));
		read_file(new_dir+"\\index___0___times_found", times_found_new, times_found_items*sizeof(int));
		
		read_file(old_dir+"\\index___0___actual_relationships_start_pos", actual_relationships_start_pos_old, actual_relationships_start_pos_items*sizeof(long long));
		read_file(new_dir+"\\index___0___actual_relationships_start_pos", actual_relationships_start_pos_new, actual_relationships_start_pos_items*sizeof(long long));

		unsigned int same = 0, not_same = 0;

		
		
		
		for(unsigned int i = 0; i < times_found_items; i++)
			if(times_found_old[i] != times_found_new[i])
			{
				cout << i << " NOT SAME " << times_found_old[i] << " " << times_found_new[i] << "\n\n";
			}

		unsigned long long bytes_compared = 0;

		
		
		
		

		for(unsigned int i = 0; i < actual_relationships_start_pos_items; i++)
		{
			
			
			
			
			
			

			
			

			
			
			
			
			

			unsigned long long 
				start_pos_old = actual_relationships_start_pos_old[actual_relationships_pos_converter_old[i]]/sizeof(int),
				start_pos_new = actual_relationships_start_pos_new[actual_relationships_pos_converter_new[i]]/sizeof(int),

				size_old = times_found_old[i],
				size_new = times_found_new[i];

			

			if((size_old == 0 || size_new == 0) && size_old != size_new)
			{
				cout << "|";
				cout << size_old << " " << size_new << " - ";
				
				cout << times_found_items << "\n";
				not_same++;
			}
			
			
			
			if(!mem_equal(&actual_relationships_old[start_pos_old], &actual_relationships_new[start_pos_new], size_old, size_new, i)) not_same++;
			else same++;
			bytes_compared += size_new*sizeof(int);

			
			

			

			
			

		}

		cout << "\nbytes " << bytes_compared << " same " << same << " not same " << not_same << "\n";

		
		
		

		for(unsigned long long i = 0; i < actual_relationships_items; i++)
			if(actual_relationships_old[i] != UINT_MAX || actual_relationships_new[i] != UINT_MAX)
				cout << i << " USED THE SAME ITEM TWICE LOL " << actual_relationships_old[i] << " | " << actual_relationships_new[i] << "\n";

		cout << "same " << same << " not same " << not_same << "\n";

		return true;
	}

public:
	compare_old_with_new_format(string old_dir, string new_dir) : old_dir(old_dir), new_dir(new_dir)
	{
		
		

		compare_files("index___0___added_strings_signle");
		compare_files("index___0___max_word_rank");
		compare_files("index___0___sentace_start_postions");
		compare_files("index___0___subreddit_ids");
		compare_files("index___0___times_found");
		compare_files("index___0___total_word_rank_in_sentence");
		compare_files("index___0___unique_words_in_sentence");
		compare_files("index___0___word_ranks");

		cout << "COMPARING ( index___0___actual_relationships | index___0___actual_relationships_pos_converter | index___0___actual_relationships_start_pos )\n";
		compare_actual_relationships();
		
		return;
		compare_files("index___0___sentace_start_postions");
		compare_files("index___0___max_word_rank");
		compare_files("index___0___subreddit_ids");
		compare_files("index___0___unique_words_in_sentence");

		unsigned int *pos_conveter_from_first_to_second;
		get_converter_and_cmp_strings(pos_conveter_from_first_to_second);

		if(true)
		{
			unsigned int size_a = get_file_size(new_dir+"\\index___0___word_ranks");
			if(size_a !=  get_file_size(old_dir+"\\index___0___word_ranks")) cout << "ERROR IN index___0___word_ranks SIZES\n";
			else
			{
				size_a /= 4;
				unsigned int *dt_a = new unsigned int[size_a], *dt_b = new unsigned int[size_a];

				read_file(new_dir+"\\index___0___word_ranks", dt_a, size_a*sizeof(int));
				read_file(old_dir+"\\index___0___word_ranks", dt_b, size_a*sizeof(int));

				string filename_to_show = "index___0___word_ranks";
				while(filename_to_show.size() < 50) filename_to_show += " ";

				if(compare_arrays(dt_a, dt_b, size_a, pos_conveter_from_first_to_second))
					cout << "COMPARING\t\t" << filename_to_show << "OKAY\n";
				else
					cout << "COMPARING\t\t" << filename_to_show << "FAIL\n";
			}
		}
		
		if(true)
		{
			unsigned int size_a = get_file_size(new_dir+"\\index___0___times_found");
			if(size_a !=  get_file_size(old_dir+"\\index___0___times_found")) cout << "ERROR IN index___0___times_found SIZES\n";
			else
			{
				size_a /= 4;
				unsigned int *dt_a = new unsigned int[size_a], *dt_b = new unsigned int[size_a];

				read_file(new_dir+"\\index___0___times_found", dt_a, size_a*sizeof(int));
				read_file(old_dir+"\\index___0___times_found", dt_b, size_a*sizeof(int));

				string filename_to_show = "index___0___times_found";
				while(filename_to_show.size() < 50) filename_to_show += " ";

				if(compare_arrays(dt_a, dt_b, size_a, pos_conveter_from_first_to_second))
					cout << "COMPARING\t\t" << filename_to_show << "OKAY\n";
				else
					cout << "COMPARING\t\t" << filename_to_show << "FAIL\n";
			}
		}

		cout << "\nDONE\n";
	}
};

class job_runner
{
public:
	

	

	
	
	
	

	

	
	void write_block(const char* file, void* block, int block_size, bool open_in_app_mode = false)
	{
		ofstream write_ptr(file, open_in_app_mode ? (ios::out | ios::app | ios::binary) : (ios::out | ios::trunc | ios::binary));
		write_ptr.write((char*)block, block_size);
		write_ptr.close();

		
		
		
		
	}

	void create_strings_array(string &base)
	{
		int pos_x = 0;
		long hard_limit = 809200000L;

		char *added_strings_signle = (char*)malloc(hard_limit);
	
		for(int i = 0; i < pos_now; i++)
		{
			int len = strlen(added_strings[sort_positions[i]]);
		
			strcpy(&added_strings_signle[pos_x], added_strings[sort_positions[i]]);

			free(added_strings[sort_positions[i]]);
			pos_x += len+1;

			if(pos_x >= hard_limit) {cout << "HARD LIMIT, " << pos_now-i << " ITEMS LEFT"; cin.get(); cin.get(); cin.get(); exit(0);}
		}

		

		write_block((base+"added_strings_signle").c_str(), added_strings_signle, pos_x);

		free(added_strings_signle);

	}
	
	void read_strings_from_file(char* data, int data_size)
	{
		added_strings = (char**)malloc(sizeof(char*)*pos_now);

		int index_count = 0, char_pos = 0, chars_now = 0;
		char *temp_buff = (char*)malloc(8092*10);

		while(char_pos < data_size)
		{
			if(chars_now == 8092*10-1)
				data[char_pos] = 0;

			if(data[char_pos] == 0)
			{
				
				temp_buff[chars_now] = 0;
				added_strings[index_count] = (char*)malloc(chars_now+1);
				
				strcpy(added_strings[index_count], temp_buff);
				
				added_hashes_linear[index_count] = hash_djb2(added_strings[index_count]);

				index_count++;
				chars_now = 0;
			}
			else
			{
				temp_buff[chars_now] = data[char_pos];
				chars_now++;
			}

			char_pos++;
		}

		
		
		
		
		

		free(temp_buff);
	}
	
	void read_file(string name, void* result, int size)
	{
		
		
			

			ifstream file(name, ios::in | ios::binary);
			file.read((char*)result, size);
			file.close();
		
		
		
	}

	void do_read_block(string work_directory, string input, int number)
	{
		string base = work_directory+input+"___"+to_string(number)+"___";
		
		string name;
		int size1, size3;
		
		name = base+"times_found";
		size1 = get_file_size(name);
		times_found = (int*)malloc(size1);
		read_file(name, times_found, size1);
		
		
		
		
		added_hashes_linear = (int*)malloc(size1);
		
		
		
		
		
		
		

		pos_now = (int)(size1/sizeof(int));
		
		name = base+"added_strings_signle";
		size3 = get_file_size(name);
		char *buffer = (char*)malloc(size3);
		read_file(name, buffer, size3);
		read_strings_from_file(buffer, size3);
		free(buffer);
	}

	void compress_spaces(char *str)
	{
		char *dst = str;

		for (; *str; ++str) {
			*dst++ = *str;

			if (isspace(*str)) {
				do ++str; 

				while (isspace(*str));

				--str;
			}
		}

		*dst = 0;
	}

	int added_strings_pos(int Hash, const char*Word)
	{
		

		auto search = added_strings_pos_map.find((char*)Word);
		if(search != added_strings_pos_map.end())
			return search->second;

		return -1;
	}

	int get_word_pos_from_hash_pos(int hash_pos, const char* Word)
	{
		int 
			start_pos = hash_pos,
			end_pos = hash_pos;

		while(start_pos >= 0 && added_hashes_linear[start_pos] == added_hashes_linear[hash_pos]) start_pos--;
		start_pos++;

		while(end_pos < binary_search_size_to_use && added_hashes_linear[end_pos] == added_hashes_linear[hash_pos]) end_pos++;
		end_pos--;

		for(int i = start_pos; i <= end_pos; i++)
			if(strcmp(added_strings[i], Word) == 0)
				return i;

		return -1;
	}

	int binary_search_added_strings_pos(int Hash, const char* Word)
	{
		int low = 0, high = binary_search_size_to_use-1, mid;

		while(low <= high)
		{
			mid = (low + high)/2;

			if(added_hashes_linear[mid] == Hash)
			{
				if(strcmp(added_strings[mid], Word) == 0)
					return mid;
				else
					return get_word_pos_from_hash_pos(mid, Word);
			}
			else if(added_hashes_linear[mid] > Hash)
				low = mid+1; 
			else
				high = mid-1;
		}
		return -1;
	}
	
	

	

	void add_to_array(const char *Word, int word_hash, int word_size, int counter_increase = 1, bool use_binary_search = false)
	{
		if(counter_increase < 1) {cout << "ERROR+++: " << counter_increase; cin.get(); cin.get(); cin.get();}

		int Hash = word_hash;
		
		int pos = use_binary_search ? binary_search_added_strings_pos(Hash, Word) : added_strings_pos(Hash, Word);
		

		

		if(pos >= 0)
			times_found[pos]+=counter_increase;
		else
		{
			added_strings[pos_now] = (char*)malloc(word_size+1);
			strcpy(added_strings[pos_now], Word);
			

			added_hashes_linear[pos_now] = Hash;
			times_found[pos_now] = counter_increase;

			if(!use_binary_search)
				added_strings_pos_map[added_strings[pos_now]] = pos_now;

			

			pos_now++;
		}
	}
	
	
	

	

	void read_json(string &json, string &body, string &subreddit_id, string &link_id, string &comment_id)
	{
		Document document;
		document.Parse(json.c_str());

		body = document["body"].GetString();
		subreddit_id = document["subreddit_id"].GetString();
		link_id = document["link_id"].GetString();
		comment_id = document["name"].GetString();
	}

	bool linear_search(char** words_list, int* words_list_hashes, int* words_lenght, int &words_num, char *word, int lenght)
	{
		int word_hash = hash_djb2(word);

		for(int i = 0; i < words_num; i++)
			if(words_list_hashes[i] == word_hash && strcmp(words_list[i], word) == 0)
				
					return true;
		
		
		

		return false;
	}

	void add_to_array_if_not_there(char** words_list, int* words_list_hashes, int* words_lenght, int &words_num, char *word, int lenght)
	{
		if(lenght == 1) return;

		if(linear_search(words_list, words_list_hashes, words_lenght, words_num, word, lenght)) return;

		string new_word;
		if(do_run_word_simplifier(word, lenght, new_word))
		{
			char* or_word = word;

			word = (char*)new_word.c_str();
			lenght = new_word.size()+1;

			if(strcmp(or_word, word) != 0 && linear_search(words_list, words_list_hashes, words_lenght, words_num, word, lenght)) return;
		}

		words_lenght[words_num] = lenght;
		words_list[words_num] = (char*)malloc(lenght);
		words_list_hashes[words_num] = hash_djb2(word);

		strcpy(words_list[words_num], word);

		

		words_num++;

		if(words_num > 40000)
		{
			cout << "sentance having more than 40.000 words error!";
			cin.get();
			cin.get();
			cin.get();
		}
	}

	void write_sentance_link(string &work_directory, string &input)
	{
		
		
		string base = work_directory+input+"___"+to_string(0)+"___";

		
		ofstream file(base+"relationships", ios::out | ios::app | ios::binary);
		
		file.write((char*)relationships, relationships_counter*sizeof(int));
		file.close();

		relationships_counter = 0;
		total_relationships_found++;
	}

	void add_sentance_link(string &work_directory, string &input, int word_pos, unsigned int words_lenght, unsigned int sentance)
	{
		
		
		if(word_pos >= pos_now)
			cout << "ERROR, WORD_POS TOO BIG!!!!!!\n";

		sentances_this_word_is_used[word_pos]++;

		relationships[relationships_counter++] = word_pos;
		relationships[relationships_counter++] = sentance;

		if(relationships_counter == 10000000)
			write_sentance_link(work_directory, input);

		

		
		

		

		
		
			
			
			
			
			
			
		

		
	}

	void read_json_pro(string &work_directory, string &input_file, string input, char *result, char** words_list, int* words_list_hashes, int* words_lenght, unsigned int sentance, bool add_to_array_and_free = true)
	{
		
		
		
		
		
		boost::replace_all(input, "&amp;", "&");
		boost::replace_all(input, "&gt;", ">");
		boost::replace_all(input, "&lt;", "<");
		
		

		
		

		

		

		
		
		

		
		
		
		

		
		

	
	
		int words_num = 0;
		unsigned int total_word_rank_in_comment_buff = 0;

		
		int i = 0;
		int res_pos = 0;

		while(input[i] != 0)
		{
			
			
			

			if(input[i] >= 'A' && input[i] <= 'Z') input[i] += ('a'-'A');

			if(res_pos == 8092*10-1 || input[i] < 'a' || input[i] > 'z')
			{
				result[res_pos++] = 0;
	
				add_to_array_if_not_there(words_list, words_list_hashes, words_lenght, words_num, result, res_pos);
				res_pos = 0;
		

				i++;
				if(input[i] >= 'A' && input[i] <= 'Z') input[i] += ('a'-'A');

				while(input[i] != 0 && (input[i] < 'a' || input[i] > 'z'))
				{
					i++;
					if(input[i] >= 'A' && input[i] <= 'Z') input[i] += ('a'-'A');
				}

				if(input[i] == 0) break;
			}

				result[res_pos++] = input[i++];

		}

		if(res_pos != 0)
		{
			result[res_pos < 8092*10-1 ? res_pos++ : (--res_pos)++] = 0;
	
			add_to_array_if_not_there(words_list, words_list_hashes, words_lenght, words_num, result, res_pos);
			
		}

		
		
			for(int i = 0; i < words_num; i++)
			{
				if(add_to_array_and_free)
				{
					
					add_to_array(words_list[i], words_list_hashes[i], words_lenght[i]);
					free(words_list[i]);
				}
				else
				{
					
					int word_pos = binary_search_added_strings_pos(words_list_hashes[i], words_list[i]);
					if(word_pos >= word_ranks_list_size) {cout << "HUGE ERROR HERE!!!!!!!!!!\n"; cin.get();}

					add_sentance_link(work_directory, input_file, word_pos, words_lenght[i], sentance);
					total_word_rank_in_comment_buff += ((unsigned long long)WORD_RANK_MAX*word_ranks_list[word_pos]/max_word_rank_now);

					free(words_list[i]);
				}
			}
			
			if(words_num > 0)
			{
				total_word_rank_in_comment = total_word_rank_in_comment_buff/words_num;
				if(total_word_rank_in_comment == 0) total_word_rank_in_comment = 1;
			}
			else
				total_word_rank_in_comment = 0;

			unique_words_in_comment = words_num;

			
			

	
	

		

			

		
		
		
		
	}

	int write_blocks(string work_directory, string input, bool create_sentace_start_postions = true)
	{
		
		

		
		

		

		int part_size = 10000;

		

		added_hashes_linear = (int*)malloc(4*1000000);
		added_strings = (char**)malloc(sizeof(char*)*1000000);
		times_found = (int*)malloc(4*1000000);

		char** words_list = (char**)malloc(40000*sizeof(char*));
		int* words_list_hashes = (int*)malloc(40000*sizeof(int));
		int* words_lenght = (int*)malloc(40000*sizeof(int));

		pos_now = 0;

	
		string messageVar;
		messageVar.reserve(1024*1024);
		char* result = (char*)malloc(8092*10);

		int count = 0, number = 0;
		

		
		string output;
		unsigned long long char_counter;
		
		ofstream out_file;
		long long buffer_size;
		unsigned long long *pos_buffer = 0;
		unsigned long long buffer_pos_size;

		if(create_sentace_start_postions)
		{
			output = work_directory+input+"___"+to_string(0)+"___"+"sentace_start_postions";

			char_counter = 0;
			
			out_file.open(output.c_str(), ios::out | ios::trunc | ios::binary);

			buffer_size = 1024*1024;
			pos_buffer = (unsigned long long*)malloc(buffer_size*sizeof(long long));
			pos_buffer[0] = 0;
			buffer_pos_size = 1;
		}
		
		
		ifstream_continous_buffered input_file_woth_buffer((work_directory+input).c_str());

		while(input_file_woth_buffer.read_a_line(messageVar))
		{
			
			if(create_sentace_start_postions)
			{
				char_counter += messageVar.size()+1;

				pos_buffer[buffer_pos_size++] = char_counter;

				if(buffer_pos_size == buffer_size)
				{
					
					out_file.write((char*)pos_buffer, sizeof(long long)*buffer_pos_size);
					buffer_pos_size = 0;
				}
			}
			

			

		
			
			
		

		
		
		

		
		

			string body, subreddit_id, link_id, comment_id;
			read_json(messageVar, body, subreddit_id, link_id, comment_id);

			read_json_pro(work_directory, input, body, result, words_list, words_list_hashes, words_lenght, 0);
			
			count++;

			if(count % part_size == 0)
			{
				store_to_files(work_directory, input, number++);
				added_strings_pos_map.clear();
				
				
				
				pos_now = 0;
			}

			
		}
		
		

		
		if(create_sentace_start_postions)
		{
			if(buffer_pos_size != 0)
				
				out_file.write((char*)pos_buffer, sizeof(long long)*buffer_pos_size);

			free(pos_buffer);
			
			out_file.close();
		}
		

		if(count % part_size != 0)
		{
			store_to_files(work_directory, input, number);
			added_strings_pos_map.clear();
			
		}

	
		free(words_lenght);
		free(words_list);
		free(words_list_hashes);
		free(result);

		

		return number+1;
	}

	class create_word_positions
	{
	private:
		void do_merge_sort_on_times_found_only(job_runner &job)
		{
			sort_arrays.sort(job.times_found, job.sort_positions, job.pos_now);
			

			
			
			

			

			
			
			
		}

		void do_merge_sort_2(job_runner &job)
		{
			sort_arrays.sort(job.sort_positions, word_postions, job.pos_now, false);

			

			
			
			

			

			
			
			
		}

		void init_pos(job_runner &job)
		{
			job.sort_positions = (int*)malloc(job.pos_now*sizeof(int));
			for(int i = 0; i < job.pos_now; i++) job.sort_positions[i] = i;
		}

		

	public:
		int *word_postions;

		void do_create_word_positions(string work_directory, string input, int number)
		{
			job_runner job;

			job.read_block(work_directory, input, 0);
			init_pos(job);

			do_merge_sort_on_times_found_only(job);

			word_postions = (int*)malloc(job.pos_now*sizeof(int));

			int prev_times = job.times_found[0], rank_now = 1;

			

			for(int i = 0; i < job.pos_now; i++)
			{
				
				

				if(job.times_found[i] < prev_times)
				{
					prev_times = job.times_found[i];
					rank_now++;
				}

				
				word_postions[i] = rank_now;
			}

			

			

			
			

			do_merge_sort_2(job);

			
			

			string base = work_directory+input+"___"+to_string(number)+"___";
			job.write_block((base+"word_ranks").c_str(), word_postions, job.pos_now*sizeof(int));
			job.write_block((base+"max_word_rank").c_str(), &rank_now, sizeof(int));

			free(word_postions);
			job.free_completly();

			free(job.sort_positions);
		}
	};

	int *sort_array_buff_0, *sort_array_buff_1, *sort_array_buff_2;
	int *sort_positions;
	

	unsigned int *relationships;
	unsigned int relationships_counter,
		total_relationships_found;
	

	unsigned int *sentances_this_word_is_used;

	
	
	unordered_map<char*, int, hash_char, cmp_char> added_strings_pos_map;
	
	unsigned short 
		unique_words_in_comment;

	AVG_WORD_RANK_TYPE
		total_word_rank_in_comment;

	unsigned int *word_ranks_list,
		word_ranks_list_size,
		max_word_rank_now;

	
	
	
public:
	void simplify_relationships(string work_directory, string input)
	{
		

		string base = work_directory+input+"___"+to_string(0)+"___";

		unsigned long long actual_relationship_size = get_file_size(base+"relationships")/(2*sizeof(int));

		

		unsigned long long relationship_part = 10000000;
		unsigned int* actual_relationships = (unsigned int*)calloc(relationship_part, sizeof(int));

		unsigned long long actual_relationship_left = actual_relationship_size;
		
		while(actual_relationship_left != 0)
		{
			unsigned long long relationship_part_size_now = actual_relationship_left < relationship_part ? actual_relationship_left : relationship_part;

			bool is_append_mode = actual_relationship_left != actual_relationship_size;

			write_block((base+"actual_relationships").c_str(), actual_relationships, relationship_part_size_now*sizeof(int), is_append_mode);

			actual_relationship_left -= relationship_part_size_now;
		}
		
		free(actual_relationships);

		unsigned long long words_num = get_file_size(base+"sentances_this_word_is_used")/4;

		
		unsigned int *pos_conv = new unsigned int[words_num];
		for(unsigned int i = 0; i < words_num; i++) pos_conv[i] = i;

		ofstream ex_out((base+"actual_relationships_pos_converter").c_str(), ios::out | ios::binary | ios::trunc);
		ex_out.write((char*)pos_conv, sizeof(int)*words_num);
		ex_out.close();
		delete []pos_conv;
		

		unsigned int* num_of_sentances_this_word_is_used = (unsigned int*)malloc(words_num*sizeof(int));

		

		ifstream sentances_this_word_is_used_file(base+"sentances_this_word_is_used", ios::in | ios::binary);
		sentances_this_word_is_used_file.read((char*)num_of_sentances_this_word_is_used, sizeof(int)*words_num);
		sentances_this_word_is_used_file.close();

		unsigned long long* words_sentance_ids_start_pos = (unsigned long long*)malloc(words_num*sizeof(unsigned long long));

		unsigned long long current_pos = 0;
		for(unsigned long long i = 0; i < words_num; i++)
		{
			words_sentance_ids_start_pos[i] = current_pos;
			current_pos += num_of_sentances_this_word_is_used[i]*sizeof(int);
		}

		write_block((base+"actual_relationships_start_pos").c_str(), words_sentance_ids_start_pos, words_num*sizeof(unsigned long long));

		

		unsigned int* relationship_storing_pos = (unsigned int*)calloc(words_num, sizeof(int));

		
		ifstream  old_relationships(base+"relationships", ios::in | ios::binary);

		fstream new_relationships((base+"actual_relationships").c_str(), ios::binary | ios::out | ios::in);

		unsigned int write_data_buff_size = 100000000;

		unsigned int* old_rel_buff = (unsigned int*)malloc(write_data_buff_size*sizeof(int));
		unsigned int old_rel_buff_pos = 0;

		
		unsigned long long* seek_pos = (unsigned long long*)malloc(write_data_buff_size*sizeof(unsigned long long));
		unsigned int *sentance_ids = (unsigned int*)malloc(write_data_buff_size*sizeof(unsigned int));
		unsigned int seek_now = 0;

		for(unsigned long long i = 0; i < actual_relationship_size; i++)
		{
			if(i % (write_data_buff_size/2) == 0)
			{
				
				old_relationships.read((char*)old_rel_buff, sizeof(int)*write_data_buff_size);
				old_rel_buff_pos = 0;
			}

			unsigned int word_id = old_rel_buff[old_rel_buff_pos++], sentance_id = old_rel_buff[old_rel_buff_pos++];

			
			
			
			unsigned long long pos_to_write = words_sentance_ids_start_pos[word_id]+relationship_storing_pos[word_id]*sizeof(int);
			relationship_storing_pos[word_id]++;

			seek_pos[seek_now] = pos_to_write;
			sentance_ids[seek_now] = sentance_id;
			seek_now++;

			

			if(seek_now == write_data_buff_size)
			{
				
				sort_arrays.sort(seek_pos, sentance_ids, seek_now, false);

				for(unsigned int i = 0; i < seek_now; i++)
				{
					if(i == 0 || seek_pos[i] != seek_pos[i-1]+sizeof(int))
						new_relationships.seekg(seek_pos[i], ios::beg);

					new_relationships.write((char*)&sentance_ids[i], sizeof(int));
				}

				seek_now = 0;
			}
		}
		
		if(seek_now > 0)
			sort_arrays.sort(seek_pos, sentance_ids, seek_now, false);
			

		for(unsigned int i = 0; i < seek_now; i++)
		{
			if(i == 0 || seek_pos[i] != seek_pos[i-1]+sizeof(int))
				new_relationships.seekg(seek_pos[i], ios::beg);

			new_relationships.write((char*)&sentance_ids[i], sizeof(int));
		}

		
		old_relationships.close();
		new_relationships.close();

		free(relationship_storing_pos);
		free(num_of_sentances_this_word_is_used);
		free(words_sentance_ids_start_pos);
		free(old_rel_buff);
		free(seek_pos);
		free(sentance_ids);

		if(remove((base+"relationships").c_str()) != 0) cout << "REMOVE ERROR!\n";
		
	}

	void create_relationships(string work_directory, string input)
	{
		main_data_file.init();
		ifstream_continous_buffered continous_buffered_file(main_data_file);

		
		added_strings_pos_map.clear();

		string base = work_directory+input+"___"+to_string(0)+"___";

		
		
		do_read_block(work_directory, input, 0);

		binary_search_size_to_use = pos_now;

		relationships = (unsigned int*)malloc(4*10000000);
		relationships_counter = 0;
		total_relationships_found = 0;
		
		string messageVar;
		messageVar.reserve(1024*1024);
		char* result = (char*)malloc(8092*10);
		
	
	

		unsigned int sentance = 0;

		char** words_list = (char**)malloc(40000*sizeof(char*));
		int* words_list_hashes = (int*)malloc(40000*sizeof(int));
		int* words_lenght = (int*)malloc(40000*sizeof(int));

		

		sentances_this_word_is_used = (unsigned int*)calloc(pos_now, sizeof(unsigned int));
		
		ofstream_with_buffer 
			subreddit_writter((base+"subreddit_ids").c_str()),
			unique_words_in_comment_file((base+"unique_words_in_sentence").c_str()),
			total_word_rank_in_comment_file((base+"total_word_rank_in_sentence").c_str());

		word_ranks_list_size = get_file_size(base+"word_ranks")/sizeof(int);
		word_ranks_list = (unsigned int*)malloc(word_ranks_list_size*sizeof(int));
		read_file(base+"word_ranks", word_ranks_list, word_ranks_list_size*sizeof(int));

		read_file(base+"max_word_rank", &max_word_rank_now, sizeof(int));
		while(continous_buffered_file.read_a_line(messageVar))
		
		{
			
			
	
		
		
		
		
			
			

			
			
			
			
			
			
			
				
	
	
			

			
		

			
			

			

			string body, subreddit_id, link_id, comment_id;
			read_json(messageVar, body, subreddit_id, link_id, comment_id);

			unsigned long long subreddit_id_int;
			char output_category;
			reddit_id.encode_id(subreddit_id, subreddit_id_int, output_category);

			if(output_category != 5) {cout << "REDDIT CATEGORY WRONG";}
			

			if(SUBREDDIT_ID_USE)
				subreddit_writter.write((char*)&subreddit_id_int, sizeof(unsigned long long));

			

			read_json_pro(work_directory, input, body, result, words_list, words_list_hashes, words_lenght, sentance++, false);

			unique_words_in_comment_file.write(&unique_words_in_comment, sizeof(short));
			total_word_rank_in_comment_file.write(&total_word_rank_in_comment, sizeof(AVG_WORD_RANK_TYPE));

			added_strings_pos_map.clear();
			
		}

		if(relationships_counter != 0)
		
			
			write_sentance_link(work_directory, input);
		
		
		
		
		
		

		write_block((base+"sentances_this_word_is_used").c_str(), sentances_this_word_is_used, pos_now*sizeof(int));

		if(SUBREDDIT_ID_USE)
			subreddit_writter.flush_and_close();

		unique_words_in_comment_file.flush_and_close();
		total_word_rank_in_comment_file.flush_and_close();

		free(sentances_this_word_is_used);

		free(word_ranks_list);
		free(result);
		free(words_list);
		free(words_lenght);
		free(words_list_hashes);
		free(relationships);

		main_data_file.kill();

		
		
	}

	
	
		
		
		
		
		
		
	

	int binary_search_size_to_use;

	int pos_now;

	int *added_hashes_linear;
	int *times_found;
	char **added_strings;
	
	void crete_word_pos(string work_directory, string input, int number)
	{
		create_word_positions word_pos;
		word_pos.do_create_word_positions(work_directory, input, number);
	}

	template <typename T>
	void sort_based_on_pos_(T *&arr, unsigned int *positions, unsigned int size)
	{
		T *new_arr = new T[size];

		for(unsigned int i = 0; i < size; i++)
			new_arr[i] = arr[positions[i]];

		memcpy(arr, new_arr, size*sizeof(T));
		delete []new_arr;
	}

	void store_to_files(string work_directory, string input, int number)
	{
		sort_positions = (int*)malloc(pos_now*sizeof(int));
		for(int i = 0; i < pos_now; i++) sort_positions[i] = i;
		
		
		
		
		
		char **added_strings_data = new char*[pos_now];
		memcpy(added_strings_data, added_strings, sizeof(char*)*pos_now);
		unsigned int *srt_pos = (unsigned int*)sort_positions;
		sort_positions_based_on_hashes_and_then_words(added_hashes_linear, added_strings_data, srt_pos, pos_now);
		delete []added_strings_data;
		sort_based_on_pos_(times_found, srt_pos, pos_now);
		
		

		string base = work_directory+input+"___"+to_string(number)+"___";

		
		
		

		write_block((base+"times_found").c_str(), times_found, pos_now*sizeof(int));
		

		create_strings_array(base);

		free(sort_positions);
	}

	void add_from_another_array_to_this(const char* Word, int word_size, int counter_increase)
	{
		add_to_array(Word, hash_djb2(Word), word_size, counter_increase, true);
	}

	int save_arrays_to_files(string work_directory, string input_file)
	{
		job_runner job;
		return job.write_blocks(work_directory, input_file);
	}

	void read_block(string work_directory, string input, int number)
	{
		do_read_block(work_directory, input, number);
	}

	void free_completly()
	{
		free(added_hashes_linear);
		free(times_found);
		for(int i = 0; i < pos_now; i++) free(added_strings[i]);
		free(added_strings);
		pos_now = 0;
		binary_search_size_to_use = 0;
	}

	void free_excluding_strings()
	{
		free(added_hashes_linear);
		free(times_found);
		
		free(added_strings);
		pos_now = 0;
		binary_search_size_to_use = 0;
	}
};

class merge_arrays
{
public:
	void merge_2_files(string work_directory, string input_file_a, int number_a, string input_file_b, int number_b, string save_as_input, int save_as_number)
	{
		job_runner array_a_0, array_b_0, buff_0;
		job_runner *array_a = &array_a_0, *array_b = &array_b_0, *buff = &buff_0;

		array_a->read_block(work_directory, input_file_a, number_a);
		array_b->read_block(work_directory, input_file_b, number_b);

		if(array_a->pos_now < array_b->pos_now)
		{
			buff = array_b;
			array_b = array_a;
			array_a = buff;
		}

		array_a->added_hashes_linear = (int*)realloc(array_a->added_hashes_linear, sizeof(int)*(array_a->pos_now+array_b->pos_now));
		array_a->times_found = (int*)realloc(array_a->times_found, sizeof(int)*(array_a->pos_now+array_b->pos_now));
		array_a->added_strings = (char**)realloc(array_a->added_strings, sizeof(char*)*(array_a->pos_now+array_b->pos_now));
		
		array_a->binary_search_size_to_use = array_a->pos_now;

		for(int i = 0; i < array_b->pos_now; i++)
		{
			
				array_a->add_from_another_array_to_this(array_b->added_strings[i], strlen(array_b->added_strings[i]), array_b->times_found[i]);
		}

		array_b->free_completly();
		
		array_a->store_to_files(work_directory, save_as_input+"_temp", save_as_number);

		array_a->free_excluding_strings();
		

		

		if(remove((work_directory+save_as_input+"___"+to_string(save_as_number)+"___"+"times_found").c_str()) != 0) cout << "REMOVE ERROR!\n";
		if(remove((work_directory+save_as_input+"___"+to_string(save_as_number)+"___"+"added_strings_signle").c_str()) != 0) cout << "REMOVE ERROR!\n";

		string base = work_directory+save_as_input+"___"+to_string(save_as_number)+"___";

		if(rename(
			(work_directory+save_as_input+"_temp"+"___"+to_string(save_as_number)+"___"+"times_found").c_str(),
			(work_directory+save_as_input+"___"+to_string(save_as_number)+"___"+"times_found").c_str()
		) != 0) cout << "RENAME ERROR!\n";

		if(rename(
			(work_directory+save_as_input+"_temp"+"___"+to_string(save_as_number)+"___"+"added_strings_signle").c_str(),
			(work_directory+save_as_input+"___"+to_string(save_as_number)+"___"+"added_strings_signle").c_str()
		) != 0) cout << "RENAME ERROR!\n";

		
		
		
	}

	void remove_files(string work_directory, string save_as_input, int save_as_number)
	{
		if(remove((work_directory+save_as_input+"___"+to_string(save_as_number)+"___"+"times_found").c_str()) != 0) cout << "REMOVE ERROR!\n";
		if(remove((work_directory+save_as_input+"___"+to_string(save_as_number)+"___"+"added_strings_signle").c_str()) != 0) cout << "REMOVE ERROR!\n";
	}

	void simple_rename_file(string work_directory, string input_file_b, int number_b, string save_as_input, int save_as_number)
	{
		if(rename(
			(work_directory+input_file_b+"___"+to_string(number_b)+"___"+"times_found").c_str(),
			(work_directory+save_as_input+"___"+to_string(save_as_number)+"___"+"times_found").c_str()
		) != 0) cout << "RENAME ERROR!\n";

		if(rename(
			(work_directory+input_file_b+"___"+to_string(number_b)+"___"+"added_strings_signle").c_str(),
			(work_directory+save_as_input+"___"+to_string(save_as_number)+"___"+"added_strings_signle").c_str()
		) != 0) cout << "RENAME ERROR!\n";
	}
};

int ceate_file_parts(string work_directory, string file_name)
{
	job_runner job;
	return job.save_arrays_to_files(work_directory, file_name);
}

void merger(int items_no, string work_directory, string file_name)
{
	
	

	
	
	int save_pos = 0;
	int stored = 0;

	bool display = false;

	while(true)
	{
		for(int i = 0; i < items_no; i+=2)
		{
			if(i+1 != items_no)
			{
				if(display) cout << "MERGING " << i << " AND " << i+1 << "=> " << save_pos << "\n";

				merge_arrays j;
				j.merge_2_files(work_directory, file_name, i, file_name, i+1, file_name, save_pos++);
				
				stored+=2;
			}
			else
				stored++;
		}

		if(display) cout << "\n";

		for(int i = save_pos; i < items_no - (items_no % 2); i++)
		{
			merge_arrays j;
			j.remove_files(work_directory, file_name, i);

			if(display) cout << "REMOVING " << i << "\n";
		}

		if(items_no % 2 == 1)
		{
			merge_arrays j;
			j.simple_rename_file(work_directory, file_name, (items_no-1), file_name, save_pos++);

			if(display) cout << "[]RENAMING " << (items_no-1) << " TO " << (save_pos-1) << "\n";
		}
		

		items_no = (int)(items_no/2) + (items_no % 2);

		if(items_no == 1 && save_pos == 1)
			break;

		stored = 0;
		save_pos = 0;
	}
}

void create_word_pos(string work_directory, string file_name)
{
	
	job_runner job;
	
	

	job.crete_word_pos(work_directory, file_name, 0);

	

	
}

void test_sentance_start_positions()
{
	
	

	

	std::ifstream read_test ("G:\\REDDIT DATABASE\\OUTPUT\\RC_2015-01___0___sentace_start_postions", std::ifstream::binary | ios::in);

	

	unsigned long long first_number, second_number;

	int read_num = 1;

	read_test.seekg((read_num-1)*sizeof(long long));

	read_test.read((char*)(&first_number), sizeof(long long));
	read_test.read((char*)(&second_number), sizeof(long long));

	read_test.close();

	

	

	

	

	cout << first_number << " " << second_number << "\n";

	std::ifstream is ("G:\\REDDIT DATABASE\\RC_2015-01", std::ifstream::binary | ios::in);

	if (is) {
		
		is.seekg (first_number);
		
		

		
		char * buffer = new char [second_number-first_number];

		
		is.read (buffer, second_number-first_number);

		is.close();

		
		

		cout << "\n|" << buffer << "|\n\n";
		delete[] buffer;

	}

	
}
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

void test_word_ranks()
{
	job_runner job;

	unsigned long long positions_file_size = get_file_size("G:\\REDDIT DATABASE\\OUTPUT\\RC_2015-01___0___word_ranks");
	unsigned int *positions = (unsigned int*)malloc(positions_file_size);
	FILE* positions_file;
	positions_file = fopen("G:\\REDDIT DATABASE\\OUTPUT\\RC_2015-01___0___word_ranks", "rb");
	fread(positions, 1, positions_file_size, positions_file);
	fclose(positions_file);

	
	

	
	
	
	
	unsigned long long strings_file_size = get_file_size("G:\\REDDIT DATABASE\\OUTPUT\\RC_2015-01___0___added_strings_signle");
	char *strings = (char*)malloc(strings_file_size);

	FILE* strings_file;
	strings_file = fopen("G:\\REDDIT DATABASE\\OUTPUT\\RC_2015-01___0___added_strings_signle", "rb");
	fread(strings, 1, strings_file_size, strings_file);
	fclose(strings_file);

	
	unsigned int *strings_start_pos = (unsigned int*)malloc(positions_file_size);
	int pos = 1;
	strings_start_pos[pos] = 0;

	for(int i = 1; i < strings_file_size; i++)
		if(strings[i] == 0)
			strings_start_pos[pos++] = i+1;
	pos--;

	

	int counter = 0;
	string words = "I was lucky enough to go to Paris last summer, I didn't run into one rude Parisian.";
	string pr;
	for(int i = 0; i < words.size(); i++)
	{
		words[i] = tolower(words[i]);
		if(words[i] == ',' || words[i] == '.') words[i] = ' ';
		if((words[i] >= 'a' && words[i] <= 'z') || words[i] == ' ') pr.push_back(words[i]);
	}

	vector<string> strs;
	split(words, ' ', strs);
	
	sort( strs.begin(), strs.end() );
	strs.erase( unique( strs.begin(), strs.end() ), strs.end() );

	cout << "\n";

	

	int zx = 0;
	for(int i = 0; i < positions_file_size/4; i++)
	{

		
		
		

		

		
		
		for(int j = 0; j < strs.size(); j++)

		if(strcmp(strs[j].c_str(), &strings[strings_start_pos[i]]) == 0)
			cout << "Word: " << " | Times Found: " << " | Rarity: " << 10000*positions[i]/positions[positions_file_size/4-1] << "/10000 \n";
	}
	
	

	

	
}

void create_parts_and_merge_them(string work_directory, string file_name, char *thread_state)
{
	merger(ceate_file_parts(work_directory, file_name), work_directory, file_name);
	*thread_state = 2;
}

void create_parts_using_threads()
{
	int 
		items_started = 0,
		active_threads = 0,
		items_finished = 0;

	char *thread_state = (char*)calloc(items_parts_total_number, sizeof(char));

	while(items_finished < items_parts_total_number)
	{
		if(active_threads < part_creation_threads_num && items_started < items_parts_total_number)
		{
			thread_state[items_started] = 1;
			boost::thread(create_parts_and_merge_them, string(base_work_directory+data_file_folder_list[items_started]+"\\"), data_file_list[items_started], &thread_state[items_started]);
			active_threads++;
			items_started++;
		}
		else
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(100));

			for(int i = 0; i < items_parts_total_number; i++)
				if(thread_state[i] == 2)
				{
					thread_state[i] = 0;
					items_finished++;
					active_threads--;
				}
		}
	}

	free(thread_state);
}

void merge_sentace_start_postions(string dir_base)
{
	ofstream output(dir_base+"index_temp_sentace_start_postions", ios::out | ios::trunc | ios::binary);

	int buffer_size = 5000000;
	unsigned long long* data_buffer = (unsigned long long*)malloc(buffer_size*sizeof(unsigned long long));

	unsigned long long file_pos = 0;

	for(int i = 0; i < items_parts_total_number; i++)
	{
		unsigned long long items_in_positions = get_file_size(dir_base+"\\index___"+to_string(i)+"___sentace_start_postions")/sizeof(unsigned long long);

		ifstream input_file(dir_base+"\\index___"+to_string(i)+"___sentace_start_postions", ios::in | ios::binary);
		
		while(items_in_positions > 0)
		{
			unsigned long long part_size_right_now = (buffer_size > items_in_positions ? items_in_positions : buffer_size);

			input_file.read((char*)data_buffer, buffer_size*sizeof(unsigned long long));

			unsigned long long read_now = input_file.gcount()/sizeof(unsigned long long);

			if(part_size_right_now != read_now || read_now == 0) {cout << "ERROR!!!!! " << items_in_positions << " | " << part_size_right_now << " " << read_now << "\n";cin.get();}

			for(int i = 0; i < read_now; i++) data_buffer[i] += file_pos;

			if(read_now > items_in_positions) {cout << "read_now " << read_now << " " << items_in_positions << "\n"; cin.get();}

			items_in_positions = items_in_positions-read_now;

			if(items_in_positions == 0)
			{
				if(read_now > 1)
					output.write((char*)data_buffer, (read_now-1)*sizeof(unsigned long long));

				file_pos = data_buffer[(read_now-1)];

				
				
			}
			else
				output.write((char*)data_buffer, read_now*sizeof(unsigned long long));
		}

		input_file.close();

		if(remove((dir_base+"\\index___"+to_string(i)+"___sentace_start_postions").c_str()) != 0) cout << "REMOVE ERROR!\n";
	}

	output.write((char*)&file_pos, sizeof(unsigned long long));

	free(data_buffer);

	output.close();

	if(rename(
		(dir_base+"index_temp_sentace_start_postions").c_str(),
		(dir_base+"index___0___sentace_start_postions").c_str()
		) != 0) cout << "RENAME ERROR!\n";
}

void run()
{
	vector<unsigned int> times(6);

	std::clock_t 
		start = std::clock(),
		buff;

	_mkdir((base_work_directory+data_file_merged_directory).c_str());

	cout << "STARTING INDEX CREATION.\n\n";
	buff = std::clock();

	cout << "1/6. CREATING INDEX BLOCKS & INDEX POINTS (MULTI THREADED).\n";
	create_parts_using_threads();
	times[0] = ( std::clock() - buff ) / (double) CLOCKS_PER_SEC;
	cout << "1/6. DONE. TIME USED " << times[0] << " SECONDS.\n\n";
	buff = std::clock();

	cout << "2/6. MERGING INDEX BLOCKS.\n";

	

	for(int i = 0; i < items_parts_total_number; i++)
	{
		string names[] = {"times_found", "added_strings_signle", "sentace_start_postions"};

		for(int j = 0; j < sizeof(names)/sizeof(names[0]); j++)
			if(rename(
				(base_work_directory+data_file_folder_list[i]+"\\"+data_file_list[i]+"___0___"+names[j]).c_str(),
				(base_work_directory+data_file_merged_directory+"\\index___"+to_string(i)+"___"+names[j]).c_str()
				) != 0) cout << "RENAME ERROR!\n";
	}

	

	merger(items_parts_total_number, base_work_directory+data_file_merged_directory+"\\", "index");
	times[1] = ( std::clock() - buff ) / (double) CLOCKS_PER_SEC;
	cout << "2/6. DONE. TIME USED " << times[1] << " SECONDS.\n\n";
	buff = std::clock();

	cout << "3/6. MERGING INDEX POINTS.\n";
	merge_sentace_start_postions(base_work_directory+data_file_merged_directory+"\\");
	times[2] = ( std::clock() - buff ) / (double) CLOCKS_PER_SEC;
	cout << "3/6. DONE. TIME USED " << times[2] << " SECONDS.\n\n";
	buff = std::clock();

	

	cout << "4/6. RATING WORDS BASED ON RARITY.\n";
	create_word_pos(base_work_directory+data_file_merged_directory+"\\", "index");
	times[3] = ( std::clock() - buff ) / (double) CLOCKS_PER_SEC;
	cout << "4/6. DONE. TIME USED " << times[3] << " SECONDS.\n\n";
	buff = std::clock();

	cout << "5/6. CREATING WORD - SENTENCE RELATIONSHIPS (1/2 PASS).\n";
	job_runner job1;
	job1.create_relationships(base_work_directory+data_file_merged_directory+"\\", "index");
	times[4] = ( std::clock() - buff ) / (double) CLOCKS_PER_SEC;
	cout << "5/6. DONE. TIME USED " << times[4] << " SECONDS.\n\n";
	buff = std::clock();

	cout << "6/6. CREATING WORD - SENTENCE RELATIONSHIPS (2/2 PASS).\n";
	job_runner job2;
	job2.simplify_relationships(base_work_directory+data_file_merged_directory+"\\", "index");
	times[5] = ( std::clock() - buff ) / (double) CLOCKS_PER_SEC;
	cout << "6/6. DONE. TIME USED " << times[5] << " SECONDS.\n\n";

	cout << "INDEX CREATION COMPLETED. TOTAL TIME USED " << (unsigned int)(( std::clock() - start ) / (double) CLOCKS_PER_SEC) << " SECONDS.\n\n";

	unsigned int time_sum = 0;
	for(int i = 0; i < 6; i++) time_sum += times[i];

	cout << "PERCENTENCE OF TIME BREAKDOWN BY STEP | 1 -> " 
		<< (unsigned int)(100*((double)times[0]/time_sum)) 
		<< "% | 2 -> " << (unsigned int)(100*((double)times[1]/time_sum)) 
		<< "% | 3 -> " << (unsigned int)(100*((double)times[2]/time_sum)) 
		<< "% | 4 -> " << (unsigned int)(100*((double)times[3]/time_sum)) 
		<< "% | 5 -> " << (unsigned int)(100*((double)times[4]/time_sum)) 
		<< "% | 6 -> " << (unsigned int)(100*((double)times[5]/time_sum)) << "% |\n\n";

	
	
	cin.get();
}

class main_program
{
private:
	unsigned int max_rank;
	string files_base;

	char** words;
	int* word_hashes;
	unsigned int words_number;
	unsigned int sentances_number;

	unsigned int
		*main_word_ranks_list,
		*pos_converter;

	
	

	AVG_WORD_RANK_TYPE *main_avg_word_rank;
	
	
		

	int sentences_word_appears_at(int word_id)
	{
		if(word_id == -1 || word_id >= words_number) {cout << "INPUT WORD POS = -1 OR TOO BIG!\n"; return -1;}

		int buff;
		read_file(sentances_this_word_is_used, &buff, sizeof(int), sizeof(int)*word_id);
		return buff;
	}

	template <typename T>
	void allocate_and_read_file(string name, T* &ptr, unsigned long long items_num)
	{
		ptr = (T*)malloc(items_num*sizeof(T));
		read_full_file(name, ptr, items_num*sizeof(T));
	}
	
	
	
		
		
	
		

	inline unsigned char get_final_score(const unsigned int &pos, const unsigned int &unique_words_num_in_sentence, const unsigned int &input_sentence_word_value_sum)
	{
		return item_scorer.get_final_score(unique_words_num_in_sentence, input_sentence_word_value_sum,
			main_unique_words[pos], current_unique_words[pos], main_avg_word_rank[pos], current_total_word_rank[pos]);
		
		
		
		
		

		

		
		
		
		
	}

	void worker_thread(on_the_fly_top_items_array &in_data, unsigned int start_pos, unsigned int &end_pos, unsigned int unique_words_num_in_sentence, unsigned int input_sentence_word_value_sum)
	{
		for(unsigned int i = start_pos; i < end_pos; i++)
			in_data.add_item(get_final_score(i, unique_words_num_in_sentence, input_sentence_word_value_sum) , i);

		end_pos = 0;
	}

public:
	void get_best_matching_comment_id(unsigned int unique_words_num_in_sentence, unsigned int &input_sentence_word_value_sum, unsigned int* &data_x, unsigned int* &data_x_pos, unsigned int &sentance_ids_num)
	{
		
		

		current_unique_words.finish_adding();
		current_total_word_rank.finish_adding();

			
			

		on_the_fly_top_items_array top_items;
		top_items.init(5000);

		unsigned int threads_no = std::thread::hardware_concurrency();
		if(threads_no == 0) threads_no = 4;

		on_the_fly_top_items_array *top_items_threads = new on_the_fly_top_items_array[threads_no];
		for(int i = 0; i < threads_no; i++) top_items_threads[i].init(5000);

		unsigned int part_size = sentances_number/threads_no;
		unsigned pos_now = 0;

		unsigned int *end_pos_threads = new unsigned int[threads_no];
		for(int i = 0; i < threads_no; i++)
		{
			end_pos_threads[i] = pos_now+part_size;
			if(i+1 == threads_no) end_pos_threads[i] += sentances_number % part_size;

			boost::thread(&main_program::worker_thread, this, boost::ref(top_items_threads[i]), pos_now, boost::ref(end_pos_threads[i]), unique_words_num_in_sentence, input_sentence_word_value_sum);
			pos_now += part_size;
		}

		for(int i = 0; i < threads_no; i++)
		{
			while(end_pos_threads[i] != 0)
				boost::this_thread::sleep(boost::posix_time::microsec(1));

			unsigned int a = 0, b = 0, pos_i = 0;
			while(top_items_threads[i].get_one(a, b, pos_i))
			{
				top_items.add_item(a, b);
				pos_i++;
			}

			top_items_threads[i].clear();
		}

		

		
		

			

			
			
			

			

			

			
			
			
			

			
			

			
		

		top_items.get_items_sorted(data_x, data_x_pos, sentance_ids_num);
		
		current_total_word_rank.memset_to_0();
		current_unique_words.memset_to_0();

		
		
		
	}

	void word_id_to_comment_ids_with_apply(unsigned int word_id)
	{
		unsigned int* sentance_ids;
		unsigned int sentance_ids_num;

		
		if(word_id == -1 || word_id >= words_number) {sentance_ids_num = 0; cout << "INPUT WORD POS = -1 OR TOO BIG!\n"; return;}

		sentance_ids_num = sentences_word_appears_at(word_id);
		if(sentance_ids_num == 0) cout << "ERROR NOT EXPETCED!";

		unsigned long long relationship_start_pos = 0;

		read_file(actual_relationships_start_pos, &relationship_start_pos, sizeof(long long), sizeof(long long)*pos_converter[word_id]);
		
		unsigned int buff_size = 1024*1024*3;
		sentance_ids = (unsigned int*)malloc(sizeof(int)*buff_size);
		unsigned int now = 0;
		
		unsigned int word_value_now = main_word_ranks_list[word_id];
		
		while(true)
		{
			unsigned int items_now = (now+buff_size >= sentance_ids_num) ? (sentance_ids_num-now) : buff_size;

			read_file(actual_relationships, sentance_ids, sizeof(int)*items_now, (now == 0 ? relationship_start_pos : ULLONG_MAX));

			for(unsigned int i = 0; i < items_now; i++)
			{
				current_unique_words.add(sentance_ids[i]);
				current_total_word_rank.add(sentance_ids[i], word_value_now);
				
				
			}

			now += items_now;

			if(now == sentance_ids_num)
				break;
		}

		free(sentance_ids);

		
	}

	unsigned int get_word_rank(unsigned int &word_id)
	{
		return main_word_ranks_list[word_id];
	}

	void comment_id_to_comment(unsigned int comment_id, char* &output)
	{
		if(comment_id >= sentances_number) {cout << "COMMENT ID DOESN'T EXIST!\n"; return;}

		unsigned long long sentance_pos[2] = {0};
		read_file(sentace_start_postions, sentance_pos, 2*sizeof(long long), comment_id*sizeof(long long));

		unsigned int output_size = (unsigned long long)(sentance_pos[1]-sentance_pos[0]);
		output = (char*)malloc(output_size+1);
		
		main_data_file.read_data(output, output_size, sentance_pos[0]);
		output[output_size] = 0;
	}

	void string_to_word_ids_and_ranks(string &input, int* &words_list, int* &word_ranks_list, int* &relationships_num, int &words_number_in_sentance, int &ignored_words, bool show = true)
	{
		split_sentance_to_unique_words_ids(input, words_list, words_number_in_sentance, ignored_words);
		
		if(words_number_in_sentance > 0)
		{
			word_ranks_list = (int*)malloc(words_number_in_sentance*sizeof(int));
			relationships_num = (int*)malloc(words_number_in_sentance*sizeof(int));

			for(int i = 0; i < words_number_in_sentance; i++)
			{
				word_ranks_list[i] = main_word_ranks_list[words_list[i]];
				if(show) cout << "Word: " << word_id_to_word(words_list[i]) << " | Rank: " << 100*word_ranks_list[i]/WORD_RANK_MAX << "/100\n";
				relationships_num[i] = sentences_word_appears_at(words_list[i]);
			}

			if(show) cout << "\n";
		}
	}

	void comment_id_to_word_ids_and_ranks(unsigned int comment_id, int* &words_list, int* &word_ranks_list, int* &relationships_num, int &words_number_in_sentance)
	{
		if(comment_id >= sentances_number) {cout << "COMMENT ID DOESN'T EXIST!\n"; return;}

		char* output;
		comment_id_to_comment(comment_id, output);

		string body = get_body_from_json(output);
		free(output);

		int ignored_words = 0;

		string_to_word_ids_and_ranks(body, words_list, word_ranks_list, relationships_num, words_number_in_sentance, ignored_words);

		if(ignored_words > 0) {cout << "ERROR! A sentence that exists in the database has unkwown words!\n"; cin.get();}
	}

	string word_id_to_word(int word_id)
	{
		if(word_id == -1 || word_id >= words_number) {cout << "INPUT WORD POS = -1 OR TOO BIG!\n"; return "";}

		return string(words[word_id]);
	}

	void clear()
	{
		

		actual_relationships.close();
		actual_relationships_start_pos.close();
		added_strings_signle.close();
		sentace_start_postions.close();
		sentances_this_word_is_used.close();
		word_ranks.close();

		free(pos_converter);

		free(word_hashes);

		for(unsigned int i = 0; i < words_number; i++)
			free(words[i]);

		free(words);
		
		free(main_avg_word_rank);
		

		
		
		free(main_word_ranks_list);

		max_rank = 0;
		files_base = "";
		words_number = 0;
		sentances_number = 0;

		main_data_file.kill();
	}

	void init()
	{
		

		

		

		

		files_base = base_work_directory+data_file_merged_directory+"\\index___0___";

		actual_relationships.open(files_base+"actual_relationships", ios::binary | ios::in),
		actual_relationships_start_pos.open(files_base+"actual_relationships_start_pos", ios::binary | ios::in),
		added_strings_signle.open(files_base+"added_strings_signle", ios::binary | ios::in),
		sentace_start_postions.open(files_base+"sentace_start_postions", ios::binary | ios::in),
		sentances_this_word_is_used.open(files_base+"times_found", ios::binary | ios::in),
		word_ranks.open(files_base+"word_ranks", ios::binary | ios::in);
		
		main_data_file.init();
		read_sentances_number();
		read_words_into_memory();

		pos_converter = (unsigned int*)malloc(words_number*sizeof(int));
		read_full_file(files_base+"actual_relationships_pos_converter", pos_converter, words_number*sizeof(int));

		read_full_file(files_base+"max_word_rank", &max_rank, sizeof(int));

		
		allocate_and_read_file(files_base+"total_word_rank_in_sentence", main_avg_word_rank, sentances_number);

		unsigned short* main_unique_words_buff;
		allocate_and_read_file(files_base+"unique_words_in_sentence", main_unique_words_buff, sentances_number);

		

		allocate_and_read_file(files_base+"word_ranks", main_word_ranks_list, words_number);
		for(int i = 0; i < words_number; i++) main_word_ranks_list[i] = (unsigned long long)WORD_RANK_MAX*main_word_ranks_list[i]/max_rank;

		unsigned int* dt_buff_x = new unsigned int[sentances_number];

		for(unsigned int i = 0; i < sentances_number; i++)
		{
			
			dt_buff_x[i] = (unsigned int)main_avg_word_rank[i]*main_unique_words_buff[i];
		}

		current_total_word_rank.max_init(dt_buff_x, sentances_number);

		delete []dt_buff_x;

		current_unique_words.max_init(main_unique_words_buff, sentances_number);

		main_unique_words.init(main_unique_words_buff, sentances_number);

		
		

		
	}
};

void tests()
{

#ifdef NOT_THERE
	std::clock_t start = std::clock();
	cout << "STARTING EXECUTION\n";
	string input = "Battle! Aqua/Magma Leader Music dfgsdfhh";

	char** words_list;
	int words_number_in_sentance;
	int ignored_words;

	input_string_to_words(input, words_list, words_number_in_sentance, ignored_words);

	cout << "KNOWN WORDS: " << words_number_in_sentance << " | UNKWOWN WORDS: " << ignored_words << "\n";

	for(int i = 0; i < words_number_in_sentance; i++)
	{
		
		cout << "Word '" << words[i] << "' not in database\n";
	}
	unsigned int data_sum = 0;

	
	
	

	

	

	string words[] = {"be", "the"};
	int items_in_array = sizeof(words)/sizeof(words[0]);

	unsigned int **positions_pointer_list = (unsigned int**)malloc(sizeof(int*)*items_in_array);
	unsigned int *positions_pointer_list_size = (unsigned int*)malloc(sizeof(int)*items_in_array);

	for(int i = 0; i < items_in_array; i++)
	{
		int word_pos_num = word_pos((char*)words[i].c_str());

		if(word_pos_num == -1)
		{
			cout << "Word '" << words[i] << "' not in database\n";
			cin.get();
		}
		else
		{
			comment_ids_list(word_pos_num, positions_pointer_list[i], positions_pointer_list_size[i]);
			cout << "Word '" << words[i] << "' appears in " << positions_pointer_list_size[i] << " comments and has a rarity of " << word_rank(word_pos_num) << "/10000\n";
		}
	}
		
	int *output_array;
	int output_array_size;

	ofstream out_put_test("C:\\Users\\Shadow\\Documents\\DATA_TEST_0", ios::out);
	out_put_test.write((char*)positions_pointer_list[0], positions_pointer_list_size[0]*sizeof(int));
	out_put_test.close();

	ofstream out_put_test1("C:\\Users\\Shadow\\Documents\\DATA_TEST_1", ios::out);
	out_put_test1.write((char*)positions_pointer_list[1], positions_pointer_list_size[1]*sizeof(int));
	out_put_test1.close();

	array_intersection((int*)positions_pointer_list[0], (int*)positions_pointer_list[1], positions_pointer_list_size[0], positions_pointer_list_size[1], output_array, output_array_size);

	cout << "Found " << output_array_size << " comments which contain all the words\n\n";
#endif
	
		
	

}

class deep_scan
{
private:
	unsigned int 
		items_no,
		item_now;

	main_program *main_prg;

	unsigned int 
		**word_lists,
		*word_lists_items_no;

	int
		*sentence_ids;

	void do_add(string &item, unsigned int sentence_id, unsigned int score)
	{
		int *words_list;
		int *word_ranks_list;
		int *word_relationships;

		int words_number_in_sentance;
		int ignored_words;

		main_prg->string_to_word_ids_and_ranks(item, words_list, word_ranks_list, word_relationships, words_number_in_sentance, ignored_words);

		word_lists[item_now] = (unsigned int*)words_list;
		word_lists_items_no[item_now] = words_number_in_sentance;
		sentence_ids[item_now] = sentence_id;

		free(word_ranks_list);
		free(word_relationships);

		item_now++;
	}

	int find_map_second(const unordered_map<unsigned int, unsigned int> &map, const unsigned int &word_id)
	{
		auto search = map.find(word_id);
		
		if(search != map.end())
			return search->second;

		return -1;
	}

	void score_all(unsigned int *&item_ids, unsigned int *&item_scores, unsigned int &items_no_)
	{
		if(items_no+1 != item_now) {cout << "ITEMS DON'T MATCH " << items_no << " | " << item_now << "\n"; cin.get();}
		

		unsigned int total_words_num = 0;

		for(unsigned int i = 1; i < item_now; i++)
			total_words_num += word_lists_items_no[i];

		unsigned int *unique_words = new unsigned int[total_words_num];
		unsigned int *unique_words_times_found = new unsigned int[total_words_num];
		unsigned int unique_words_count = 0;

		unordered_map<unsigned int, unsigned int> word_map;

		for(unsigned int i = 1; i < item_now; i++)
			for(unsigned int j = 0; j < word_lists_items_no[i]; j++)
			{
				int word_pos = find_map_second(word_map, word_lists[i][j]);

				if(word_pos != -1)
					unique_words_times_found[word_pos]++;
				else
				{
					word_map[word_lists[i][j]] = unique_words_count;

					unique_words[unique_words_count] = word_lists[i][j];
					unique_words_times_found[unique_words_count] = 1;

					unique_words_count++;
				}
			}

		word_map.clear();
		
		unsigned int *new_word_ranks = new unsigned int[unique_words_count];

		
		int prev_times = unique_words_times_found[0], rank_now = 1;

		for(unsigned int i = 0; i < unique_words_count; i++)
		{
			if(unique_words_times_found[i] < prev_times)
			{
				prev_times = unique_words_times_found[i];
				rank_now++;
			}

			new_word_ranks[i] = rank_now;
		}

		unordered_map<unsigned int, unsigned int> rank_map;
		for(unsigned int i = 0; i < unique_words_count; i++)
		{
			new_word_ranks[i] = (unsigned long long)WORD_RANK_MAX*new_word_ranks[i]/rank_now;
			rank_map[unique_words[i]] = new_word_ranks[i];
		}

		unsigned int *total_sentence_word_ranks = new unsigned int[item_now]();

		for(unsigned int i = 0; i < item_now; i++)
			for(unsigned int j = 0; j < word_lists_items_no[i]; j++)
			{
				int map_second = find_map_second(rank_map, word_lists[i][j]);
				if(map_second == -1)
				{
					if(i == 0)
						total_sentence_word_ranks[i] += WORD_RANK_MAX;
					else
						{cout << "NOT EXPECTED 0! " << i << " " << j << "\n"; cin.get();}
				}
				else total_sentence_word_ranks[i] += map_second;
			}

		

		unsigned int 
			*common_points = new unsigned int[item_now](),
			*common_words = new unsigned int[item_now]();

		common_points[0] = total_sentence_word_ranks[0];
		common_words[0] = word_lists_items_no[0];

		unsigned int *pos_buff = new unsigned int[item_now]();

		for(unsigned int i = 0; i < word_lists_items_no[0]; i++)
		{
			unsigned int id_now = word_lists[0][i];

			for(unsigned int j = 1; j < item_now; j++)
				
				for(unsigned int k = pos_buff[j]; k < word_lists_items_no[j]; k++)
				{
					unsigned int word_id_now = word_lists[j][k];
					if(word_id_now == id_now)
					{
						int map_second = find_map_second(rank_map, word_id_now);
						if(map_second == -1) {cout << "NOT EXPECTED 1!\n"; cin.get();}
						else
						{
							common_points[j] += map_second;
							common_words[j]++;
						}
					}
					else if(word_id_now > id_now)
					{
						pos_buff[j] = k;
						break;
					}

				}
		}

		rank_map.clear();
		delete []pos_buff;
		

		
		
		
		items_no_ = item_now-1;

		unsigned int *item_ids = new unsigned int[item_now];
		unsigned int *item_scores = new unsigned int[item_now];
		
		for(unsigned int i = 1; i < item_now; i++)
		{
			unsigned int score = (word_lists_items_no[i] > 0 && common_words[i] > 0) ? item_scorer.get_final_score(
				word_lists_items_no[0], total_sentence_word_ranks[0],
				word_lists_items_no[i], common_words[i],
				total_sentence_word_ranks[i]/word_lists_items_no[i], common_points[i]/common_words[i], true) : 0;

			
			item_ids[i-1] = sentence_ids[i];
			item_scores[i-1] = score;
			
						char* output;
			main_prg->comment_id_to_comment(sentence_ids[i], output);

			string out_put_sentence = read_json_body(output);
			free(output);

			printf("V2 SCORE: %d [%s]\n", (unsigned int)((double)score), out_put_sentence.c_str());

		}
		
		

		

		

		

	}
public:
	deep_scan(main_program &main_prg, unsigned int items_no, string &input): items_no(items_no), main_prg(&main_prg), item_now(0)
	{
		if(items_no <= 1) return;

		word_lists = new unsigned int*[items_no];
		word_lists_items_no = new unsigned int[items_no];
		sentence_ids = new int[items_no];

		do_add(input, -1, 0);
	}

	void add_item(string &item, unsigned int &sentence_id, unsigned int &score)
	{
		if(items_no <= 1) return;

		do_add(item, sentence_id, score);
	}

	unsigned int get_top()
	{
		if(items_no <= 1) return 0;

		unsigned int 
			*item_ids,
			*item_scores,
			items_no_;

		score_all(item_ids, item_scores, items_no_);

		for(int i = 0; i < items_no_; i++)
		{
			char* output;
			cout << item_ids[i] << "\n";
			main_prg->comment_id_to_comment(item_ids[i], output);

			string out_put_sentence = read_json_body(output);
			free(output);

			printf("V2 SCORE: %d [%s]\n", (unsigned int)((double)item_scores[i]/2.5), out_put_sentence.c_str());
		}

		return 0;
	}
};

class sentence_finder
{
private:
	void array_intersection(int *arr1, int *arr2, int m, int n, int *&output_array, int &output_array_size)
	{
		output_array = (int*)malloc(sizeof(int)*(m > n ? m : n));
		output_array_size = 0;

		int i = 0, j = 0;
		while (i < m && j < n)
		{
			if (arr1[i] < arr2[j])
				i++;
			else if (arr2[j] < arr1[i])
				j++;
			else
			{
				output_array[output_array_size++] = arr2[j++];
				i++;
			}
		}

		free(arr1);
		free(arr2);
	}

	void array_intersection(unsigned int *&arr1, unsigned int *&arr2, unsigned int &m, unsigned int &n)
	{
		
		if(m < n)
		{
			unsigned int *temp = arr1;
			arr1 = arr2;
			arr2 = temp;

			unsigned int size_temp = m;
			m = n;
			n = size_temp;
		}

		unsigned int *buff = (unsigned int*)malloc(n*sizeof(int));

		unsigned int output_array_size = 0;

		unsigned int i = 0, j = 0;
		while (i < m && j < n)
		{
			if (arr1[i] < arr2[j])
				i++;
			else if (arr2[j] < arr1[i])
				j++;
			else
			{
				buff[output_array_size++] = arr2[j++];
				i++;
			}
		}

		free(arr1);
		free(arr2);
	
		arr1 = buff;
		m = output_array_size;
	}

	void multiple_array_intersection(unsigned int **positions_pointer_list, unsigned int *positions_pointer_list_size, int items_num, unsigned int *&output, unsigned int &output_size)
	{
		for(int i = 1; i < items_num; i++)
				array_intersection(positions_pointer_list[0], positions_pointer_list[i], positions_pointer_list_size[0], positions_pointer_list_size[i]);

		output = positions_pointer_list[0];
		output_size = positions_pointer_list_size[0];

		free(positions_pointer_list);
		free(positions_pointer_list_size);
	}
	
	string read_json_body(char* input)
	{
		Document document;
		document.Parse(input);

		return document["body"].GetString();
	}

public:
	void run(const char* input)
	{
		main_program main_prg;
		main_prg.init();

		
		{

		std::clock_t start = std::clock();

		
		
		
		
		
		
		
		
		string sentence = string(input);
		
		

		
		cout << "-------------------------------------------------------------\n-------------------------------------------------------------\n\n";
		cout << "Input Sentence: [" << sentence << "]\n\n";

		int* words_list;
		int* word_ranks_list;
		int* word_relationships;

		int words_number_in_sentance;
		int ignored_words;

		main_prg.string_to_word_ids_and_ranks(sentence, words_list, word_ranks_list, word_relationships, words_number_in_sentance, ignored_words);

		unsigned int input_sentence_word_value_sum = 0;
		for(int i = 0; i < words_number_in_sentance; i++)
			input_sentence_word_value_sum += word_ranks_list[i];

		
		
		
		
		
		

		if(words_number_in_sentance > 0)
		{
			
			

			

			for(int i = 0; i < words_number_in_sentance; i++)
			{
				main_prg.word_id_to_comment_ids_with_apply(words_list[i]);
				
				
				
			}

			unsigned int* data_scores;
			unsigned int* data_pos;
			unsigned int data_num;

			main_prg.get_best_matching_comment_id(words_number_in_sentance, input_sentence_word_value_sum, data_scores, data_pos, data_num);
			
			std::cout << "Total Results: " << data_num << " | Time: " << ( std::clock() - start ) / (double) CLOCKS_PER_SEC << "s\n\n";
			
			deep_scan deep_scanner(main_prg, data_num, sentence);

			for(int i = 0; i < data_num; i++)
			{
				char* output;

				main_prg.comment_id_to_comment(data_pos[i], output);

				string out_put_sentence = read_json_body(output);

				if(i < 50)
					printf("SCORE: %d [%s]\n", (unsigned int)((double)data_scores[i]/2.5), out_put_sentence.c_str());

				deep_scanner.add_item(out_put_sentence, data_pos[i], data_scores[i]);

				free(output);
			}
			
			if(data_num > 0)
			{
				char* output;
				unsigned int result_id = deep_scanner.get_top();

				main_prg.comment_id_to_comment(result_id, output);

				

				free(output);
			}
			else
				printf("%s", "No result\n");

			delete []data_scores;
			delete []data_pos;

			free(words_list);
			free(word_ranks_list);
			free(word_relationships);
			cout << "\n-------------------------------------------------------------\n-------------------------------------------------------------\n\n";
			

			
		}
		
		}

		main_prg.clear();
	}
};

void test_all_round(const char* input)
{
	
	

	
	
		
		
		
		
		
		
		
		
		

		
		
		

		
			
		
}

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "simdcomp.h"

int compress_decompress_demo() {
  size_t k, N = 9999;
  uint32_t * datain = (uint32_t*)malloc(N * sizeof(uint32_t));
  uint8_t * buffer = (uint8_t*)malloc(N * sizeof(uint32_t) + N / SIMDBlockSize);
  uint32_t * backbuffer = (uint32_t*)malloc(N * sizeof(uint32_t));
  uint32_t b;

  for (k = 0; k < N; ++k){        
    datain[k] = k;
  }

  b = maxbits_length(datain, N);
  simdpack_length(datain, N, (__m128i *)buffer, b);
  simdunpack_length((const __m128i *)buffer, N, backbuffer, b);

  for (k = 0; k < N; ++k){
    if(datain[k] != backbuffer[k]) {
      printf("bug\n");
      return -1;
    }
  }
  return 0;
}

size_t compress(uint32_t * datain, size_t length, uint8_t * buffer, int &ignored) {
    uint32_t offset;
    uint8_t * initout;
    size_t k;
	
	
		while(length/SIMDBlockSize*SIMDBlockSize != length && length != 0)
		{
			
			length--;
		
			
			ignored++;
		}
	    
	

		if(length/SIMDBlockSize*SIMDBlockSize != length)
			printf("Data length should be a multiple of %i \n",SIMDBlockSize);

    offset = 0;
    initout = buffer;
	for(k = 0; k < length / SIMDBlockSize; ++k) {
        uint32_t b = simdmaxbitsd1(offset,
                    datain + k * SIMDBlockSize);
		*buffer++ = b;
		simdpackwithoutmaskd1(offset, datain + k * SIMDBlockSize, (__m128i *) buffer,
                    b);
        offset = datain[k * SIMDBlockSize + SIMDBlockSize - 1];
        buffer += b * sizeof(__m128i);
	}
	return buffer - initout;
}

void t()
{
	cout << "START\n";
	std::clock_t start = std::clock();
	string out;

	main_data_file.init();
	
	ifstream_continous_buffered in(main_data_file);

	while(in.read_a_line(out))
	{
		cout << "[" << out << "]";
		
		
	}
	
	main_data_file.kill();

	

	cout << "DONE " << ( std::clock() - start ) / (double) CLOCKS_PER_SEC << "\n";

}

char* random_string( size_t length )
{
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
	char* r = new char[length+1];
	strcpy(r, str.c_str());
    return r;
}
void performance_c(unsigned int item_num, unsigned int sort_based_on, bool desc)
{
	
	
	
	

		int values_size = 100000000;
		unsigned int *nums1 = new unsigned int[values_size];
		int *nums2 = new int[values_size];
		int *nums3 = new int[values_size];

		for(unsigned int i = 0; i < values_size; i++)
		{
			nums1[i] = 1.7*rand();
			nums2[i] = 65.8*rand();
			nums3[i] = 35.8*rand();
		}

		srand((unsigned)time(0));
		std::random_shuffle(nums1, nums1+values_size);
		
  
		std::clock_t start_s = std::clock();
		

		

		
	

	if(item_num == 3 && sort_based_on == 1)
	{
	
		sort_arrays.sort(nums1, nums2, nums3, values_size, desc);
	
	}
	else if(item_num == 3 && sort_based_on == 2)
	{
	
		sort_arrays.sort(nums2, nums1, nums3, values_size, desc);
	
	}
	else if(item_num == 3 && sort_based_on == 3)
	{
	
		sort_arrays.sort(nums3, nums1, nums2, values_size, desc);
	
	}
	else if(item_num == 2 && sort_based_on == 1)
	{
	
		sort_arrays.sort(nums1, nums2, values_size, desc);
	
	}
	else if(item_num == 2 && sort_based_on == 2)
	{
	
		sort_arrays.sort(nums2, nums1, values_size, desc);
	
	}
	else if(item_num == 1 && sort_based_on == 1)
	{
	
		sort_arrays.sort(nums1, values_size, desc);
	
	}
	else if(item_num == 0 && sort_based_on == 1)
	{
	
		sort_arrays.sort(nums2, values_size, desc);
	
	}
	else
		cout << "ERROR\n";
	
	cout << "DONE. TIME USED " << (std::clock()-start_s)/(double)CLOCKS_PER_SEC << " SECONDS.\n\n";

}

void run_c(unsigned int item_num, unsigned int sort_based_on, bool desc)
{
	int values_size = 6;
	int nums1[] = {5, 8, 7, 89, 56, 4};
	const char *nums2[] = {"two", "three", "four", "five", "six", "seven"};
	short nums3[] = {1, 4, 3, 2, 0, 239};

	

	if(item_num == 3 && sort_based_on == 1)
	{
		for(unsigned int i = 0; i < values_size; i++) cout << "nums1[" << i << "] " << nums1[i] << " | nums2[" << i << "] "  << nums2[i]  << " | nums3[" << i << "] "  << nums3[i]<< "\n"; cout << "\n";
		sort_arrays.sort(nums1, nums2, nums3, values_size, desc);
		cout << "OUTPUT\n\n"; for(unsigned int i = 0; i < values_size; i++) cout << "nums1[" << i << "] " << nums1[i] << " | nums2[" << i << "] "  << nums2[i]  << " | nums3[" << i << "] "  << nums3[i]<< "\n"; cout << "\n\n";
	}
	else if(item_num == 3 && sort_based_on == 2)
	{
		for(unsigned int i = 0; i < values_size; i++) cout << "nums2[" << i << "] " << nums2[i] << " | nums1[" << i << "] "  << nums1[i]  << " | nums3[" << i << "] "  << nums3[i]<< "\n"; cout << "\n";
		sort_arrays.sort(nums2, nums1, nums3, values_size, desc);
		cout << "OUTPUT\n\n"; for(unsigned int i = 0; i < values_size; i++) cout << "nums2[" << i << "] " << nums2[i] << " | nums1[" << i << "] "  << nums1[i]  << " | nums3[" << i << "] "  << nums3[i]<< "\n"; cout << "\n\n";
	}
	else if(item_num == 3 && sort_based_on == 3)
	{
		for(unsigned int i = 0; i < values_size; i++) cout << "nums3[" << i << "] " << nums3[i] << " | nums1[" << i << "] "  << nums1[i]  << " | nums2[" << i << "] "  << nums2[i]<< "\n"; cout << "\n";
		sort_arrays.sort(nums3, nums1, nums2, values_size, desc);
		cout << "OUTPUT\n\n"; for(unsigned int i = 0; i < values_size; i++) cout << "nums3[" << i << "] " << nums3[i] << " | nums1[" << i << "] "  << nums1[i]  << " | nums2[" << i << "] "  << nums2[i]<< "\n"; cout << "\n\n";
	}
	else if(item_num == 2 && sort_based_on == 1)
	{
		for(unsigned int i = 0; i < values_size; i++) cout << "nums1[" << i << "] " << nums1[i] << " | nums2[" << i << "] "  << nums2[i] << "\n"; cout << "\n";
		sort_arrays.sort(nums1, nums2, values_size, desc);
		cout << "OUTPUT\n"; for(unsigned int i = 0; i < values_size; i++) cout << "nums1[" << i << "] " << nums1[i] << " | nums2[" << i << "] "  << nums2[i] << "\n"; cout << "\n\n";
	}
	else if(item_num == 2 && sort_based_on == 2)
	{
		for(unsigned int i = 0; i < values_size; i++) cout << "nums2[" << i << "] " << nums2[i] << " | nums1[" << i << "] "  << nums1[i] << "\n"; cout << "\n";
		sort_arrays.sort(nums2, nums1, values_size, desc);
		cout << "OUTPUT\n\n"; for(unsigned int i = 0; i < values_size; i++) cout << "nums2[" << i << "] " << nums2[i] << " | nums1[" << i << "] "  << nums1[i] << "\n"; cout << "\n\n";
	}
	else if(item_num == 1 && sort_based_on == 1)
	{
		for(unsigned int i = 0; i < values_size; i++) cout << "nums1[" << i << "] " << nums1[i] << "\n"; cout << "\n";
		sort_arrays.sort(nums1, values_size, desc);
		cout << "OUTPUT\n\n"; for(unsigned int i = 0; i < values_size; i++) cout << "nums1[" << i << "] " << nums1[i] << "\n"; cout << "\n\n";
	}
	else if(item_num == 0 && sort_based_on == 1)
	{
		for(unsigned int i = 0; i < values_size; i++) cout << "nums1[" << i << "] " << nums2[i] << "\n"; cout << "\n";
		sort_arrays.sort(nums2, values_size, desc);
		cout << "OUTPUT\n\n"; for(unsigned int i = 0; i < values_size; i++) cout << "nums1[" << i << "] " << nums2[i] << "\n"; cout << "\n\n";
	}
	else
		cout << "ERROR\n";
}

void xz()
{
	threaded_bg_line_reader t_data;
	string *d, *p = NULL;

	t_data.read_next_line(d);

	cout << *d << "\n";

	unsigned int i = 0;

	while(t_data.read_next_line(d))
	{
		i++;
		p = d;
	}
	
	cout << *p << "\n";
}

int main(int argc, const char* argv[])
{
	const char* input = argc == 2 ? argv[1] : "reddit is great!";
	
#ifdef INDEX_CREATION
#ifdef PROGRAM_RUN
	cout << "Multiple modes are not allowed, select one.\n";
	return 0;
#endif
#endif

#ifdef DICTIONARY_CREATION
#ifdef PROGRAM_RUN
	cout << "Multiple modes are not allowed, select one.\n";
	return 0;
#endif
#endif

#ifdef INDEX_CREATION
#ifdef DICTIONARY_CREATION
	cout << "Multiple modes are not allowed, select one.\n";
	return 0;
#endif
#endif

	word_simplifier_setup(get_first_result_else_shorter_const, kill_word_outside_of_dictionary_const);

#ifdef DICTIONARY_CREATION
	cout << "UN-DEFINE DICTIONARY_CREATION AND START THE PROGRAM AGAIN.\n";
	cin.get();
#endif

#ifdef INDEX_CREATION
	std::clock_t start = std::clock();
	cout << "INDEX CREATION STARTED.\n";

#ifdef NEW_INDEX
#ifndef OLD_INDEX
	index_creator create_index;
	create_index.run();
#endif
#endif

#ifdef OLD_INDEX
#ifndef NEW_INDEX
	run();
#endif
#endif

	cout << "DONE. TIME USED " << (std::clock()-start)/(double)CLOCKS_PER_SEC << " SECONDS.\n\n";
	cin.get();
#endif

#ifdef PROGRAM_RUN
	test_all_round(input);
#endif

	return 0;
}
