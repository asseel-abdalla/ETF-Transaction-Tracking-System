#ifndef _HISTORY_HPP_
#define _HISTORY_HPP_

#include <iomanip>
#include <iostream>
#include <string>

#include "Transaction.hpp"

class History {
public:
	History();
	~History();
	void insert(Transaction *p_new_trans);
	void read_history();
	void print();
	void sort_by_date();
	void update_acb_cgl();
	double compute_cgl(unsigned int year);
	
	Transaction *get_p_head();

private:
	Transaction *p_head;
};

#endif
