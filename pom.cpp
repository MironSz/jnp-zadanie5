/**
 * Univeristy of Warsaw 2017
 *  Task: KEYED QUEUE - basic test case runner for checking errors v0.1
 *  Usage:
 *     Just compile and run (might need to change include path to keyed_queue.h).
 *
 *  Writing custom tests:
 *     Use comparable_error_thrower class as an object throwing errros.
 *     Errors can be set on construction or later using #set_hooks (might require const_cast).
 *     Test cases are boolean functions returning true if case is passing.
 *     All errors thrown within a test case must be caught.
 *     Cases to run can be selected in an array just before main() function
 *     - just add implemented test cases there.
 *
 *  Authors:
 *     kk385830  @kowaalczyk-priv
 *     ps386038  @styczynski
 *
 */
#include <iostream>
#include "keyed_queue1.h"
#include <vector>
#include <algorithm>

/*
 * TODO:
 * Uwagi z 2 ćwiczeń dot. programu do sprawdzenia:
 * Exception safety:
 * - push_back wielokrotnie prowadzi do niespójności, lepiej tworzyć nową listę i splice ze starą
 * - map rzuca wyjątki nie tylko przy dodawaniu ale też przy porównywaniu
 * - operator [] na mapie - najlepiej w ogóle nie używać w tym zadaniu (niespójny stan i rzuca wyjątki)
 *
 * Każda operacja która się nie uda ma nie unieważniać żadnych iteratorów:
 * - operacja push - !!! TODO: To nie jest na razie sprawdzane
 *
 * Dziedziczenie publiczne vs prywatne - domyślnie mamy dziedziczyć publicznie
 *
 * #move_to_back:
 * - ma nie kopiować elementów tylko je przesuwać
 */

const int COPY_FAIL = 2137;
const int MOVE_FAIL = 1488;
const int ASSIGN_FAIL = 112;
const int COMPARE_FAIL = 314;
const int DESTRUCT_FAIL = 404;

/**
 * Wrapper for integer,
 * that throws errors depending on booleans specified in its constructor.
 * Compare and assignment operators only affect underlying integer value,
 * not the error-throwing behaviour.
 * It does have strong exception safety: whenever exception is thrown,
 * underlying values are not modified.
 */
class comparable_error_thrower {
private:
    int val;
    bool throw_on_copy, throw_on_move, throw_on_assign, throw_on_compare, throw_on_destruct;

public:

    /**
     * Constructor for use in test cases. Allows to specify throwing behaviour in tests.
     */
    comparable_error_thrower(int val=0,
                             bool throw_on_copy=false,
                             bool throw_on_move=false,
                             bool throw_on_assign=false,
                             bool throw_on_compare=false,
                             bool throw_on_destruct=false) {
        this->val = val;
        this->throw_on_copy = throw_on_copy;
        this->throw_on_move = throw_on_move;
        this->throw_on_assign = throw_on_assign;
        this->throw_on_compare = throw_on_compare;
        this->throw_on_destruct = throw_on_destruct;
    }

    /**
     * Default constructor
     */
    comparable_error_thrower() {
        // TODO: Create a version of this class that throws error on_default_construct
        this->val = 0;
        this->throw_on_copy = false;
        this->throw_on_move = false;
        this->throw_on_assign = false;
        this->throw_on_compare = false;
        this->throw_on_destruct = false;
    }

    /**
     * Copy constructor
     * @param other
     * @throws COPY_FAIL
     */
    comparable_error_thrower(const comparable_error_thrower& other) {
        if(other.throw_on_copy) {
            throw COPY_FAIL;
        }
        this->val = other.val;
        this->throw_on_copy = other.throw_on_copy;
        this->throw_on_move = other.throw_on_move;
        this->throw_on_assign = other.throw_on_assign;
        this->throw_on_compare = other.throw_on_compare;
        this->throw_on_destruct = other.throw_on_destruct;
    }

    /**
     * Move constructor
     * @param other
     * @throws MOVE_FAIL
     */
    comparable_error_thrower(comparable_error_thrower&& other) {
        if(other.throw_on_move) {
            throw MOVE_FAIL;
        }
        this->val = other.val;
        this->throw_on_copy = other.throw_on_copy;
        this->throw_on_move = other.throw_on_move;
        this->throw_on_assign = other.throw_on_assign;
        this->throw_on_compare = other.throw_on_compare;
        this->throw_on_destruct = other.throw_on_destruct;
    }

    /**
     * Allows to modify throwing behaviour in tests.
     */
    void set_hooks(bool throw_on_copy=false,
                   bool throw_on_move=false,
                   bool throw_on_assign=false,
                   bool throw_on_compare=false,
                   bool throw_on_destruct=false) {
        this->throw_on_copy = throw_on_copy;
        this->throw_on_move = throw_on_move;
        this->throw_on_assign = throw_on_assign;
        this->throw_on_compare = throw_on_compare;
        this->throw_on_destruct = throw_on_destruct; // TODO: Not testing this right now
    }

