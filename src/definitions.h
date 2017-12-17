#include "tbb/tbbmalloc_proxy.h"
#include <string>
#include <thread>
using namespace std;

//#define DICTIONARY_CREATION
//#define INDEX_CREATION
#define PROGRAM_RUN




#define NEW_INDEX
//#define OLD_INDEX




/*
	MODES:
	1. CREATE DICTIONARY: DEFINE DICTIONARY_CREATION
	2. CREATE INDEX (NEEDS DICTIONARY): DEFINE INDEX_CREATION
	3. RUN THE PROGRAM (NEEDS DICTIONARY AND INDEX): DEFINE PROGRAM_RUN

	You can only have ONE of those definitions each time
*/

#define DEF_base_work_directory "F:\\REDDIT DATABASE\\"//"\\\\Shadow-pc\\reddit database\\"//"U:\\REDDIT DATABASE\\"
//#define DEF_base_work_directory "C:\\test\\"
#define DEF_data_file_dictionary_directory "reddit_data\\english_dictionaries"
#define DEF_data_file_merged_directory "reddit_data\\index"//index_TILL_12_2009"///"reddit_data\\index"//index"

#define DEF_relationship_buffer_directory "relationship_buffer"


const unsigned int threads_number_supported = std::thread::hardware_concurrency();

#define THREADS_TO_USE threads_number_supported

//to change that you NEED TO REBUILD THE INDEX
#define WORD_RANK_MAX 250//65000//WORD_RANK_MAX*20.000 should be <= 2^32-1. don't set it bellow 255
#define AVG_WORD_RANK_TYPE unsigned char//short//this vairbale needs to be big enough to fit WORD_RANK_MAX

/*
	I think w should keep the 1st result (so 1st param true)
	I also think we shoudl NOT use the dictionary (so seond param false). NOT SURE ABOUT THIS
*/

const bool get_first_result_else_shorter_const = true;
const bool kill_word_outside_of_dictionary_const = false;

const string data_file_folder_list[] = 
{
	"reddit_data\\2007\\RC_2007-10", "reddit_data\\2007\\RC_2007-11", "reddit_data\\2007\\RC_2007-12",
	
	"reddit_data\\2008\\RC_2008-01", "reddit_data\\2008\\RC_2008-02", "reddit_data\\2008\\RC_2008-03", "reddit_data\\2008\\RC_2008-04", "reddit_data\\2008\\RC_2008-05", "reddit_data\\2008\\RC_2008-06",
	"reddit_data\\2008\\RC_2008-07", "reddit_data\\2008\\RC_2008-08", "reddit_data\\2008\\RC_2008-09", "reddit_data\\2008\\RC_2008-10", "reddit_data\\2008\\RC_2008-11", "reddit_data\\2008\\RC_2008-12",

	"reddit_data\\2009\\RC_2009-01", "reddit_data\\2009\\RC_2009-02", "reddit_data\\2009\\RC_2009-03", "reddit_data\\2009\\RC_2009-04", "reddit_data\\2009\\RC_2009-05", "reddit_data\\2009\\RC_2009-06",
	"reddit_data\\2009\\RC_2009-07", "reddit_data\\2009\\RC_2009-08", "reddit_data\\2009\\RC_2009-09", "reddit_data\\2009\\RC_2009-10", "reddit_data\\2009\\RC_2009-11", "reddit_data\\2009\\RC_2009-12",

	"reddit_data\\2010\\RC_2010-01", "reddit_data\\2010\\RC_2010-02", "reddit_data\\2010\\RC_2010-03", "reddit_data\\2010\\RC_2010-04", "reddit_data\\2010\\RC_2010-05", "reddit_data\\2010\\RC_2010-06",
	"reddit_data\\2010\\RC_2010-07", "reddit_data\\2010\\RC_2010-08", "reddit_data\\2010\\RC_2010-09", "reddit_data\\2010\\RC_2010-10", "reddit_data\\2010\\RC_2010-11", "reddit_data\\2010\\RC_2010-12",/*

	"reddit_data\\2011\\RC_2011-01", "reddit_data\\2011\\RC_2011-02", "reddit_data\\2011\\RC_2011-03", "reddit_data\\2011\\RC_2011-04", "reddit_data\\2011\\RC_2011-05", "reddit_data\\2011\\RC_2011-06",
	"reddit_data\\2011\\RC_2011-07", "reddit_data\\2011\\RC_2011-08", "reddit_data\\2011\\RC_2011-09", "reddit_data\\2011\\RC_2011-10", "reddit_data\\2011\\RC_2011-11", "reddit_data\\2011\\RC_2011-12",

	"reddit_data\\2012\\RC_2012-01", "reddit_data\\2012\\RC_2012-02", "reddit_data\\2012\\RC_2012-03", "reddit_data\\2012\\RC_2012-04", "reddit_data\\2012\\RC_2012-05", "reddit_data\\2012\\RC_2012-06",
	"reddit_data\\2012\\RC_2012-07", "reddit_data\\2012\\RC_2012-08", "reddit_data\\2012\\RC_2012-09", "reddit_data\\2012\\RC_2012-10", "reddit_data\\2012\\RC_2012-11", "reddit_data\\2012\\RC_2012-12",

	"reddit_data\\2013\\RC_2013-01", "reddit_data\\2013\\RC_2013-02", "reddit_data\\2013\\RC_2013-03", "reddit_data\\2013\\RC_2013-04", "reddit_data\\2013\\RC_2013-05", "reddit_data\\2013\\RC_2013-06",
	"reddit_data\\2013\\RC_2013-07", "reddit_data\\2013\\RC_2013-08", "reddit_data\\2013\\RC_2013-09", "reddit_data\\2013\\RC_2013-10", "reddit_data\\2013\\RC_2013-11", "reddit_data\\2013\\RC_2013-12",

	"reddit_data\\2014\\RC_2014-01", "reddit_data\\2014\\RC_2014-02", "reddit_data\\2014\\RC_2014-03", "reddit_data\\2014\\RC_2014-04", "reddit_data\\2014\\RC_2014-05", "reddit_data\\2014\\RC_2014-06",
	"reddit_data\\2014\\RC_2014-07", "reddit_data\\2014\\RC_2014-08", "reddit_data\\2014\\RC_2014-09", "reddit_data\\2014\\RC_2014-10", "reddit_data\\2014\\RC_2014-11", "reddit_data\\2014\\RC_2014-12",

	"reddit_data\\2015\\RC_2015-01", "reddit_data\\2015\\RC_2015-02", "reddit_data\\2015\\RC_2015-03", "reddit_data\\2015\\RC_2015-04", "reddit_data\\2015\\RC_2015-05"*/
};

