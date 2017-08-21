#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "../utils/utils.h"

typedef char PATTERN_LEN_TYPE;
typedef unsigned char POS_ID_TYPE;
typedef int TOTAL_TOKENS_TYPE;
typedef int PATTERN_ID_TYPE;
typedef int TOKEN_ID_TYPE;
typedef unsigned long long ULL;
typedef int INDEX_TYPE; // sentence id
typedef short int POSITION_INDEX_TYPE; // position inside a sentence

/*
const string TRAIN_FILE = "tmp/tokenized_train.txt";
const string TEST_FILE = "tmp/tokenized_test.txt";
const string TRAIN_CAPITAL_FILE = "tmp/case_tokenized_train.txt";
const string STOPWORDS_FILE = "tmp/tokenized_stopwords.txt";
const string ALL_FILE = "tmp/tokenized_all.txt";
const string QUALITY_FILE = "tmp/tokenized_quality.txt";
const string QUALITY_FILE_TAG = "data/remine/nyt_tmp.txt";
const string POS_TAGS_FILE = "tmp/pos_tags_tokenized_train.txt";
const string TEXT_TO_SEG_FILE = "tmp/tokenized_text_to_seg.txt";
const string TEXT_TO_SEG_POS_TAGS_FILE = "tmp/pos_tags_tokenized_text_to_seg.txt";
*/

const string TRAIN_FILE = "tmp_remine/tokenized_train.txt";
const string TEST_FILE = "tmp_remine/tokenized_test.txt";
const string TRAIN_CAPITAL_FILE = "tmp_remine/case_tokenized_train.txt";
const string STOPWORDS_FILE = "tmp_remine/tokenized_stopwords.txt";
const string ALL_FILE = "tmp_remine/tokenized_quality.txt";
//const string QUALITY_FILE = "tmp_remine/tokenized_quality.txt";
const string QUALITY_FILE = "tmp_remine/tokenized_quality.txt";
const string QUALITY_FILE_ENTITY = "tmp_remine/refine_postags_quality.txt";
//const string QUALITY_FILE = "tmp_remine/relation_token.txt";
const string QUALITY_FILE_RELATION = "tmp_remine/pos_relation_token.txt";
const string POS_TAGS_FILE = "tmp_remine/pos_tags_tokenized_train.txt";
const string TEXT_TO_SEG_FILE = "tmp_remine/tokenized_text_to_seg.txt";
const string TEXT_TO_SEG_POS_TAGS_FILE = "tmp_remine/pos_tags_tokenized_text_to_seg.txt";


const TOKEN_ID_TYPE BREAK = -911;

int ITERATIONS = 3;
int MIN_SUP = 30;
int MAX_LEN = 4;
int MAX_POSITIVE = 100;
int NEGATIVE_RATIO = 5;
int NTHREADS = 4;
int POSTAG_SCORE = 0;
bool ENABLE_POS_TAGGING = false;
bool ENABLE_POS_PRUNE = false;
string NO_EXPANSION_POS_FILENAME = "";
double DISCARD = 0.05;
string LABEL_FILE = "";
bool INTERMEDIATE = true;
string LABEL_METHOD = "ByLengthByPositive";
//string LABEL_METHOD = "ByRandom";
string SEGMENTATION_MODEL = "";
int SEGMENT_QUALITY_TOP_K = 50000;



#endif