    /**
     * @throws ASSIGN_FAIL
     */
    comparable_error_thrower& operator= (comparable_error_thrower& other) {
        if(throw_on_assign) {
            throw ASSIGN_FAIL;
        }
        std::swap(this->val, other.val); // TODO: Make sure this is OK
        return *this;
    }

    /**
     * @throws COMPARE_FAIL
     */
    bool operator< (const comparable_error_thrower& other) const {
        if(throw_on_compare) {
            throw COMPARE_FAIL;
        }
        return this->val < other.val;
    }

    /**
     * @throws COMPARE_FAIL
     */
    bool operator> (const comparable_error_thrower& other) const {
        if(throw_on_compare) {
            throw COMPARE_FAIL;
        }
        return this->val > other.val;
    }

    /**
     * @throws COMPARE_FAIL
     */
    bool operator<= (const comparable_error_thrower& other) const {
        if(throw_on_compare) {
            throw COMPARE_FAIL;
        }
        return this->val <= other.val;
    }

    /**
     * @throws COMPARE_FAIL
     */
    bool operator>= (const comparable_error_thrower& other) const {
        if(throw_on_compare) {
            throw COMPARE_FAIL;
        }
        return this->val >= other.val;
    }

    /**
     * @throws COMPARE_FAIL
     */
    bool operator== (const comparable_error_thrower& other) const {
        if(throw_on_compare) {
            throw COMPARE_FAIL;
        }
        return this->val == other.val;
    }
};

// HELPER FUNCTIONS for test runner and test cases

void reportCaseFail(const std::string& case_name,
                    const std::string& expected_behaviour="",
                    const std::string& real_behaviour="") {
    std::cout << "Test case failed: " << case_name << std::endl;
    std::cout << "-- Expected behaviour: " << std::endl;
    std::cout << "-- " << expected_behaviour << std::endl;
    std::cout << "-- Real behaviour: " << std::endl;
    std::cout << "-- " << real_behaviour << std::endl;
    std::cout << std::endl;
}

const int ASSERTION_FAIL = 500;

void custom_assert(bool val, std::string fail_comment) {
    if(!val) {
        std::cout << "ASSERTION FAIL: " << fail_comment << std::endl; // TODO: Not sure this is the best place for error logging
        throw ASSERTION_FAIL;
    }
}


// TEST CASES - Define Your test cases here

// default constructor

auto default_constructor_working = []{
    try {
        keyed_queue<int, int> kq1;
        keyed_queue<comparable_error_thrower, comparable_error_thrower> kq2;
    } catch (...) {
        reportCaseFail("default_constructor_working",
                       "No errors thrown",
                       "Unknown error thrown");
        return false;
    }
    return true;
};

// copy constructor

auto copy_constructor_working = []{
    try {
        keyed_queue<int, int> kq1;
        keyed_queue<comparable_error_thrower, comparable_error_thrower> kq2;

        auto kq1_copy1(kq1);
        custom_assert(kq1_copy1.empty(), "Copied queue contains random elements");
        auto kq2_copy1(kq2);
        custom_assert(kq2_copy1.empty(), "Copied queue contains random elements");

        kq1.push(1, 2);
        kq1.push(3, 4);
        kq2.push(comparable_error_thrower(1), comparable_error_thrower(2));
        kq2.push(comparable_error_thrower(3), comparable_error_thrower(4));

        auto kq1_copy2(kq1);
        custom_assert(kq1_copy2.front().first == kq1.front().first &&
                      kq1_copy2.front().second == kq1.front().second &&
                      kq1_copy2.back().first == kq1.back().first &&
                      kq1_copy2.back().second == kq1.back().second,
                      "Copied queue contains wrong elements");
        auto kq2_copy2(kq2);
        custom_assert(kq2_copy2.front().first == kq2.front().first &&
                      kq2_copy2.front().second == kq2.front().second &&
                      kq2_copy2.back().first == kq2.back().first &&
                      kq2_copy2.back().second == kq2.back().second,
                      "Copied queue contains wrong elements");
    } catch (...) {
        reportCaseFail("copy_constructor_working",
                       "No errors thrown",
                       "Unknown error thrown");
        return false;
    }
    return true;
};

