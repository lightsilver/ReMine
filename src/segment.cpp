#include "utils/parameters.h"
#include "utils/commandline_flags.h"
#include "utils/utils.h"
#include "frequent_pattern_mining/frequent_pattern_mining.h"
#include "data/documents.h"
#include "classification/feature_extraction.h"
#include "classification/label_generation.h"
#include "classification/predict_quality.h"
#include "model_training/segmentation.h"
#include "data/dump.h"

using FrequentPatternMining::Pattern;
using FrequentPatternMining::patterns;

vector<double> f;
vector<int> pre;

void process(const vector<TOTAL_TOKENS_TYPE>& tokens, const vector<POS_ID_TYPE>& tags, Segmentation& segmenter, FILE* out)
{
    if (ENABLE_POS_TAGGING) {
        segmenter.viterbi(tokens, tags, f, pre);
    } else {
        segmenter.viterbi(tokens, f, pre);
    }

    int i = (int)tokens.size();
    assert(f[i] > -1e80);
    vector<string> ret;
    while (i > 0) {
        int j = pre[i];
        size_t u = 0;
        bool quality = true;
        for (int k = j; k < i; ++ k) {
            if (!trie[u].children.count(tokens[k])) {
                quality = false;
                break;
            }
            u = trie[u].children[tokens[k]];
        }
        quality &= trie[u].id >= 0 && trie[u].id < SEGMENT_QUALITY_TOP_K;


        if (quality) {
            ret.push_back("</"+trie[u].indicator+">");
            //ret.push_back(to_string(patterns[trie[u].id].quality));
            //cerr<<patterns[trie[u].id].tokens[0]<<" "<<tokens[j]<<endl;
            //ret.push_back(to_string(patterns[trie[u].id].postagquality));
        }

        if (true) {
            for (int k = i - 1; k >= j; -- k) {
                ostringstream sout;
                sout << tokens[k];  
                //ret.push_back(Documents::posid2Tag[int(tags[k])]);
                ret.push_back(sout.str());
            }
        }
        
        if (quality) {
            ret.push_back("<"+trie[u].indicator+">");
        }

        i = j;
    }

    reverse(ret.begin(), ret.end());
    for (int i = 0; i < ret.size(); ++ i) {
        fprintf(out, "%s%c", ret[i].c_str(), i + 1 == ret.size() ? '\n' : ' ');
    }
}

inline bool byQuality(const Pattern& a, const Pattern& b)
{
    return a.quality > b.quality + EPS || fabs(a.quality - b.quality) < EPS && a.currentFreq > b.currentFreq;
}

int main(int argc, char* argv[])
{
    parseCommandFlags(argc, argv);

    sscanf(argv[1], "%d", &NTHREADS);
    omp_set_num_threads(NTHREADS);

    Dump::loadSegmentationModel(SEGMENTATION_MODEL);
    
    //for (int i=0;i<patterns.size();++i)
    //    cerr<<"check:"<<patterns[i].postagquality<<endl;

    //Output ranking List
    //cerr<<"Checkpoint"<<endl;
    //Dump::dumpResults("tmp_ranklist/ori_final_quality");

    //return 0;

    sort(patterns.begin(), patterns.end(), byQuality);

    //for (int i=0;i<patterns.size();++i)
    //    cerr<<"check:"<<patterns[i].postagquality<<endl;

    constructTrie(); // update the current frequent enough patterns

    Segmentation* segmenter;
    if (ENABLE_POS_TAGGING) {
        segmenter = new Segmentation(ENABLE_POS_TAGGING);
        Segmentation::getDisconnect();
        Segmentation::logPosTags();
    } else {
        segmenter = new Segmentation(Segmentation::penalty);
    }

    char currentTag[100];

    FILE* in = tryOpen(TEXT_TO_SEG_FILE, "r");
    FILE* posIn = NULL;
    if (ENABLE_POS_TAGGING) {
        posIn = tryOpen(TEXT_TO_SEG_POS_TAGS_FILE, "r");
    }

    FILE* out = tryOpen("tmp_remine/tokenized_segmented_sentences.txt", "w");

    while (getLine(in)) {
        stringstream sin(line);
        vector<TOTAL_TOKENS_TYPE> tokens;
        vector<POS_ID_TYPE> tags;

        string lastPunc = "";
        for (string temp; sin >> temp;) {
            // get pos tag
            POS_ID_TYPE posTagId = -1;
            if (ENABLE_POS_TAGGING) {
                myAssert(fscanf(posIn, "%s", currentTag) == 1, "POS file doesn't have enough POS tags");
                if (!Documents::posTag2id.count(currentTag)) {
                    posTagId = -1; // unknown tag
                } else {
                    posTagId = Documents::posTag2id[currentTag];
                }
            }

            // get token
            bool flag = true;
            TOKEN_ID_TYPE token = 0;
            for (int i = 0; i < temp.size() && flag; ++ i) {
                flag &= isdigit(temp[i]) || i == 0 && temp.size() > 1 && temp[0] == '-';
            }
            stringstream sin(temp);
            sin >> token;

            if (!flag) {
                string punc = temp;
                if (Documents::separatePunc.count(punc)) {
                    process(tokens, tags, *segmenter, out);
                    tokens.clear();
                    tags.clear();
                }
            } else {
                tokens.push_back(token);
                if (ENABLE_POS_TAGGING) {
                    tags.push_back(posTagId);
                }
            }
        }
        if (tokens.size() > 0) {
            process(tokens, tags, *segmenter, out);
        }
    }
    fclose(in);
    if (ENABLE_POS_TAGGING) {
        fclose(posIn);
    }
    fclose(out);

    return 0;
}