#include <cassert>
#include <iomanip>
#include <iostream>

#include "project4.hpp"
#include "History.hpp"
#include "Transaction.hpp"

////////////////////////////////////////////////////////////////////////////////
// Definitions for Transaction class
////////////////////////////////////////////////////////////////////////////////
//
//
// Transaction::Transaction( std::string ticker_symbol, unsigned int day_date, unsigned int
// month_date, unsigned year_date, bool buy_sell_trans, unsigned int number_shares,
// double trans_amount );
// ~Transaction();
// bool operator<( Transaction const &other );

// Constructor
// TODO
Transaction::Transaction( std::string ticker_symbol, unsigned int day_date, unsigned int
month_date, unsigned year_date, bool buy_sell_trans, unsigned int number_shares,
double trans_amount ) {
  symbol = ticker_symbol;
  day = day_date;
  month = month_date;
  year = year_date;
  if (buy_sell_trans) {
    trans_type = "Buy";
  }
  else {
    trans_type = "Sell";
  }
  shares = number_shares;
  amount = trans_amount;
  trans_id = assigned_trans_id++;
  acb = 0;
  acb_per_share = 0;
  cgl = 0;
  p_next = nullptr;

}

// Destructor
// TODO
Transaction::~Transaction() {
  
}

// Overloaded < operator.
// TODO
bool Transaction::operator<(Transaction const &other) {
    if (year < other.get_year()) {
        return true;
    } else if (year == other.get_year() && month < other.get_month()) {
        return true;
    } else if (year == other.get_year() && month == other.get_month() && day < other.get_day()) {
        return true;
    } else if (year == other.get_year() && month == other.get_month() && day == other.get_day() && trans_id < other.get_trans_id()) {
        return true;
    } else {
        return false;
    }
}

History::History() {
  p_head = nullptr;
}

	// Destructor
	History::~History() {
    Transaction *curr = p_head;
    Transaction *next = nullptr;
		while (curr != nullptr) {
      next = curr->get_next();
      delete curr;
      curr = next;
    }
	}
	// Insert transaction into linked list.
	void History::insert( Transaction *p_new_trans ) {
    if (p_head == nullptr) {
      p_head = p_new_trans;
    }
    else {
      Transaction *t = p_head;
      while (t->get_next() != nullptr) {
        t = t->get_next();
      }
      t->set_next(p_new_trans);
    }
	}

	// Read the transaction history from file.
	void History::read_history() {
    ece150::open_file();
    while (ece150::next_trans_entry()) {
      Transaction *new_trans = new Transaction {ece150::get_trans_symbol(), ece150::get_trans_day(), ece150::get_trans_month(), ece150::get_trans_year(), ece150::get_trans_type(), ece150::get_trans_shares(), ece150::get_trans_amount()};
      insert (new_trans);
    }
    ece150::close_file();
    
	}

	// Print the transaction history.
	void History::print() {
    Transaction *t = p_head;
    std::cout << "======== BEGIN TRANSACTION HISTORY ========" << std::endl;
    while (t != nullptr) {
      t->print();
      t = t->get_next();
    }
    std::cout << "======== END TRANSACTION HISTORY ========" << std::endl;
	}
	
	// Sort using insertion sort.
	void History::sort_by_date() {
    if (p_head == nullptr || p_head->get_next() == nullptr) { // if empty or one thing
      return;
    }

    Transaction *sorted_head = nullptr;
 
    while (p_head != nullptr) {
      Transaction *t_to_sort = p_head;
      p_head = p_head->get_next();

      if (sorted_head == nullptr || *t_to_sort < *sorted_head) {
        t_to_sort->set_next(sorted_head);
        sorted_head = t_to_sort;
      }
      else {
        Transaction *sorted_head_TEMP = sorted_head;
        Transaction *t_before = nullptr;

        while (sorted_head_TEMP != nullptr && *sorted_head_TEMP < *t_to_sort) {
          t_before = sorted_head_TEMP;
          sorted_head_TEMP = sorted_head_TEMP->get_next();
        }

        if (t_before == nullptr) {
          t_to_sort->set_next(sorted_head_TEMP);
          sorted_head = t_to_sort;
        }
        else {
          t_before->set_next(t_to_sort);
          t_to_sort->set_next(sorted_head_TEMP);
        }
      }
    }
    p_head = sorted_head;
  }
  


	// Update the acb members in the linked list.
	void History::update_acb_cgl() {
    sort_by_date();
    Transaction *t = p_head;

    if (t != nullptr) {
      Transaction *next = t->get_next();
      t->set_cgl(0);

      if (t->get_trans_type()) {
        t->set_acb(t->get_amount());
        t->set_acb_per_share(t->get_acb()/t->get_shares());
        t->set_share_balance(t->get_shares());
      }
      else {
        t->set_acb(0-t->get_amount());
        t->set_acb_per_share(0);
        t->set_share_balance(0-t->get_shares());
      }

      while (next != nullptr) {
        if (next->get_trans_type()) {
          next->set_share_balance(t->get_share_balance()+next->get_shares());
          next->set_acb(t->get_acb() + next->get_amount());
          next->set_cgl(0);
          next->set_acb_per_share(next->get_acb()/next->get_share_balance());
        }
        else {
           next->set_share_balance(t->get_share_balance()-next->get_shares());
          next->set_acb(t->get_acb()-next->get_shares()*t->get_acb_per_share());
          next->set_cgl(next->get_amount() - t->get_acb_per_share()*next->get_shares());
          next->set_acb_per_share(t->get_acb_per_share());
        }
        t = next;
        next = next->get_next();
      }
    }  
	}

	// Compute the ACB, and CGL.
	double History::compute_cgl( unsigned int year ) {
    sort_by_date();
		Transaction *t = p_head;
    double year_cgl{t->get_cgl()};
    if (t != nullptr) {
      Transaction *next = t->get_next();
      while (next != nullptr) {
        if (!(next->get_trans_type()) && next->get_year() == year) {
            year_cgl += next->get_cgl();
        }
        next = next->get_next();
      }
    }

    return year_cgl;
	}


