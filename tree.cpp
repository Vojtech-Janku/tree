#include "tree.hpp"
#include <cassert>
#include <type_traits>

/* You can put implementations of non-template methods and functions
 * into this file, as you see fit. Do not put anything after the
 * ‹main› function, and make sure its prototype remains untouched.
 * Feel free to add testing code to ‹main›. */

std::vector<int> push_and_get( const std::vector<int> &path, const int &i ) {
        std::vector<int> new_path = path;
        new_path.push_back( i );
        return new_path;
}

template< typename value_t >
std::vector<value_t> get_post_order( const tree<value_t> &tre ) {
        std::vector<value_t> res;
        auto it = tree_begin( tre );
        while ( it != tree_end( tre ) ) {
                res.push_back(*it );
                ++it;
        }
        return res;
}

tree<int> get_tree1() {
        tree< int > tre;
        auto nref = tre.make_root( 10 );
        tre.add_child( nref, 1 );
        tre.add_child( nref, 4 );
        auto nref4 = tre.child_at( nref, 1 );
        tre.add_child( nref4, 2 );
        tre.add_child( nref4, 3 );
        tre.add_child( nref, 9 );
        auto nref9 = tre.child_at( nref, 2 );
        tre.add_child( nref9, 8 );
        auto nref8 = tre.child_at( nref9, 0 );
        tre.add_child( nref8, 5 );
        tre.add_child( nref8, 6 );
        tre.add_child( nref8, 7 );
        return tre;
}

void test_empty() {
        std::cout << "Test empty: \n";
        tree< int > tre;
        assert( tree_begin( tre ) == tree_end( tre ) );
        const tree< int > &const_tre = tre;
        assert( tree_begin( const_tre ) == tree_end( const_tre ) );
        std::cout << "PASSED\n";
}

void test_iterate() {
        std::cout << "Test iterate: \n";
    tree< int > tre;
    auto nref = tre.make_root(10);
    tre.add_child( nref, 5 );
    tre.add_child( nref, 3 );
    tre.add_child( nref, 8 );
    auto nref2 = tre.child_at( nref, 2 );
    tre.add_child( nref2, 11 );
    tre.add_child( nref, 4 );
    auto nref3 = tre.root();
    tre.add_child( nref3, 2300 );
    nref3 = tre.child_at( nref3, 4 );

    std::vector<int> expect = { 5, 3, 11, 8, 4, 2300, 10 };
        std::vector<int> res;
        const tree<int> &const_tre = tre;
        auto const_it = tree_begin( const_tre );
        while ( const_it != tree_end( const_tre ) ) {
                res.push_back( *const_it );
                ++const_it;
        }
        assert( res == expect );
        res.clear();
    auto it = tree_begin( tre );
        while ( it != tree_end( tre ) ) {
                res.push_back(*it );
                ++it;
        }
        assert( res == expect );

        auto tre1 = get_tree1();
        std::vector<int> res1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        assert( get_post_order( tre1 ) == res1 );
        std::cout << "PASSED\n";
}

void test_mutable() {
        std::cout << "Test mutable: \n";
        auto tre = get_tree1();
        const auto &const_tre = tre;
        std::vector<int> before = get_post_order( tre );
        std::vector<int> changed = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };

        int count = 0;
        auto const_it = tree_begin( const_tre );
        while ( const_it != tree_end( const_tre ) ) {
                assert ( std::is_const_v<std::remove_reference_t<decltype(*const_it)>> );
                ++const_it;
                ++count;
        }
        //assert( get_post_order( tre ) == before );

    auto it = tree_begin( tre );
    count = 0;
        while ( it != tree_end( tre ) ) {
                int &val = *it;
                val = changed[count];
                ++it;
                ++count;
        }
        assert( get_post_order( tre ) == changed );
        std::cout << "PASSED\n";
}

int main()
{
    /* tests go here */
        test_empty();
        test_iterate();
        test_mutable();

    return 0;
}