auto copy_constructor_key_copy_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(true, false, false, false, false);
        keyed_queue<comparable_error_thrower, comparable_error_thrower> target(kq);
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("copy_constructor_key_copy_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto copy_constructor_key_move_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, true, false, false, false);
        keyed_queue<comparable_error_thrower, comparable_error_thrower> target(kq);
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==MOVE_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("copy_constructor_key_move_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto copy_constructor_key_assign_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, false, true, false, false);
        keyed_queue<comparable_error_thrower, comparable_error_thrower> target(kq);
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==ASSIGN_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("copy_constructor_key_assign_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto copy_constructor_key_compare_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    std::cout<<"jestem w tescie\n\n";
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, false, false, true, false);
        keyed_queue<comparable_error_thrower, comparable_error_thrower> target(kq);
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COMPARE_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
            std::cout<<"hello\n";
        }
    } catch(...) {
        reportCaseFail("copy_constructor_key_compare_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto copy_constructor_val_copy_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_val = const_cast<comparable_error_thrower&>(kq.front().second);
        throwing_val.set_hooks(true, false, false, false, false);
        keyed_queue<comparable_error_thrower, comparable_error_thrower> target(kq);
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("copy_constructor_val_compare_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

// move constructor

auto move_constructor_working = []{
    try {
        keyed_queue<int, int> kq1;
        keyed_queue<int, int> kq2;
        keyed_queue<comparable_error_thrower, comparable_error_thrower> kq3;
        keyed_queue<comparable_error_thrower, comparable_error_thrower> kq4;

        kq1.push(1, 2);
        kq1.push(3, 4);
        kq3.push(comparable_error_thrower(1), comparable_error_thrower(2));
        kq3.push(comparable_error_thrower(3), comparable_error_thrower(4));

        std::swap(kq1, kq2);
        custom_assert(kq2.front().first == 1 &&
                      kq2.front().second == 2 &&
                      kq2.back().first == 3 &&
                      kq2.back().second == 4 &&
                      kq1.empty(),
                      "Swapped queue contains wrong elements");

        std::swap(kq3, kq4);
        custom_assert(kq4.front().first == comparable_error_thrower(1) &&
                      kq4.front().second == comparable_error_thrower(2) &&
                      kq4.back().first == comparable_error_thrower(3) &&
                      kq4.back().second == comparable_error_thrower(4) &&
                      kq3.empty(),
                      "Swapped queue contains wrong elements");
    } catch (...) {
        reportCaseFail("move_constructor_working",
                       "No errors thrown",
                       "Unknown error thrown");
        return false;
    }
    return true;
};

auto move_constructor_key_copy_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(true, false, false, false, false);
        std::swap(target, kq);
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
            custom_assert(target.empty(), "Target was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("move_constructor_key_copy_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto move_constructor_key_move_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, true, false, false, false);
        std::swap(target, kq);
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==MOVE_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
            custom_assert(target.empty(), "Target was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("move_constructor_key_move_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto move_constructor_key_assign_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, false, true, false, false);
        std::swap(target, kq);
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==ASSIGN_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
            custom_assert(target.empty(), "Target was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("move_constructor_key_assign_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto move_constructor_key_compare_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, false, false, true, false);
        std::swap(target, kq);
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COMPARE_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
            custom_assert(target.empty(), "Target was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("move_constructor_key_compare_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto move_constructor_val_copy_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_val = const_cast<comparable_error_thrower&>(kq.front().second);
        throwing_val.set_hooks(true, false, false, false, false);
        std::swap(target, kq);
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
            custom_assert(target.empty(), "Target was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("move_constructor_val_copy_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

// assignment operator

auto assign_working = []{
    try {
        keyed_queue<int, int> kq1;
        keyed_queue<comparable_error_thrower, comparable_error_thrower> kq2;

        auto kq1_copy1 = kq1;
        custom_assert(kq1_copy1.empty(), "Copied queue contains random elements");
        auto kq2_copy1 = kq2;
        custom_assert(kq2_copy1.empty(), "Copied queue contains random elements");

        kq1.push(1, 2);
        kq1.push(3, 4);
        kq2.push(comparable_error_thrower(1), comparable_error_thrower(2));
        kq2.push(comparable_error_thrower(3), comparable_error_thrower(4));

        auto kq1_copy2 = kq1;
        custom_assert(kq1_copy2.front().first == kq1.front().first &&
                      kq1_copy2.front().second == kq1.front().second &&
                      kq1_copy2.back().first == kq1.back().first &&
                      kq1_copy2.back().second == kq1.back().second,
                      "Copied queue contains wrong elements");
        auto kq2_copy2 = kq2;
        custom_assert(kq2_copy2.front().first == kq2.front().first &&
                      kq2_copy2.front().second == kq2.front().second &&
                      kq2_copy2.back().first == kq2.back().first &&
                      kq2_copy2.back().second == kq2.back().second,
                      "Copied queue contains wrong elements");
    } catch (...) {
        reportCaseFail("assign_working",
                       "No errors thrown",
                       "Unknown error thrown");
        return false;
    }
    return true;
};

auto assign_key_copy_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(true, false, false, false, false);
        target = kq;
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
            custom_assert(target.empty(), "Target was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("assign_key_copy_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto assign_key_move_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, true, false, false, false);
        target = kq;
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==MOVE_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
            custom_assert(target.empty(), "Target was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("assign_key_move_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto assign_key_assign_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, false, true, false, false);
        target = kq;
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==ASSIGN_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
            custom_assert(target.empty(), "Target was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("assign_key_assign_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto assign_key_compare_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, false, false, true, false);
        target = kq;
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COMPARE_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
            custom_assert(target.empty(), "Target was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("assign_key_compare_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto assign_val_copy_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_val = const_cast<comparable_error_thrower&>(kq.front().second);
        throwing_val.set_hooks(true, false, false, false, false);
        target = kq;
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
            custom_assert(target.empty(), "Target was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("assign_val_copy_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

// push

auto push_working = []{
    try {
        keyed_queue<int, int> kq;
        kq.push(1, 3);
        custom_assert(kq.front().first == 1, "Keys do not match");
        custom_assert(kq.front().second == 3, "Values do not match");
    } catch(int e) {
        if(e == ASSERTION_FAIL) {
            reportCaseFail("push_working",
                           "",
                           "inserted values do not match given ones");
            return false;
        }
    } catch (...) {
        reportCaseFail("push_working",
                       "no exceptions thrown",
                       "unknown exception thrown");
        return false;
    }
    return true;
};

auto push_key_copy_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
    try {
        kq.push(comparable_error_thrower(21, true, false, false, false, false), comparable_error_thrower(37, false, false, false, false, false));
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Queue was modified despite exception");
            custom_assert(kq.back().first == comparable_error_thrower(10) && kq.back().second == comparable_error_thrower(20), "Queue was modified despite exception");
            custom_assert(kq.size() == 1, "Queue was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("push_key_copy_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto push_key_move_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
    try {
        kq.push(comparable_error_thrower(21, false, true, false, false, false), comparable_error_thrower(37, false, false, false, false, false));
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==MOVE_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Queue was modified despite exception");
            custom_assert(kq.back().first == comparable_error_thrower(10) && kq.back().second == comparable_error_thrower(20), "Queue was modified despite exception");
            custom_assert(kq.size() == 1, "Queue was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("push_key_move_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto push_key_assign_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
    try {
        kq.push(comparable_error_thrower(21, false, false, true, false, false), comparable_error_thrower(37, false, false, false, false, false));
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==ASSIGN_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Queue was modified despite exception");
            custom_assert(kq.back().first == comparable_error_thrower(10) && kq.back().second == comparable_error_thrower(20), "Queue was modified despite exception");
            custom_assert(kq.size() == 1, "Queue was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("push_key_assign_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto push_key_compare_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
    try {
        kq.push(comparable_error_thrower(21, false, false, false, true, false), comparable_error_thrower(37, false, false, false, false, false));
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COMPARE_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Queue was modified despite exception");
            custom_assert(kq.back().first == comparable_error_thrower(10) && kq.back().second == comparable_error_thrower(20), "Queue was modified despite exception");
            custom_assert(kq.size() == 1, "Queue was modified despite exception");
        }
    } catch(int e) {
        if(e == ASSERTION_FAIL) {
            reportCaseFail("push_key_compare_fail_does_not_modify_queue",
                           "",
                           "One of assertions failed - see message above");
        } else {
            reportCaseFail("push_key_compare_fail_does_not_modify_queue",
                           "",
                           "Unknown error thrown");
        }
        return false;
    } catch (...) {
        reportCaseFail("push_key_compare_fail_does_not_modify_queue",
                       "",
                       "Unknown error thrown");
        return false;
    }
    return true;
};

auto push_val_copy_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
    try {
        kq.push(comparable_error_thrower(21, false, false, false, false, false), comparable_error_thrower(37, true, false, false, false, false));
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Queue was modified despite exception");
            custom_assert(kq.back().first == comparable_error_thrower(10) && kq.back().second == comparable_error_thrower(20), "Queue was modified despite exception");
            custom_assert(kq.size() == 1, "Queue was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("push_val_copy_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

// pop

auto pop_working = []{
    try {
        keyed_queue<int, int> kq1;
        keyed_queue<comparable_error_thrower, comparable_error_thrower> kq2;

        // this test obviously assumes #push is working :c
        kq1.push(1, 2);
        kq1.push(3, 4);
        kq2.push(comparable_error_thrower(1), comparable_error_thrower(2));
        kq2.push(comparable_error_thrower(3), comparable_error_thrower(4));

        kq1.pop();
        kq1.pop();

        kq2.pop();
        kq2.pop();

        custom_assert(kq1.empty() && kq2.empty(),
                      "Swapped queue contains wrong elements");
    } catch (...) {
        reportCaseFail("pop_working",
                       "No errors thrown",
                       "Unknown error thrown");
        return false;
    }
    return true;
};

auto pop_key_copy_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(true, false, false, false, false);
        target.pop();
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(!target.empty(), "Item was popped despite exception");
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("pop_key_copy_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto pop_key_move_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, true, false, false, false);
        target.pop();
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==MOVE_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(!target.empty(), "Item was popped despite exception");
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("pop_key_move_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto pop_key_assign_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, false, true, false, false);
        target.pop();
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==ASSIGN_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(!target.empty(), "Item was popped despite exception");
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("pop_key_assign_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto pop_key_compare_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.front().first);
        throwing_key.set_hooks(false, false, false, true, false);
        target.pop();
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COMPARE_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(!target.empty(), "Item was popped despite exception");
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("pop_key_compare_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto pop_val_copy_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> target;
    try {
        kq.push(comparable_error_thrower(10, false, false, false, false, false), comparable_error_thrower(20, false, false, false, false, false));
        comparable_error_thrower& throwing_val = const_cast<comparable_error_thrower&>(kq.front().second);
        throwing_val.set_hooks(true, false, false, false, false);
        target.pop();
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(!target.empty(), "Item was popped despite exception");
            custom_assert(kq.front().first == comparable_error_thrower(10) && kq.front().second == comparable_error_thrower(20), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("pop_val_copy_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

// pop with key

auto pop_w_key_working = []{
    try {
        keyed_queue<int, int> kq1;
        keyed_queue<comparable_error_thrower, comparable_error_thrower> kq2;

        // this test obviously assumes #push is working :c
        kq1.push(1, 2);
        kq1.push(3, 4);
        kq2.push(comparable_error_thrower(1), comparable_error_thrower(2));
        kq2.push(comparable_error_thrower(3), comparable_error_thrower(4));

        kq1.pop(3);
        custom_assert(kq1.size() == 1, "Failed to remove item from queue");
        custom_assert(kq1.front().first == 1 && kq1.front().second == 2, "Wrong item removed");
        kq1.pop(1);
        custom_assert(kq1.empty(), "Failed to remove item from queue");

        kq2.pop(comparable_error_thrower(3));
        custom_assert(kq2.size() == 1, "Failed to remove item from queue");
        custom_assert(kq2.front().first == comparable_error_thrower(1) && kq2.front().second == comparable_error_thrower(2), "Wrong item removed");
        kq2.pop(comparable_error_thrower(1));
        custom_assert(kq2.empty(), "Failed to remove item from queue");

        custom_assert(kq1.empty() && kq2.empty(),
                      "Swapped queue contains wrong elements");
    } catch (...) {
        reportCaseFail("pop_w_key_working",
                       "No errors thrown",
                       "Unknown error thrown");
        return false;
    }
    return true;
};

auto pop_w_key_key_copy_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    try {
        // this test obviously assumes #push and #first(K) are working :c
        kq.push(comparable_error_thrower(1), comparable_error_thrower(2));
        kq.push(comparable_error_thrower(3), comparable_error_thrower(4));

        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.first(comparable_error_thrower(3)).first);
        throwing_key.set_hooks(true, false, false, false, false);
        kq.pop(comparable_error_thrower(3));
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.size() == 2, "Item was popped despite exception");
            custom_assert(kq.first(comparable_error_thrower(3)).first == comparable_error_thrower(3) && kq.first(comparable_error_thrower(3)).second == comparable_error_thrower(4), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("pop_w_key_key_copy_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto pop_w_key_key_move_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    try {
        // this test obviously assumes #push and #first(K) are working :c
        kq.push(comparable_error_thrower(1), comparable_error_thrower(2));
        kq.push(comparable_error_thrower(3), comparable_error_thrower(4));

        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.first(comparable_error_thrower(3)).first);
        throwing_key.set_hooks(false, true, false, false, false);
        kq.pop(comparable_error_thrower(3));
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.size() == 2, "Item was popped despite exception");
            custom_assert(kq.first(comparable_error_thrower(3)).first == comparable_error_thrower(3) && kq.first(comparable_error_thrower(3)).second == comparable_error_thrower(4), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("pop_w_key_key_move_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto pop_w_key_key_assign_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    try {
        // this test obviously assumes #push and #first(K) are working :c
        kq.push(comparable_error_thrower(1), comparable_error_thrower(2));
        kq.push(comparable_error_thrower(3), comparable_error_thrower(4));

        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.first(comparable_error_thrower(3)).first);
        throwing_key.set_hooks(false, false, true, false, false);
        kq.pop(comparable_error_thrower(3));
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.size() == 2, "Item was popped despite exception");
            custom_assert(kq.first(comparable_error_thrower(3)).first == comparable_error_thrower(3) && kq.first(comparable_error_thrower(3)).second == comparable_error_thrower(4), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("pop_w_key_key_assign_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto pop_w_key_key_compare_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    try {
        // this test obviously assumes #push and #first(K) are working :c
        kq.push(comparable_error_thrower(1), comparable_error_thrower(2));
        kq.push(comparable_error_thrower(3), comparable_error_thrower(4));

        comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.first(comparable_error_thrower(3)).first);
        throwing_key.set_hooks(false, false, false, true, false);
        kq.pop(comparable_error_thrower(3));
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.size() == 2, "Item was popped despite exception");
            custom_assert(kq.first(comparable_error_thrower(3)).first == comparable_error_thrower(3) && kq.first(comparable_error_thrower(3)).second == comparable_error_thrower(4), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("pop_w_key_key_compare_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

auto pop_w_key_val_copy_fail_does_not_modify_queue = []{
    int err_code = -1;
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    try {
        // this test obviously assumes #push and #first(K) are working :c
        kq.push(comparable_error_thrower(1), comparable_error_thrower(2));
        kq.push(comparable_error_thrower(3), comparable_error_thrower(4));

        comparable_error_thrower& throwing_val = const_cast<comparable_error_thrower&>(kq.first(comparable_error_thrower(3)).second);
        throwing_val.set_hooks(true, false, false, false, false);
        kq.pop(comparable_error_thrower(3));
    } catch(int e) {
        err_code = e;
    } catch (...) {
        // do nothing
    }
    try {
        custom_assert(err_code==-1 || err_code==COPY_FAIL, "Unknown error thrown: "+std::to_string(err_code));
        if(err_code != -1) {
            // not sure where exceptions are thrown, but if they were - no changes should've been made
            custom_assert(kq.size() == 2, "Item was popped despite exception");
            custom_assert(kq.first(comparable_error_thrower(3)).first == comparable_error_thrower(3) && kq.first(comparable_error_thrower(3)).second == comparable_error_thrower(4), "Source was modified despite exception");
        }
    } catch(...) {
        reportCaseFail("pop_w_key_val_copy_fail_does_not_modify_queue",
                       "",
                       "One of assertions failed - see message above");
        return false;
    }
    return true;
};

// move to back

auto move_to_back_working = []{
    // TODO
    reportCaseFail("move_to_back_working",
                   "",
                   "Unimplemented");
    return false;
};

auto move_to_back_key_copy_fail_does_not_modify_queue = []{
    // TODO
    reportCaseFail("move_to_back_key_copy_fail_does_not_modify_queue",
                   "",
                   "Unimplemented");
    return false;
};

auto move_to_back_key_move_fail_does_not_modify_queue = []{
    // TODO
    reportCaseFail("move_to_back_key_move_fail_does_not_modify_queue",
                   "",
                   "Unimplemented");
    return false;
};

auto move_to_back_key_assign_fail_does_not_modify_queue = []{
    // TODO
    reportCaseFail("move_to_back_key_assign_fail_does_not_modify_queue",
                   "",
                   "Unimplemented");
    return false;
};

auto move_to_back_key_compare_fail_does_not_modify_queue = []{
    // TODO
    reportCaseFail("move_to_back_key_compare_fail_does_not_modify_queue",
                   "",
                   "Unimplemented");
    return false;
};

auto move_to_back_val_copy_fail_does_not_modify_queue = []{
    // TODO
    reportCaseFail("move_to_back_val_copy_fail_does_not_modify_queue",
                   "",
                   "Unimplemented");
    return false;
};

// front and back

auto front_working_on_non_empty_list = []{
    try {
        keyed_queue<int, int> kq;
        kq.push(1, 3);
        custom_assert(kq.front().first == 1, "Keys do not match");
        custom_assert(kq.front().second == 3, "Values do not match");
    } catch(int e) {
        if(e == ASSERTION_FAIL) {
            reportCaseFail("front_working_on_non_empty_list",
                           "",
                           "inserted values do not match given ones");
            return false;
        }
    } catch (...) {
        reportCaseFail("front_working_on_non_empty_list",
                       "no exceptions thrown",
                       "unknown exception thrown");
        return false;
    }
    return true;
};

auto front_throws_err_on_empty_list = []{
    try {
        keyed_queue<int, int> kq;
        kq.front();
    } catch(lookup_error) {
        return true;
    } catch (...) {
        reportCaseFail("front_throws_err_on_empty_list",
                       "no exceptions thrown",
                       "unknown exception thrown");
        return false;
    }
    reportCaseFail("front_throws_err_on_empty_list",
                   "lookup_error_thrown",
                   "no error was thrown");
    return false;
};

auto back_working_on_non_empty_list = []{
    try {
        keyed_queue<int, int> kq;
        kq.push(1, 3);
        custom_assert(kq.back().first == 1, "Keys do not match");
        custom_assert(kq.back().second == 3, "Values do not match");
    } catch(int e) {
        if(e == ASSERTION_FAIL) {
            reportCaseFail("end_working_on_non_empty_list",
                           "",
                           "inserted values do not match given ones");
            return false;
        }
    } catch (...) {
        reportCaseFail("end_working_on_non_empty_list",
                       "no exceptions thrown",
                       "unknown exception thrown");
        return false;
    }
    return true;
};

auto back_throws_err_on_empty_list = []{
    try {
        keyed_queue<int, int> kq;
        kq.back();
    } catch(lookup_error) {
        return true;
    } catch (...) {
        reportCaseFail("end_throws_err_on_empty_list",
                       "no exceptions thrown",
                       "unknown exception thrown");
        return false;
    }
    reportCaseFail("end_throws_err_on_empty_list",
                   "lookup_error_thrown",
                   "no error was thrown");
    return false;
};

// first and last

auto first_working_on_non_empty_list = []{
    try {
        keyed_queue<int, int> kq;
        kq.push(1, 3);
        kq.push(21, 37);
        custom_assert(kq.first(1).first == 1, "Keys do not match");
        custom_assert(kq.first(1).second == 3, "Values do not match");
    } catch(int e) {
        if(e == ASSERTION_FAIL) {
            reportCaseFail("first_working_on_non_empty_list",
                           "",
                           "inserted values do not match given ones");
            return false;
        }
    } catch (...) {
        reportCaseFail("first_working_on_non_empty_list",
                       "no exceptions thrown",
                       "unknown exception thrown");
        return false;
    }
    return true;
};

auto first_throws_err_on_empty_list = []{
    try {
        keyed_queue<int, int> kq;
        kq.first(7);
    } catch(lookup_error) {
        return true;
    } catch (...) {
        reportCaseFail("first_throws_err_on_empty_list",
                       "no exceptions thrown",
                       "unknown exception thrown");
        return false;
    }
    reportCaseFail("first_throws_err_on_empty_list",
                   "lookup_error_thrown",
                   "no error was thrown");
    return false;
};

auto last_working_on_non_empty_list = []{
    try {
        keyed_queue<int, int> kq;
        kq.push(1, 3);
        kq.push(21, 37);
        custom_assert(kq.last(1).first == 1, "Keys do not match");
        custom_assert(kq.last(1).second == 3, "Values do not match");
    } catch(int e) {
        if(e == ASSERTION_FAIL) {
            reportCaseFail("last_working_on_non_empty_list",
                           "",
                           "inserted values do not match given ones");
            return false;
        }
    } catch (...) {
        reportCaseFail("last_working_on_non_empty_list",
                       "no exceptions thrown",
                       "unknown exception thrown");
        return false;
    }
    return true;
};

auto last_throws_err_on_empty_list = []{
    try {
        keyed_queue<int, int> kq;
        kq.last(7);
    } catch(lookup_error) {
        return true;
    } catch (...) {
        reportCaseFail("last_throws_err_on_empty_list",
                       "no exceptions thrown",
                       "unknown exception thrown");
        return false;
    }
    reportCaseFail("last_throws_err_on_empty_list",
                   "lookup_error_thrown",
                   "no error was thrown");
    return false;
};

// size and empty

auto size_working = []{
    try {
        keyed_queue<int, int> kq;
        kq.push(1, 3);
        custom_assert(kq.size() == 1, "Wrong count");
        kq.push(21, 37);
        custom_assert(kq.size() == 2, "Wrong count");
        kq.push(21, 37);
        custom_assert(kq.size() == 3, "Wrong count");
        kq.pop(21);
        custom_assert(kq.size() == 2, "Wrong count");
    } catch(int e) {
        if(e == ASSERTION_FAIL) {
            reportCaseFail("size_working",
                           "",
                           "wrong count");
            return false;
        }
    } catch (...) {
        reportCaseFail("size_working",
                       "no exceptions thrown",
                       "unknown exception thrown");
        return false;
    }
    return true;
};

auto empty_working = []{
    try {
        keyed_queue<int, int> kq;
        custom_assert(kq.empty(), "Empty queue is not empty");
        kq.push(1, 3);
        custom_assert(!kq.empty(), "Wrong count");
        kq.push(1, 3);
        custom_assert(!kq.empty(), "Wrong count");
        kq.pop();
        custom_assert(!kq.empty(), "Too much values removed in pop");
        kq.pop();
        custom_assert(kq.empty(), "Empty queue is not empty");
    } catch(int e) {
        if(e == ASSERTION_FAIL) {
            reportCaseFail("empty_working",
                           "",
                           "wrong count");
            return false;
        }
    } catch (...) {
        reportCaseFail("empty_working",
                       "no exceptions thrown",
                       "unknown exception thrown");
        return false;
    }
    return true;
};

// clear

auto clear_working_on_non_empty_list = []{
    try {
        keyed_queue<int, int> kq;
        for(int count = 0; count < 100; count++) {
            for(int i=0; i<count; i++) {
                kq.push(i, i);
            }
            kq.clear();
            custom_assert(kq.empty(),  "queue not empty");
        }
    } catch(int e) {
        if(e == ASSERTION_FAIL) {
            reportCaseFail("clear_working_on_non_empty_list",
                           "",
                           "queue not empty");
            return false;
        }
    } catch (...) {
        reportCaseFail("clear_working_on_non_empty_list",
                       "no exceptions thrown",
                       "unknown exception thrown");
        return false;
    }
    return true;
};

auto clear_working_on_empty_list = []{
    keyed_queue<int, int> kq;
    try {
        kq.clear();
        custom_assert(kq.empty(),  "queue not empty");
    } catch(int e) {
        if(e == ASSERTION_FAIL) {
            reportCaseFail("clear_working_on_empty_list",
                           "",
                           "queue not empty");
            return false;
        }
    } catch (...) {
        reportCaseFail("clear_working_on_empty_list",
                       "no exceptions thrown",
                       "unknown exception thrown");
        return false;
    }
    return true;
};

// count

auto count_working_on_non_empty_list = []{
    try {
        keyed_queue<int, int> kq;
        for(int count = 1; count < 100; count++) {
            for(int i=0; i<count; i++) {
                kq.push(i, 1);
            }
            for(int i=0; i<count; i++) {
                custom_assert(kq.count(i) == 1,  "wrong count");
            }
            kq.clear();
        }
    } catch(int e) {
        if(e == ASSERTION_FAIL) {
            reportCaseFail("count_working_on_non_empty_list",
                           "",
                           "wrong count");
            return false;
        }
    } catch (...) {
        reportCaseFail("count_working_on_non_empty_list",
                       "no exceptions thrown",
                       "unknown exception thrown");
        return false;
    }
    return true;
};

auto count_working_on_empty_list = []{
    keyed_queue<int, int> kq;
    try {
        custom_assert(kq.count(7) == 0,  "empty queue does not have count == 0");
    } catch(int e) {
        if(e == ASSERTION_FAIL) {
            reportCaseFail("count_working_on_empty_list",
                           "",
                           "wrong count");
            return false;
        }
    } catch (...) {
        reportCaseFail("count_working_on_empty_list",
                       "no exceptions thrown",
                       "unknown exception thrown");
        return false;
    }
    return true;
};

auto count_fails_safely_when_key_comparison_fails = []{
    keyed_queue<comparable_error_thrower, comparable_error_thrower> kq;
    for(int size = 1; size < 100; size++) {
        for(int i=0; i<size; i++) {
            kq.push(comparable_error_thrower(i), 1);
            comparable_error_thrower& throwing_key = const_cast<comparable_error_thrower&>(kq.first(comparable_error_thrower(i)).first);
            throwing_key.set_hooks(false, false, false, true, false);
        }
        for(int i=0; i<size; i++) {
            int err_code = -1;
            try {
                kq.count(i) == 1;
            } catch (int e) {
                err_code = e;
            }
            try {
                custom_assert(err_code == -1 || err_code == COMPARE_FAIL, "wrong error is re-thrown from queue: " + std::to_string(err_code));
                custom_assert(kq.size() == size, "queue corrupted by invalid comparison");
            } catch (...) {
                reportCaseFail("count_fails_safely_when_key_comparison_fails",
                               "",
                               "Assertion failed");
                return false;
            }
        }
        kq.clear();
    }
    return true;
};


// TODO: Make tests for iterator

// TEST RUNNER ----------------------------------------------------------------------------------------

// TODO: Uncomment implemented cases
bool (*cases_to_run[])(void) = {
        default_constructor_working,
        copy_constructor_working,
        copy_constructor_key_copy_fail_does_not_modify_queue,
        copy_constructor_key_move_fail_does_not_modify_queue,
        copy_constructor_key_assign_fail_does_not_modify_queue,
        copy_constructor_key_compare_fail_does_not_modify_queue,
        copy_constructor_val_copy_fail_does_not_modify_queue,
        move_constructor_working,
        move_constructor_key_copy_fail_does_not_modify_queue,
        move_constructor_key_move_fail_does_not_modify_queue,
        move_constructor_key_assign_fail_does_not_modify_queue,
        move_constructor_key_compare_fail_does_not_modify_queue,
        move_constructor_val_copy_fail_does_not_modify_queue,
        assign_working,
        assign_key_copy_fail_does_not_modify_queue,
        assign_key_move_fail_does_not_modify_queue,
        assign_key_assign_fail_does_not_modify_queue,
        assign_key_compare_fail_does_not_modify_queue,
        assign_val_copy_fail_does_not_modify_queue,
        push_working,
        push_key_copy_fail_does_not_modify_queue,
        push_key_move_fail_does_not_modify_queue,
        push_key_assign_fail_does_not_modify_queue,
        push_key_compare_fail_does_not_modify_queue,
        push_val_copy_fail_does_not_modify_queue,
        pop_working,
        pop_key_copy_fail_does_not_modify_queue,
        pop_key_move_fail_does_not_modify_queue,
        pop_key_assign_fail_does_not_modify_queue,
        pop_key_compare_fail_does_not_modify_queue,
        pop_val_copy_fail_does_not_modify_queue,
        pop_w_key_working,
        pop_w_key_key_copy_fail_does_not_modify_queue,
        pop_w_key_key_move_fail_does_not_modify_queue,
        pop_w_key_key_assign_fail_does_not_modify_queue,
        pop_w_key_key_compare_fail_does_not_modify_queue,
        pop_w_key_val_copy_fail_does_not_modify_queue,
//        move_to_back_working,
//        move_to_back_key_copy_fail_does_not_modify_queue,
//        move_to_back_key_move_fail_does_not_modify_queue,
//        move_to_back_key_assign_fail_does_not_modify_queue,
//        move_to_back_key_compare_fail_does_not_modify_queue,
//        move_to_back_val_copy_fail_does_not_modify_queue,
        front_working_on_non_empty_list,
        front_throws_err_on_empty_list,
        back_working_on_non_empty_list,
        back_throws_err_on_empty_list,
        first_working_on_non_empty_list,
        first_throws_err_on_empty_list,
        last_working_on_non_empty_list,
        last_throws_err_on_empty_list,
        size_working,
        empty_working,
        clear_working_on_non_empty_list,
        clear_working_on_empty_list,
        count_working_on_non_empty_list,
        count_working_on_empty_list,
        count_fails_safely_when_key_comparison_fails
};

int main() {
    int failed_cases = 0;
    int passed_cases = 0;
    for(auto *test_case : cases_to_run) {
        if(test_case()) {
            passed_cases++;
        } else {
            failed_cases++;
        }
    }

    std::cout << std::endl;
    std::cout << "Test run finished, #passed:" << passed_cases << " #failed:" << failed_cases << std::endl;

    return 0;
}