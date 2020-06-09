#ifndef _SENSITIVE_WORDS_H_
#define _SENSITIVE_WORDS_H_

#include <string>

namespace advertisement
{
	struct Node
	{
		char data;
		bool last;
		Node* sibling;
		Node* next;
	};

	class SensitiveWordsLib
	{
	public:
		static const char* keywords;
	public:
		static const SensitiveWordsLib& GetInstance();
		bool Match(const std::string& text) const;
		
	protected:
		SensitiveWordsLib();
		~SensitiveWordsLib();
		
	private:	
		Node* root;
	};
}

#endif