// GIVEN
// Member functions to get values.
//
std::string Transaction::get_symbol() const { return symbol; }
unsigned int Transaction::get_day() const { return day; }
unsigned int Transaction::get_month() const { return month; }
unsigned int Transaction::get_year() const { return year; }
unsigned int Transaction::get_shares() const { return shares; }
double Transaction::get_amount() const { return amount; }
double Transaction::get_acb() const { return acb; }
double Transaction::get_acb_per_share() const { return acb_per_share; }
unsigned int Transaction::get_share_balance() const { return share_balance; }
double Transaction::get_cgl() const { return cgl; }
bool Transaction::get_trans_type() const { return (trans_type == "Buy") ? true: false ; }
unsigned int Transaction::get_trans_id() const { return trans_id; }
Transaction *Transaction::get_next() { return p_next; }

// GIVEN
// Member functions to set values.
//
void Transaction::set_acb( double acb_value ) { acb = acb_value; }
void Transaction::set_acb_per_share( double acb_share_value ) { acb_per_share = acb_share_value; }
void Transaction::set_share_balance( unsigned int bal ) { share_balance = bal ; }
void Transaction::set_cgl( double value ) { cgl = value; }
void Transaction::set_next( Transaction *p_new_next ) { p_next = p_new_next; }

// GIVEN
// Print the transaction.
//
void Transaction::print() {
  std::cout << std::fixed << std::setprecision(2);
  std::cout << std::setw(4) << get_trans_id() << " "
    << std::setw(4) << get_symbol() << " "
    << std::setw(4) << get_day() << " "
    << std::setw(4) << get_month() << " "
    << std::setw(4) << get_year() << " ";


  if ( get_trans_type() ) {
    std::cout << "  Buy  ";
  } else { std::cout << "  Sell "; }

  std::cout << std::setw(4) << get_shares() << " "
    << std::setw(10) << get_amount() << " "
    << std::setw(10) << get_acb() << " " << std::setw(4) << get_share_balance() << " "
    << std::setw(10) << std::setprecision(3) << get_acb_per_share() << " "
    << std::setw(10) << std::setprecision(3) << get_cgl()
    << std::endl;
}


////////////////////////////////////////////////////////////////////////////////
// Definitions for the History class
////////////////////////////////////////////////////////////////////////////////
//
//


// Constructor
// TODO

// Destructor
// TODO

// insert(...): Insert transaction into linked list.
// TODO

// read_history(...): Read the transaction history from file.
// TODO

// print() Print the transaction history.
// TODO

// sort_by_date(): Sort the linked list by trade date.
// TODO

// update_acb_cgl(): Updates the ACB and CGL values.
// TODO

// compute_cgl(): )Compute the ACB, and CGL.
// TODO


// GIVEN
// get_p_head(): Full access to the linked list.
//
Transaction *History::get_p_head() { return p_head; }
