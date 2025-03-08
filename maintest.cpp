#include <iostream>
using namespace std;

class Node{
	public:
		int key;
		int value;
		Node* next;
		Node(int k, int v){
			key = k;
			value = v;
			next = nullptr;
		}
	
};

class HashTable{
	private:
		int capacity;
		int size;
		Node** array;
		int hash(int key){
			return key % capacity;
		}
	
	
	public:
		HashTable(int c){
				capacity = c;
				size = 0;
				array = new Node*[capacity];
				for (int i=0; i < capacity; i++){
					array[i] = nullptr;
			}
			
	}
	
	
	~HashTable(){
		for(int i = 0; i < capacity; i++){
			Node* cur = array[i];
			while (cur != nullptr)
			{
				Node* next = cur->next;
				delete cur;
				cur = next;
			}
			
		}
		delete[] array;	
	}
				



	void insert(int key, int value){
		int index = hash(key);
		Node* cur = array[index];
		while (cur != nullptr){
			if (cur->key == key)
			{
				cur->key = value;
				return;
			}
			cur = cur ->next;
		}
		
		Node* newNode = new Node(key, value);
		newNode->next = array[index];
		array[index] = newNode;
		size++;

	}

	int get(int key){
		int index = hash(key);
		Node* cur = array[index];
		while (cur != nullptr)
		{
			if (cur->key == key)
			{
				return cur->value;
				
			}
			cur = cur->next;
		}
		
		return -1;
	}



	void remove (int key){
		int index = hash(key);
		Node* cur = array[index];
		Node* prev = nullptr;
		while (cur != nullptr){
			if (cur -> key == key){
				if (prev == nullptr){
					
					array[index] = cur->next;
				}
				else{
				prev->next = cur->next;
				}
				delete cur;
				size--;
				return;
					
			}
			
			prev = cur;
			cur = cur->next;
			
			
		}
		
	  }	
	
};


int main(){


     HashTable ht(13);
	 
	 ht.insert(1,10);
	 ht.insert(7,20);
	 ht.insert(3,30);
	 ht.insert(11,110);
	 
	 std::cout << "This is a line with std::endl." << std::endl;
	 std::cout << ht.get(1) << std::endl;
	 std::cout << ht.get(7) << std::endl;
	 std::cout << ht.get(3) << std::endl;
	 std::cout << ht.get(11) << std::endl;
	 
	 
	 ht.remove(2);
	 
	 std::cout << ht.get(2) << std::endl;
	 
	 return 0;


}