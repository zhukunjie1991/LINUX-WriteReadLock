#ifndef CYCLE_LIST_H
#define CYCLE_LIST_H

#include <list>

using namespace std;
//no using namespace

//modify list<T> for list<T*>
//for getNextElement will return NULL;
	
template <class T>
class CycleList
{
public:
	//intenal data structure
	list<T*> m_list;
	//the current iterator
	typename std::list<T*>::iterator m_itor; 

public:
	CycleList();
	~CycleList();
public:
	//get the next itor
	T* getNextElement();
	T* getCurElement();
	//remove the specify itor:
	void remove(T* t);
	//clear
	void clear();
	//add anelement;
	void add(T* t);
};
template<class T> 
CycleList<T>::CycleList()
{
	m_itor = m_list.begin();
}

template<class T> 
CycleList<T>::~CycleList()
{
}

//get the next itor
template<class T> 
T* CycleList<T>::getNextElement()
{
	//if no element;
	if(m_list.empty())
		return NULL;
	//if goto the end.goback to front.
#ifdef WIN32
	list<T*>::iterator a;
#else
	typename list <T*> ::iterator   a;
#endif
	a = m_list.end();
	if(m_itor == a)
	{
		m_itor = m_list.begin();
		return *m_itor;
	}
	//else ,goto next value;
	T* tmp = *m_itor;
	m_itor ++;
	return tmp;
}
//get the next itor
template<class T> 
T* CycleList<T>::getCurElement()
{
	return * m_itor;
}
//remove the specify itor:
template<class T> 
void CycleList<T>::remove(T* t)
{
	//if no element ,do nothing
	if(m_list.empty())
		return;
	//find where is t.
#ifdef WIN32
	list<T*>::iterator ite;
#else
	typename list <T*> ::iterator   ite;
#endif
	bool isInList = false;
	for (ite = m_list.begin(); ite != m_list.end(); ++ite)
	{		
		T* tmp = *ite;
		if (t == tmp)
		{
			//already in the list;
			isInList = true;
			break;
		}
	}

	//find the element
	if(isInList)
	{
		//remove the element
		//and also recalculate the current itor
		if(ite == m_itor)
		{
			m_itor++;
			//把当前节点置为开始那个节点。
			//m_itor = m_list.begin();
		}
		m_list.erase(ite);
		return;
	}
	else//cant find the element
	{
		//just return;
		return;
	}
}
//clear
template<class T> 
void CycleList<T>::clear()
{
	m_list.clear();
	m_itor = this->m_list.begin();
}

//add anelement;
template<class T> 
void CycleList<T>::add(T* t)
{

	//check whether the list is empty()
	if(m_list.empty())
	{	
		m_list.push_back(t);	
		m_itor = m_list.begin();
		return;
	}
	//m_list.push_back(t);

	//if the list is not empty,first check whether is exist
#ifdef WIN32
	list<T*>::iterator ite;
#else
	typename list <T*> ::iterator   ite;
#endif
	bool isAlreadyInList = false;
	for (ite = m_list.begin(); ite != m_list.end(); ++ite)
	{		
		T* tmp = *ite;
		if (t == tmp)
		{
			//already in the list;
			isAlreadyInList = true;
			break;
		}
	}
	if(!isAlreadyInList)
		m_list.push_back(t);

}
#endif
