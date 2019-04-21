#pragma once

#include <iostream>
#include <algorithm>
#include <queue>
#include <list>
#include <exception>

class InvalidInterval 
	: public std::exception
{
public:
	virtual const char* what() const override
	{
		return "Invalid interval [low > high]";
	}
};

class Interval
{
public:
	Interval()
		: m_low(0)
		, m_high(0)
	{}

	Interval(int l, int h)
		: Interval()
	{
		setInterval(l, h);
	}

	void setInterval(int low, int high)
	{
		if (low > high) {
			throw InvalidInterval();
		}
		m_low = low;
		m_high = high;
	}

	bool isOverlap(const Interval& i) const
	{
		return (m_low >= i.m_low && m_low >= i.m_high) ||
		       (m_high >= i.m_low && m_high >= i.m_high);
	}

	bool isEqual(const Interval& i) const
	{
		return m_low == i.m_low && m_high == i.m_high;
	}

	int getLow() const
	{
		return m_low;
	}

	int getHigh() const
	{
		return m_high;
	}
private:
	int m_low;
	int m_high;
};

class IntervalNode
{
public:
	IntervalNode(const Interval& i)
		: m_left(0)
		, m_right(0)
		, m_interval(i)
		, m_height(0)
		, m_max_high(i.getHigh())
	{}

	~IntervalNode()
	{
		delete m_left;
		delete m_right;
	}
private:
	IntervalNode* m_left;
	IntervalNode* m_right;
	Interval m_interval;
	int m_height;
	int m_max_high;
	friend class IntervalTree;
};

//IntervalTree implementation based on AVL balanced tree
class IntervalTree
{
public:
	IntervalTree()
		: m_root(0)
	{}
	
	~IntervalTree()
	{
		delete m_root;
	}

	void insert(const Interval& i)
	{
		insertHelper(m_root, i);
	}

	void printInorder() const
	{
		std::cout << "In-order:\n";
		printHelper(m_root);
	}

	void printLevelorder() const
	{
		std::cout << "Level-order:\n";
		std::queue<const IntervalNode*> q;
		q.push(m_root);
		while (!q.empty()) {
			if (const IntervalNode* n = q.front()) {
				q.push(n->m_left);
				q.push(n->m_right);
				print(n);
			}
			q.pop();
		}
	}

	const Interval* searchOverlap(const Interval& i) const
	{
		auto pred = [](const Interval& i1, const Interval& i2) {
			return i1.isOverlap(i2);
		};
		if (const IntervalNode* found = searchNode(m_root, i, pred)) {
			return &(found->m_interval);
		}
		return 0;
	}

	void remove(const Interval& i)
	{
		removeHelper(m_root, i);
	}

	bool search(const Interval& i)
	{
		auto pred = [](const Interval& i1, const Interval& i2) { 
			return i1.isEqual(i2); 
		};
		return 0 != searchNode(m_root, i, pred);
	}
private:
	//Helper functions
	static IntervalNode* max(IntervalNode* root)
	{
		if (0 == root) {
			return 0;
		}
		while (0 != root->m_right) {
			root = root->m_right;
		}
		return root;
	}

	static IntervalNode* maxRecursive(IntervalNode* root)
	{
		if (0 == root) {
			return 0;
		}
		return 0 == root->m_right ? root : maxRecursive(root->m_right);
	}

	static IntervalNode* min(IntervalNode* root)
	{
		if (0 == root) {
			return 0;
		}
		while (0 != root->m_left) {
			root = root->m_left;
		}
		return root;
	}

	static IntervalNode* minRecursive(IntervalNode* root)
	{
		if (0 == root) {
			return 0;
		}
		return 0 == root->m_left ? root : minRecursive(root->m_left);
	}

	static void printHelper(const IntervalNode* root)
	{
		if (0 != root) {
			printHelper(root->m_left);
			print(root);
			printHelper(root->m_right);
		}
	}

	static void print(const IntervalNode* root)
	{
		if (0 != root) {
			std::cout << "[" << root->m_interval.getLow() << ", "
				<< root->m_interval.getHigh() << "] - " << root->m_max_high << "\n";
		}
	}

	static void removeHelper(IntervalNode*& root, const Interval& i)
	{
		//TODO: implement
	}

	//Helper functions
	template <typename Predicate>
	static IntervalNode* searchNode(IntervalNode* root, const Interval& i, const Predicate& pred)
	{
		if (0 == root) {
			return 0;
		}
		if (pred(root->m_interval, i)) {
			return root;
		}
		if (root->m_interval.getLow() < i.getLow()) {
			return searchNode(root->m_right, i, pred);
		}
		return searchNode(root->m_left, i, pred);
	}

	static void insertHelper(IntervalNode*& root, const Interval& i)
	{
		if (0 == root) {
			root = new IntervalNode(i);
		} else if (root->m_interval.getLow() < i.getLow()) {
			insertHelper(root->m_right, i);
		} else if (root->m_interval.getLow() > i.getLow()) {
			insertHelper(root->m_left, i);
		} else {
			return;
		}
		//update root balance factor
		updateHeight(root);
		//update balance of the tree
		root = rebalance(root);
		//update max high
		updateMaxHigh(root, i);
	}

	static void updateMaxHigh(IntervalNode* root, const Interval& i)
	{
		if (root->m_max_high < i.getHigh()) {
			root->m_max_high = i.getHigh();
		}
	}

	static void updateHeight(IntervalNode* root)
	{
		if (0 != root) {
			root->m_height = 1 + std::max(getHeight(root->m_left),
						      getHeight(root->m_right));
		}
	}

	static IntervalNode* rebalance(IntervalNode* root)
	{
		int bfactor = getBlanaceFactor(root);
		if (bfactor == -2) {
			if (getBlanaceFactor(root->m_right) == 1) {
				root->m_right = RightRotate(root->m_right);
			}
			return leftRotate(root);
		}
		if (bfactor == 2) {
			if (getBlanaceFactor(root->m_left) == -1) {
				root->m_left = leftRotate(root->m_left);
			}
			return RightRotate(root);
		}
		return root;
	}

	static IntervalNode* leftRotate(IntervalNode* root)
	{
		if (0 != root) {
			IntervalNode* newRoot = root->m_right;
			IntervalNode* tmp = newRoot->m_left;
			newRoot->m_left = root;
			root->m_right = tmp;
			updateHeight(root);
			updateHeight(newRoot);
			return newRoot;
		}
		return 0;
	}

	static IntervalNode* RightRotate(IntervalNode* root)
	{
		if (0 != root) {
			IntervalNode* newRoot = root->m_left;
			IntervalNode* tmp = newRoot->m_right;
			newRoot->m_right = root;
			root->m_left = tmp;
			updateHeight(root);
			updateHeight(newRoot);
			return newRoot;
		}
		return 0;
	}

	static int getBlanaceFactor(const IntervalNode* root)
	{
		return 0 == root ? 0 : getHeight(root->m_left) - getHeight(root->m_right);
	}

	static int getHeight(const IntervalNode* root)
	{
		return 0 == root ? 0 : root->m_height;
	}
private:
	IntervalNode* m_root;
};