const string data_file_list[] = 
{
	"RC_2007-10", "RC_2007-11", "RC_2007-12",

	"RC_2008-01", "RC_2008-02", "RC_2008-03", "RC_2008-04", "RC_2008-05", "RC_2008-06",
	"RC_2008-07", "RC_2008-08", "RC_2008-09", "RC_2008-10", "RC_2008-11", "RC_2008-12",

	"RC_2009-01", "RC_2009-02", "RC_2009-03", "RC_2009-04", "RC_2009-05", "RC_2009-06",
	"RC_2009-07", "RC_2009-08", "RC_2009-09", "RC_2009-10", "RC_2009-11", "RC_2009-12",

	"RC_2010-01", "RC_2010-02", "RC_2010-03", "RC_2010-04", "RC_2010-05", "RC_2010-06",
	"RC_2010-07", "RC_2010-08", "RC_2010-09", "RC_2010-10", "RC_2010-11", "RC_2010-12",/*

	"RC_2011-01", "RC_2011-02", "RC_2011-03", "RC_2011-04", "RC_2011-05", "RC_2011-06",
	"RC_2011-07", "RC_2011-08", "RC_2011-09", "RC_2011-10", "RC_2011-11", "RC_2011-12",

	"RC_2012-01", "RC_2012-02", "RC_2012-03", "RC_2012-04", "RC_2012-05", "RC_2012-06",
	"RC_2012-07", "RC_2012-08", "RC_2012-09", "RC_2012-10", "RC_2012-11", "RC_2012-12",

	"RC_2013-01", "RC_2013-02", "RC_2013-03", "RC_2013-04", "RC_2013-05", "RC_2013-06",
	"RC_2013-07", "RC_2013-08", "RC_2013-09", "RC_2013-10", "RC_2013-11", "RC_2013-12",

	"RC_2014-01", "RC_2014-02", "RC_2014-03", "RC_2014-04", "RC_2014-05", "RC_2014-06",
	"RC_2014-07", "RC_2014-08", "RC_2014-09", "RC_2014-10", "RC_2014-11", "RC_2014-12",

	"RC_2015-01", "RC_2015-02", "RC_2015-03", "RC_2015-04", "RC_2015-05"*/
};

const int items_parts_total_number = sizeof(data_file_folder_list)/sizeof(data_file_folder_list[0]);
