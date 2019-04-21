#include "interval_tree.hpp"

int main()
{
	IntervalTree t;
	t.insert(Interval(0, 3));
	t.insert(Interval(6, 10));
	t.insert(Interval(5, 8));
	t.insert(Interval(19, 20));
	t.insert(Interval(17, 60));
	t.insert(Interval(26, 26));
	t.insert(Interval(8, 9));
	t.insert(Interval(16, 21));
	t.insert(Interval(25, 30));
	t.insert(Interval(15, 23));
	t.printInorder();
	t.printLevelorder();
	std::list<const Interval*> overlaps;
	t.collectOverlaps(Interval(2, 17), overlaps);
	std::cout << "\n[2, 17] overlaps:\n";
	for (const Interval* i : overlaps) {
		std::cout << "[" << i->getLow() << ", " << i->getHigh() << "]\n";
	}
	std::cout << "\n";

	if (const Interval* i = t.searchOverlap(Interval(4, 9))) {
		std::cout << "[4, 9] overlapped " << "[" << i->getLow() << ", " << i->getHigh() << "]\n";
	}
	std::cout << t.search(Interval(4, 9)) << " " << t.search(Interval(8, 9)) << "\n";
	t.remove(Interval(25, 30));
	t.remove(Interval(17, 60));
	t.printLevelorder();
}