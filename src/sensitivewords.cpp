#include "sensitivewords.h"
#include<list>
#include<cstring>
using namespace std;
using namespace advertisement;

// thanks to: https://blog.csdn.net/nightwizard2030/article/details/70544697

void GetKeyWordList(list<char*>& list_keyword){
	list<char*>::iterator it_keyword;
	const char* keywords = SensitiveWordsLib::keywords;
	
	const size_t len = strlen(keywords);
	char* content = new char[len+1];
	memcpy(content,keywords,len);
	content[len]=0;
	
	char* temp=strtok(content,"@");//delim is @
	char* temp2=0;
	while(temp!=0){
		bool exist=false;
		for(list<char*>::iterator it_keyword=list_keyword.begin();it_keyword!=list_keyword.end();++it_keyword){
			if(strcmp(*it_keyword,temp)==0){
				exist=true;
				break;
			}
		}
		if(!exist){//push keyword to list except the repeated keyword
			temp2=new char[strlen(temp)+1];
			memcpy(temp2,temp,strlen(temp));
			temp2[strlen(temp)]=0;
			list_keyword.push_back(temp2);
		}
		temp=strtok(0,"@");
	}
	
	delete[] content;
}
 
void ReleaseKeyWordList(list<char*>& list_keyword){
	for(list<char*>::iterator it_keyword=list_keyword.begin();it_keyword!=list_keyword.end();++it_keyword){
		delete *it_keyword;
	}
}
 
Node* NodeHasExist(Node* cur,char data){
	while(cur!=0){
		if(cur->data==data){
			return cur;
		}
		cur=cur->sibling;
	}
	return 0;
}
 
 
Node* BuildTree(){
	list<char*> list_keyword;
	
	GetKeyWordList(list_keyword);
	
	Node* root=new Node();//make keyword tree by list
	root->next=0;
	root->sibling=0;
	root->data=0;
	Node* cur=root;
	for(list<char*>::iterator it_keyword=list_keyword.begin();it_keyword!=list_keyword.end();++it_keyword){//traversal list 
		int length=strlen(*it_keyword);
		for(int i=0;i<length;i++){//visit each char
			
			Node* node=NodeHasExist(cur->next,(*it_keyword)[i]);//check char node does exist in cur->next link list
			
			if(node==0){
				node=new Node();
				node->data=(*it_keyword)[i];
				node->last=false;
				node->next=0;
				node->sibling=0;
				if(cur->next==0){
					cur->next=node;//make node become cur->next's first
				}
				else{
					Node* temp=cur->next;
					while(temp->sibling!=0){//make point move to cur->next's last
						temp=temp->sibling;
					}
					temp->sibling=node;//add node at cur-next's last
				}			
			}
			
			//if node exist,skip create node again
 
			cur=node;//make cur point at node 
		}
		cur->last=true;//if is the end of word,set sign at the last,because it may has many branches
		cur=root;
	}
	
	ReleaseKeyWordList(list_keyword);
	
	return root;
}
 
void ReleaseTree(Node* root){//post traversal delete
	if(root!=0){
		ReleaseTree(root->next);
		ReleaseTree(root->sibling); 
		delete root;
	}
}                 
	                   
bool LookUp(const char* content, const Node* root){
	const Node* cur=root->next;
	int start=-1;
	bool shouldReview=false;
	for(int i=0;i<strlen(content);i++){//traversal content
	
		bool hasfind=false;//if hasfind,find next char
		
		char ch=content[i];
		
		while(cur!=0){
			if(cur->data==ch){
				hasfind=true;
				if(start==-1){//first time find char ,record first time index
					start=i;
				}
				if(cur->last&&cur->next==0){//it has arrived at last branch
					start=-1;
					cur=root->next;
					shouldReview=false; 
					return true;
				}
				else if(cur->last){//it may not arrived at last branch
					start=i+1;
					cur=cur->next;
					shouldReview=true;
					return true;
				}
				else{
					cur=cur->next;
				}
				break;
			}
			cur=cur->sibling;
		} // end while loop
		
		if(shouldReview&&!hasfind){
			i=start-1;//this time is checking cur->next,it should also check root->next,start-1 because of i++
			shouldReview=false;
			start=-1;
			cur=root->next;
		}
		else if(!hasfind){
			if(start>-1){
				i=start;//start must have checked with root->next,so it was not used to be -1
			}
			start=-1;
			cur=root->next;
		}
	} // end for loop
	
	return false;
}

const SensitiveWordsLib& SensitiveWordsLib::GetInstance()
{
	static SensitiveWordsLib singleton;
	return singleton;
}

bool SensitiveWordsLib::Match(const std::string& text) const
{
	return LookUp(text.c_str(),root);
}
	
SensitiveWordsLib::SensitiveWordsLib()
{
	root = BuildTree();
}

SensitiveWordsLib::~SensitiveWordsLib()
{
	ReleaseTree(root);
}
	
