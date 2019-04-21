#include "interval_tree.hpp"

int main()
{
	IntervalTree t;
	t.insert(Interval(0, 3));
	t.insert(Interval(6, 10));
	t.insert(Interval(5, 8));
	t.insert(Interval(19, 20));
	t.insert(Interval(17, 19));
	t.insert(Interval(26, 26));
	t.insert(Interval(8, 9));
	t.insert(Interval(16, 21));
	t.insert(Interval(25, 30));
	t.insert(Interval(15, 23));
	t.printInorder();
	t.printLevelorder();

	if (const Interval* i = t.searchOverlap(Interval(4, 9))) {
		std::cout << "[4, 9] overlapped " << "[" << i->getLow() << ", " << i->getHigh() << "]\n";
	}
}