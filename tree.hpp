/* In this task, you will write a simple tree iterator, i.e. an
 * iterator that can be used to visit all nodes of a tree, in
 * post-order. */

/* First, implement a ‹tree› class, with the given interface. This
 * will be the interface that ‹tree_iterator› will use (of course,
 * you can add methods and attributes to ‹tree› as you see fit, but
 * ‹tree_iterator› must not use them).
 *
 * The tree is made of nodes, where each node can have an arbitrary
 * number of children and a single value. */

#include <vector>
#include <memory>
#include <iostream>
#include <string>
#include <iterator>
#include <cstddef>

std::vector<int> push_and_get( const std::vector<int> &path, const int &i );

template< typename value_t_ >
struct node {
        value_t_ value;
        node *parent;
        std::vector< std::shared_ptr<node> > children;

        node( value_t_ value, node *parent = nullptr ) : value(value), parent(parent) {}
        node() = default;
};

template< typename value_t_ >
struct tree
{
    using value_t = value_t_;
    using node_t = node<value_t>;
    std::unique_ptr<node_t> _root;

        tree() : _root( nullptr ) {}

    /* Substitute for any type you like, but make sure it can be
     * copied, assigned and compared for equality; again,
     * ‹tree_iterator› must not rely on the details (it can assign
     * and copy values of type ‹node_ref› though). */

    struct node_ref
    {
        std::vector<int> path; // path from root
                node_t *node;
                friend bool operator==( const node_ref &a, const node_ref &b ) { return a.path == b.path; }

                node_ref( std::vector<int> path, node_t *node ) : path( path ), node( node ) {}
                node_ref() = default;

                void print() {
                        std::cout << " ( value: " << std::to_string( node->value )
                                          << ", children: " << std::to_string( node->children.size() ) << " ) ";
                }
    };

    /* These functions provide access to the tree and the values
     * stored in nodes. */

    bool empty() const { return !_root; }
    node_ref root() const { return node_ref( {}, _root.get() ); }
    node_ref child_at( node_ref nref, int i ) const {
        return node_ref( push_and_get( nref.path, i ), ( nref.node->children )[ i ].get() );
    }
    int child_count( node_ref nref ) const {
        if ( ((nref.node)->children).empty() ) return 0;
        return ((nref.node)->children).size();
    }

    const value_t &value( node_ref nref ) const {
        return nref.node->value;
    }
    value_t &value( node_ref nref ) {
        return nref.node->value;
    }

    /* Finally, methods for constructing and updating the tree
     * follow. A tree iterator must not use them. The child added
     * last has the highest index. */

    node_ref make_root( const value_t &value ) {
        _root = std::make_unique<node_t>( value );
        return node_ref( {}, _root.get() );
    }
    node_ref add_child( node_ref parent, const value_t &value ) {
        std::shared_ptr<node_t> new_child = std::make_shared<node_t>();
        (*new_child).parent = parent.node;
        (*new_child).value = value;
        parent.node->children.push_back( new_child );
        return node_ref( push_and_get( parent.path, child_count( parent )-1), new_child.get() );
    }

    /* Remove the entire subtree rooted at ‹node›. */

    void erase_subtree( node_ref node ) {
        if ( node.path.empty() ) { // node is ref to _root
                _root.reset();
        } else {
                auto &vec = node.node->parent->children;
                vec.erase( std::next( vec.begin(), node.path.back() ) );
        }
    }

        void print() {
                std::cout << "TREE: \n";
                std::vector<node_ref> level = { root() };
                while ( !level.empty() ) {
                        std::vector<node_ref> next_level;
                        for ( auto ref : level ) {
                                ref.print();
                                for ( int i = 0; i < child_count( ref ); i++ ) {
                                        next_level.push_back( child_at( ref, i ) );
                                }
                        }
                        std::cout << "\n";
                        level = next_level;
                }
        }
};

/* Iterate a given tree in post-order; ‹tree_iterator› must be (at
 * least) a forward iterator. Adding nodes to the tree must not
 * invalidate any iterators. Removal of a node invalidates the
 * iterators pointing at that node or at any of its right siblings.
 * Dereferencing the iterator yields the «value» of the node being
 * pointed at.
 *
 * The tree given may or may not be an instance of the above class
 * template ‹tree›, but it will have a ‹node_ref› nested type and
 * the access methods (‹empty›, ‹root›, ‹child_at›, ‹child_count›
 * and ‹value›). The ‹value› method might return a reference (like
 * in your implementation above) or a value, and the iterator must
 * preserve the return type of ‹value› when dereferenced. */

/* Note: You don't have to keep the exact form of the following
 * declarations, but if you decide to replace them, you must make
 * sure that the new declarations are equivalent in this sense:
 *
 *  • it must be possible to declare values using ‹tree_iterator›
 *    and ‹const_tree_iterator› template instances as their type,
 *  • it is permissible to call ‹tree_begin› and ‹tree_end› on both
 *    ‹const› and mutable references to the respective ‹tree› type and
 *    the result type must match the below declarations. */

