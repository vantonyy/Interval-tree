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
	
	/// l		  h
	//  |-------------|
	//    |------------------------|
	///   l			       h
	bool isOverlap(const Interval& i) const
	{
		return m_low <= i.m_high && i.m_low <= m_high;
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
		return searchOverlap(m_root, i);
	}

	const bool collectOverlaps(const Interval& i, std::list<const Interval*>& overlaps) const
	{
		overlaps.clear();
		collectOverlaps(m_root, i, overlaps);
		return !overlaps.empty();
	}

	void remove(const Interval& i)
	{
		removeHelper(m_root, i);
	}

	bool search(const Interval& i)
	{
		return 0 != searchNode(m_root, i);
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
		if (0 == root) {
			return;
		}
		if (root->m_interval.isEqual(i)) {
			if (0 == root->m_left) {
				IntervalNode* newRoot = root->m_right;
				root->m_right = 0;
				delete root;
				root = newRoot;
			}
			else if (0 == root->m_right) {
				IntervalNode* newRoot = root->m_left;
				root->m_left = 0;
				delete root;
				root = newRoot;
			}
			else {
				IntervalNode* minNode = min(root->m_right);
				root->m_interval = minNode->m_interval;
				removeHelper(root->m_right, minNode->m_interval);
				updateMaxHigh(root);
			}
		}
		else if (root->m_interval.getLow() < i.getLow()) {
			removeHelper(root->m_right, i);
		}
		else {
			removeHelper(root->m_left, i);
		}
		adjustTree(root);
	}

	static IntervalNode*& searchNode(IntervalNode* root, const Interval& i)
	{
		if (0 == root) {
			return root;
		}
		if (i.isEqual(root->m_interval)) {
			return root;
		}
		if (root->m_interval.getLow() < i.getLow()) {
			return searchNode(root->m_right, i);
		}
		return searchNode(root->m_left, i);
	}

	static const Interval* searchOverlap(const IntervalNode* root, const Interval& i)
	{
		if (0 == root) {
			return 0;
		}
		if (i.isOverlap(root->m_interval)) {
			return &root->m_interval;
		}
		if (0 != root->m_left && root->m_left->m_max_high >= i.getLow()) {
			return searchOverlap(root->m_left, i);
		}
		return searchOverlap(root->m_right, i);
	}

	static void collectOverlaps(const IntervalNode* root, const Interval& i, std::list<const Interval*>& overlaps)
	{
		if (0 == root) {
			return;
		}
		if (i.isOverlap(root->m_interval)) {
			overlaps.push_back(&root->m_interval);
		}
		collectOverlaps(root->m_left, i, overlaps);
		collectOverlaps(root->m_right, i, overlaps);
	}

	static void insertHelper(IntervalNode*& root, const Interval& i)
	{
		if (0 == root) {
			root = new IntervalNode(i);
		}
		else if (root->m_interval.getLow() < i.getLow()) {
			insertHelper(root->m_right, i);
		}
		else if (root->m_interval.getLow() > i.getLow()) {
			insertHelper(root->m_left, i);
		}
		else {
			return;
		}
		adjustTree(root);
		//update max high
		updateMaxHigh(root, i.getHigh());
	}

	static void updateMaxHigh(IntervalNode* root, int maxHigh)
	{
		if (root->m_max_high < maxHigh) {
			root->m_max_high = maxHigh;
		}
	}

	static int getMaxInterval(const IntervalNode* l, const IntervalNode* r)
	{
		if (0 == r) {
			return 0 == l ? 0 : l->m_max_high;
		}
		if (0 == l) {
			return 0 == r ? 0 : r->m_max_high;
		}
		return l->m_max_high > r->m_max_high ? l->m_max_high : r->m_max_high;
	}

	static void updateHeight(IntervalNode* root)
	{
		if (0 != root) {
			root->m_height = 1 + std::max(getHeight(root->m_left),
				getHeight(root->m_right));
		}
	}

	static void adjustTree(IntervalNode*& root)
	{
		//update root balance factor
		updateHeight(root);
		//update balance of the tree
		root = rebalance(root);
	}

	static void updateMaxHigh(IntervalNode* root)
	{
		if (0 == root) {
			return;
		}
		if (0 == root->m_left && 0 == root->m_right) {
			root->m_max_high = root->m_interval.getHigh();
		}
		else {
			updateMaxHigh(root->m_left);
			updateMaxHigh(root->m_right);
			//update max high
			int max = getMaxInterval(root->m_left, root->m_right);
			if (root->m_interval.getHigh() < max) {
				root->m_max_high = max;
			}
		}
	}

	static IntervalNode* rebalance(IntervalNode* root)
	{
		int bfactor = getBlanaceFactor(root);
		if (bfactor == -2) {
			if (getBlanaceFactor(root->m_right) == 1) {
				root->m_right = rightRotate(root->m_right);
			}
			return leftRotate(root);
		}
		if (bfactor == 2) {
			if (getBlanaceFactor(root->m_left) == -1) {
				root->m_left = leftRotate(root->m_left);
			}
			return rightRotate(root);
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

	static IntervalNode* rightRotate(IntervalNode* root)
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