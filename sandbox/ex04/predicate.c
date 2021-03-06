#include "stdlib.h"
#include <assert.h>

struct account {
    int balance;
    int limit;
};
typedef struct account account_t;

/*@
predicate account_pred(account_t* the_account, int the_balance, int the_limit) =
    the_account->balance |-> the_balance &*& the_account->limit |-> the_limit &*&
    malloc_block_account(the_account);
@*/

account_t* create_account(int limit)
    //@ requires limit <= 0;
    /*@ ensures  result->balance |-> 0 &*& result->limit |-> limit &*&
            malloc_block_account(result); @*/
{
    account_t* the_account = malloc(sizeof(account_t));
    if (!the_account) abort();
    the_account->balance = 0;
    the_account->limit = limit;

    return the_account;
}

int account_get_balance(account_t* my_account)
    //@ requires my_account->balance |-> ?the_balance;
    //@ ensures  my_account->balance |-> the_balance &*& result == the_balance;
{
    return my_account->balance;
}

void account_deposit(account_t* my_account, int amount)
    //@ requires 0 <= amount &*& account_pred(my_account, ?balance, ?limit);
    //@ ensures  account_pred(my_account, balance + amount, limit);
{
    //@ open  account_pred(my_account, balance, limit);
    my_account->balance += amount;
    //@ close account_pred(my_account, balance + amount, limit);
}

int account_withdraw(account_t* my_account, int amount)
    /*@ requires 0 <= amount &*&
            my_account->limit |-> ?limit &*& my_account->balance |-> ?balance; @*/
    /*@ ensures  my_account->limit |-> limit &*& my_account->balance |-> balance - result &*&
            result == (balance - amount < limit ? balance - limit : amount); @*/
{
    int withdrawal_amount = my_account->balance - amount < my_account->limit ?
        my_account->balance - my_account->limit : amount;
    my_account->balance -= withdrawal_amount;

    return withdrawal_amount;
}

void dispose_account(account_t* my_account)
    /*@ requires my_account->balance |-> _ &*& my_account->limit |-> _ &*&
            malloc_block_account(my_account); @*/
    //@ ensures  true;
{
    free(my_account);
}

int main()
    //@ requires true;
    //@ ensures  true;
{
    account_t* my_account = create_account(-100);
    account_deposit(my_account, 200);

    int w1 = account_withdraw(my_account, 50);
    assert(w1 == 50);

    int b1 = account_get_balance(my_account);
    assert(b1 == 150);

    int w2 = account_withdraw(my_account, 300);
    assert(w2 == 250);

    int b2 = account_get_balance(my_account);
    assert(b2 == -100);

    dispose_account(my_account);

    return 0;
}