template< typename tree > struct tree_iterator {
        using n_ref = typename tree::node_ref;
        //using value_t = typename std::result_of< decltype( &tree::value )( n_ref ) >::type;
        //using value_t = decltype( static_cast< auto (tree::*)() > ( &tree::value ) );
        using value_t = decltype( std::declval<tree&>().value( n_ref() ) );

        using value_type        = typename tree::value_t;
        using reference         = value_t;
        using pointer           = value_type*;
        using difference_type   = long int;
        using iterator_category = std::forward_iterator_tag;

        tree *tre;
        std::vector< n_ref > nref_stack;
        std::vector< int > next = { 0 };

        tree_iterator< tree >() = default;
        tree_iterator< tree >( tree *tre, std::vector< n_ref > nref_stack ) : tre( tre ), nref_stack( nref_stack ) {}

        n_ref &get_noderef() { return nref_stack.back(); }

        void move_down() {
                if ( nref_stack.empty() ) return;
                while ( next.back() < tre->child_count( get_noderef() ) ) {
                        nref_stack.push_back( tre->child_at( get_noderef(), next.back() ) );
                        ++next.back();
                        next.push_back( 0 );
                }
        }
        void move_up() {
                next.pop_back();
                nref_stack.pop_back();
        }

        friend bool operator==( const tree_iterator< tree > &a, const tree_iterator< tree > &b ) {
                return ( a.tre == b.tre ) && ( a.nref_stack == b.nref_stack );
        }
        friend bool operator!=( const tree_iterator< tree > &a, const tree_iterator< tree > &b ) { return !(a == b); }

        tree_iterator &operator++() {
                move_up();
                move_down();
                return *this;
        }
        tree_iterator operator++(int) {
                auto it = (*this);
                ++(*this);
                return it;
        }
        reference operator*() {  return tre->value(  get_noderef() ); }
        pointer operator->() { return &tre->value( get_noderef() ); }

        void print() {
                std::cout << " Const it, value = "
                        << std::to_string( tre->value( get_noderef() ) ) << '\n';
        }
};

template< typename tree > struct const_tree_iterator {
        using n_ref = typename tree::node_ref;
        using value_t = decltype( std::declval<const tree&>().value( n_ref() ) );

        using value_type        = typename tree::value_t;
        using reference         = value_t;
        using pointer           = const value_type*;
        using difference_type   = long int;
        using iterator_category = std::forward_iterator_tag;

        const tree *tre;
        std::vector< n_ref > nref_stack;
        std::vector< int > next = { 0 };

        const_tree_iterator< tree >() = default;
        const_tree_iterator< tree >( const tree *tre, std::vector< n_ref > nref_stack ) : tre( tre ), nref_stack( nref_stack ) {}

        const n_ref &get_noderef() { return nref_stack.back(); }

        void move_down() {
                if ( nref_stack.empty() ) return;
                while ( next.back() < tre->child_count( get_noderef() ) ) {
                        nref_stack.push_back( tre->child_at( get_noderef(), next.back() ) );
                        ++next.back();
                        next.push_back( 0 );
                }
        }
        void move_up() {
                next.pop_back();
                nref_stack.pop_back();
        }

        friend bool operator==( const const_tree_iterator< tree > &a, const const_tree_iterator< tree > &b ) {
                return ( a.tre == b.tre ) && ( a.nref_stack == b.nref_stack );
        }
        friend bool operator!=( const const_tree_iterator< tree > &a, const const_tree_iterator< tree > &b ) { return !(a == b); }

        const_tree_iterator &operator++() {
                move_up();
                move_down();
                return *this;
        }
        const_tree_iterator operator++(int) {
                auto it = (*this);
                ++(*this);
                return it;
        }
        reference operator*() {  return tre->value(  get_noderef() ); }
        pointer operator->() { return &tre->value( get_noderef() ); }

        void print() {
                std::cout << " Const it, value = "
                        << std::to_string( tre->value( get_noderef() ) ) << '\n';
        }
};

template< typename tree >
tree_iterator< tree > tree_begin( tree &tre ) {
        using n_ref = typename tree::node_ref;
        std::vector<n_ref> nref_stack;
        if ( !tre.empty() ) { nref_stack.push_back(tre.root()); };
        tree_iterator<tree> it( &tre, nref_stack );
        it.move_down();
        return it;
}
template< typename tree >
tree_iterator< tree > tree_end( tree &tre ) {
        return tree_iterator<tree>( &tre, {} );
}


template< typename tree >
const_tree_iterator< tree > tree_begin( const tree &tre ) {
        using n_ref = typename tree::node_ref;
        std::vector<n_ref> nref_stack;
        if ( !tre.empty() ) { nref_stack.push_back(tre.root()); };
        const_tree_iterator<tree> it( &tre, nref_stack );
        it.move_down();
        return it;
}
template< typename tree >
const_tree_iterator< tree > tree_end( const tree &tre ) {
        return const_tree_iterator<tree>( &tre, {} );
}
