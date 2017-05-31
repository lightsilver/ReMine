import json
import sys
import pickle
import re

class PreProcessor(object):
	"""docstring for PreProcessor"""
	def __init__(self, train_path):
		self.word_mapping=dict()
		#self.reverse_mapping=dict()
		self.word_cnt=1
		self.cache=[]
		self.punc=['.',',','"',"'",'?',':',';','-','!','(',')','``',"''"]
		if train_path:
			with open(train_path,'r') as IN:
				for line in IN:
					content=json.loads(line)
					self.cache.append(content)


	def inWordmapping(self,word):
		if word in self.punc:
			return word
		if word not in self.word_mapping:
			self.word_mapping[word]=self.word_cnt
			#self.reverse_mapping[self.word_cnt]=word
			self.word_cnt+=1
		return str(self.word_mapping[word])

	def inTrainmapping(self,word):
		if word in self.punc:
			return word
		if word in self.word_mapping:
			return str(self.word_mapping[word])
		else:
			return '-1111'
	
	def tokenize_train(self,out_path):
		CASE=open('case_'+out_path,'w')
		POS_tag=open('pos_tags_tokenized_train.txt','w')
		#ENTITIES=open('tokenized_quality.txt','w')
		ENTITIES_POS=open('postags_quality.txt','w')
		with open(out_path,'w') as OUT:
			for content in self.cache:
				assert(len(content['tokens'])==len(content['pos']))
				for w in content['tokens']:
					if len(w)==0:
						continue
					if w.isupper() or w in self.punc:
						CASE.write('3')
					elif w[0].isupper():
						CASE.write('1')
					elif w.isdigit():
						CASE.write('4')
					else:
						CASE.write('0')
					OUT.write(self.inWordmapping(w.lower())+' ')
				for p in content['pos']:
					POS_tag.write(p+'\n')
				for e in content['entityMentions']:
					tokens=[]
					#for t in content['tokens'][e['start']:e['end']]:
					#if u',' in content['tokens'][e[0]:e[1]]:
					#	continue
					for t in content['tokens'][e[0]:e[1]]:
						if t.lower() not in self.word_mapping:
							break
						else:
							tokens.append(str(self.word_mapping[t.lower()]))
					if len(tokens)==e[1]-e[0]:
						#ENTITIES.write(' '.join(tokens)+'\n')
						#ENTITIES_POS.write(' '.join(content['pos'][e['start']:e['end']])+'\n')
						ENTITIES_POS.write(' '.join(content['pos'][e[0]:e[1]])+'\n')
				OUT.write('\n')
				CASE.write('\n')
		#ENTITIES.close()
		ENTITIES_POS.close()
		CASE.close()
		POS_tag.close()
	
	def tokenize_stopwords(self,in_path,out_path):
		OUT=open(out_path,'w')
		with open(in_path,'r') as IN:
			for line in IN:
				OUT.write(self.inTrainmapping(line.replace('\n','').lower())+'\n')
		OUT.close()

	def tokenize_phrases(self):
		Phrases=open('tokenized_quality.txt','w')
		with open('distant_phrases_nyt.txt','r') as IN:
			for line in IN:
				result=''
				for t in line.strip().split(' '):
					if t.lower() in self.word_mapping:
						result+=str(self.word_mapping[t.lower()])+' '
					else:
						result=''
						break
					#Phrases.write(str(self.word_mapping[t.lower()])+' ')
				if len(result)>0:
					Phrases.write(result+'\n')

	def dump(self):
		with open('token_mapping.txt','w') as OUT:
			for k,v in self.word_mapping.iteritems():
				OUT.write(str(v)+'\t'+k.encode('ascii', 'ignore').decode('ascii')+'\n')
		pickle.dump(self.test_token, open('test_token.p','wb'))
		pickle.dump(self.test_word, open('test_word.p','wb'))

	def dump_raw(self,outpath):
		with open(outpath,'w') as OUT:
			for content in self.cache:
				OUT.write(' '.join(content['tokens'])+'\n')


	def load(self):
		self.test_token=pickle.load(open('tmp_remine/test_token.p','rb'))
		#print self.test_token
		self.test_word=pickle.load(open('tmp_remine/test_word.p','rb'))

	def load_dict(self):
		with open('token_mapping.txt','r') as IN:
			for line in IN:
				tmp=line.strip().split('\t')
				self.word_mapping[tmp[1]]=int(tmp[0])
	
	def tokenize_test(self,test_path,out_path):
		CASE=open('case_'+out_path,'w')
		POS_tag=open('pos_tags_tokenized_text_to_seg.txt','w')
		OUT=open(out_path,'w')
		TEXT=open('raw_text_to_seg.txt','w')
		self.test_token=[]
		self.test_word=[]
		self._punc=[]
		with open(test_path,'r') as IN:
			for line in IN:
				content=json.loads(line)
				assert(len(content['tokens'])==len(content['pos']))
				token=[]
				word=[]
				punc_pos=[]
				for w in content['tokens']:
					if w=="``" or w=="''":
						continue
					if len(w)==0:
						continue
					if w.isupper() or w in self.punc:
						CASE.write('3')
					elif w[0].isupper():
						CASE.write('1')
					elif w.isdigit():
						CASE.write('4')
					else:
						CASE.write('0')
					OUT.write(self.inTrainmapping(w.lower())+' ')
					if w not in self.punc:
						token.append(self.inTrainmapping(w.lower()))
						word.append(w)
					else:
						punc_pos.append((len(word),w))
					#if w != "``" and w!= "''": 
					#	TEXT.write(w+' ')
					#else:
					TEXT.write(w.encode('ascii', 'ignore').decode('ascii')+' ')
				for p in content['pos']:
					POS_tag.write(p+'\n')
				OUT.write('\n')
				CASE.write('\n')
				TEXT.write('\n')
				self.test_token.append(token)
				self.test_word.append(word)
				self._punc.append(punc_pos)
		OUT.close()
		POS_tag.close()
		CASE.close()
		TEXT.close()



	def mapBack(self,seg_path,outpath):
		queue=[]
		r_ptr=0
		c_ptr=0
		start=['<None>','<ENTITY>','<RELATION>']
		end=['</None>','</ENTITY>','</RELATION>']
		with open(seg_path,'r') as _seg, open(outpath,'w') as OUT:
			for line in _seg:
				for token in line.strip().split(' '):
					queue.append(token)
				#print queue
				while (len(queue)>0):
					#print c_ptr,r_ptr
					if queue[0] in start or queue[0] in end:
						#if queue[0] == '</phrase>' or c_ptr < len(self.test_token[r_ptr]):
						if queue[0] in start and c_ptr == len(self.test_token[r_ptr]):
							OUT.write('\n'+queue.pop(0)+' ')
							r_ptr+=1
							c_ptr=0
							continue
						else:
							OUT.write(queue.pop(0)+' ')
					elif c_ptr < len(self.test_token[r_ptr]) and queue[0] == self.test_token[r_ptr][c_ptr]:
						OUT.write(self.test_word[r_ptr][c_ptr].encode('ascii', 'ignore').decode('ascii')+' ')
						queue.pop(0)
						c_ptr+=1
					else:
						#print 'here'
						r_ptr+=1
						c_ptr=0
						OUT.write('\n')
				#break
					
		
if __name__ == '__main__':
	if sys.argv[1]=='translate':
		tmp=PreProcessor(sys.argv[2])
		tmp.tokenize_train(sys.argv[3])
		tmp.tokenize_test(sys.argv[4],sys.argv[5])
		tmp.tokenize_stopwords(sys.argv[6],sys.argv[7])
		tmp.tokenize_phrases()
		tmp.dump()
	elif sys.argv[1]=='segmentation':
		tmp=PreProcessor(None)
		tmp.load()
		tmp.mapBack(sys.argv[2],sys.argv[3])
	elif sys.argv[1]=='raw':
		tmp=PreProcessor(sys.argv[2])
		tmp.dump_raw(sys.argv[3])
	elif sys.argv[1]=='temp':
		tmp=PreProcessor(None)
		tmp.load_dict()
		tmp.tokenize_phrases()
