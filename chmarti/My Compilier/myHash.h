#include "classes.h"

//The linked list template... my best friend
//With large amounts of data arrays are much faster
template <typename T>
class linkedlist
{	protected:
		struct link
			{	T data;
				link* next;	
				link* prev;	};
		link* contents;
		link* poslist;
		bool rev;
		bool setrev;
	public:
        linkedlist<T>(void)
            {	contents = NULL; 
				poslist = NULL;
				rev = 0;
				setrev = 0;	}
		void add(T x)
			{	link* n = new link;
				n->next = contents;
				n->data = x;
				contents = n;	}
		void startscan(bool b = 0)	
			{	if(b && !setrev)
				{	//printf("Doing it in Linked List!");
					rev = b;
					setrev = 1;
					poslist = contents;
					link* before;
					before = NULL;
					while(anyleft())
					{	poslist->prev = before;
						before = poslist;
						poslist = poslist->next;	}	
					poslist = before;}
				else
					poslist = contents;	}
		bool anyleft(void)	
			{	return poslist != NULL;	}
		T next(void)
			{	T ans = poslist->data;
				if(rev)
					poslist = poslist->prev;
				else
					poslist = poslist->next;
				return ans;		}	};

template <typename H>
class HashTable
{	protected:
		int size, num, avglistlen;
		linkedlist<H>* start;
        string (*hashf) ( H );

		int HashFunction(string s)
		{	int key = 0, a = 127, len = s.length();
			for(int i = 0; i < s.length(); i++)
				key = (a * key + s[i] ) % size;
			//cout << "key: " << key << endl;
			return key;	}

	public: 
        HashTable(int s, string (*f) ( H ))
		{	size = s;
            hashf = f;
			num = 0;
			start = new linkedlist<H>[size];	}

		void add(H thing)
		{	start[HashFunction((* hashf)(thing))].add(thing);
			num++;	
			avglistlen = num / size;	}
		
        H find(string thing)
        {   linkedlist<H> list = start[HashFunction(thing)];
			list.startscan();
			while(list.anyleft())
			{	H cur = list.next();
                if( (*hashf)( cur ) == thing)
    			    return cur;	} 
			return NULL;	}	};

//For the hash table
static string helper(IdentifierID *v) { return v->name; }

HashTable<IdentifierID*> symbolTable(20, helper);
vector<DecStmt*> symbolList;

IdentifierID* addIfNew(string s)
{
    IdentifierID *id = symbolTable.find(s);
    if(id == NULL)
    {
        id = new IdentifierID(s);
        symbolTable.add(id);
    }
    return id;
}
