//
// State.cpp
// Copyright (c) Charles Baker. All rights reserved.
//

#include "State.hpp"
#include "Item.hpp"
#include "Transition.hpp"
#include "assert.hpp"
#include <stdio.h>

using namespace sweet;
using namespace sweet::lalr;

using std::set;

#if defined(BUILD_PLATFORM_MSVC)
#define snprintf _snprintf
#endif

/**
// Constructor.
*/
State::State()
: items_(),
  transitions_(),
  processed_( false ),
  index_( INVALID_INDEX )
{
}

/**
// Add an item to this state.
//
// @param production
//  The production of the item to add.
//
// @param position
//  The position of the dot in the item to add.
//
// @return
//  The number of items added (0 or 1).
*/
int State::add_item( Production* production, int position )
{
    SWEET_ASSERT( production );
    return items_.insert( Item(production, position) ).second ? 1 : 0;
}

/**
// Add the symbols in \e lookahead_symbols to the item in this state for 
// \e production at \e next_nodes.
//
// @param production
//  The production of the item to add \e lookahead_symbols to.
//
// @param position
//  The position of the dot in the item to add \e lookahead_symbols to.
//
// @param lookahead_symbols
//  The lookahead symbols to add to the item in this state.
//
// @return
//  The number of lookahead symbols added.
*/
int State::add_lookahead_symbols( Production* production, int position, const std::set<const LalrSymbol*>& lookahead_symbols )
{
    SWEET_ASSERT( production );
    std::set<Item>::iterator item = items_.find( Item(production, position) );
    SWEET_ASSERT( item != items_.end() );
    return item->add_lookahead_symbols( lookahead_symbols );
}

/**
// Get the items in this state.
//
// @return
//  The items.
*/
const std::set<Item>& State::get_items() const
{
    return items_;
}

/**
// Describe this state.
//
// @param description
//  A variable to append the description of this state to (assumed not 
//  null).
*/
void State::describe( std::string* description ) const
{
    SWEET_ASSERT( description );

    char buffer [32];
    snprintf( buffer, sizeof(buffer), "%d:\n", get_index() );
    buffer [sizeof(buffer) - 1] = '\0';
    description->append( buffer );

    std::set<Item>::const_iterator item = items_.begin(); 
    while ( item != items_.end() )
    {
        item->describe( description );
        description->append( "\n" );
        ++item;
    }

    std::set<Transition>::const_iterator transition = transitions_.begin();
    while ( transition != transitions_.end() )
    {
        transition->describe( description );
        description->append( "\n" );
        ++transition;
    }
}

/**
// Describe this state.
//
// @return
//  A string describing this state.
*/
std::string State::description() const
{
    std::string description;
    description.reserve( 1024 );
    describe( &description );
    return description;
}

/**
// Add a transition on \e symbol from this state to \e state.
//
// @param symbol
//  The symbol to transition on (assumed not null).
//
// @param state
//  The state to transition to (assumed not null).
*/
void State::add_transition( const LalrSymbol* symbol, State* state )
{
    SWEET_ASSERT( symbol );
    SWEET_ASSERT( state );
    SWEET_ASSERT( transitions_.find(Transition(symbol, state)) == transitions_.end() );
    transitions_.insert( Transition(symbol, state) );
}

/**
// Add a reduction to \e production from this state on \e symbol.
//
// @param symbol
//  The symbol to make the reduction on.
//
// @param reduced_symbol
//  The symbol that is reduced to.
//
// @param reduced_length
//  The number of symbols on the right-hand side of the production that is
//  reduced.
//
// @param action
//  The index of the action taken on the reduction or 
//  `Action::INVALID_INDEX` if no action is taken.
*/
void State::add_transition( const LalrSymbol* symbol, const LalrSymbol* reduced_symbol, int reduced_length, int precedence, int action )
{
    SWEET_ASSERT( symbol );
    SWEET_ASSERT( reduced_symbol );
    SWEET_ASSERT( reduced_length >= 0 );
    SWEET_ASSERT( precedence >= 0 );

    std::set<Transition>::iterator transition = transitions_.find( Transition(symbol, reduced_symbol, reduced_length, precedence, action) );
    if ( transition != transitions_.end() )
    {        
        SWEET_ASSERT( transition->get_type() == TRANSITION_SHIFT );
        transition->override_shift_to_reduce( reduced_symbol, reduced_length, precedence, action );
    }
    else
    {
        transition = transitions_.insert( Transition(symbol, reduced_symbol, reduced_length, precedence, action) ).first;
    }
}

/**
// Add a reduction to \e production from this state on any of the symbols in 
// \e symbols.
//
// @param symbols
//  The symbols to make the reduction on.
//
// @param reduced_symbol
//  The symbol that is reduced to.
//
// @param reduced_length
//  The number of symbols on the right-hand side of the production that is
//  reduced.
//
// @param action
//  The index of the action taken on the reduction or 
//  `Action::INVALID_INDEX` if no action is taken.
*/
void State::add_transition( const std::set<const LalrSymbol*>& symbols, const LalrSymbol* reduced_symbol, int reduced_length, int precedence, int action )
{
    SWEET_ASSERT( reduced_symbol );
    SWEET_ASSERT( reduced_length >= 0 );
    SWEET_ASSERT( precedence >= 0 );
    for ( set<const LalrSymbol*>::const_iterator i = symbols.begin(); i != symbols.end(); ++i )
    {
        const LalrSymbol* symbol = *i;
        SWEET_ASSERT( symbol );
        add_transition( symbol, reduced_symbol, reduced_length, precedence, action );
    }
}

/**
// Find a transition on \e symbol from this state.
//
// @param symbol
//  The symbol to find a transition for.
//
// @return
//  The transition or null if there is no transition on \e symbol from this
//  state.
*/
Transition* State::find_transition_by_symbol( const LalrSymbol* symbol )
{    
    Transition* transition = NULL;  
      
    if ( symbol )
    {
        std::set<Transition>::iterator i = transitions_.begin();
        while ( i != transitions_.end() && !i->is_symbol(symbol) )
        {
            ++i;
        }
        transition = i != transitions_.end() ? const_cast<Transition*>(&(*i)) : NULL;
    }    
    
    return transition;
}

/**
// Find a transition on \e symbol from this state.
//
// @param symbol
//  The symbol to find a transition for.
//
// @return
//  The transition or null if there is no transition on \e symbol from this
//  state.
*/
const Transition* State::find_transition_by_symbol( const LalrSymbol* symbol ) const
{    
    const Transition* transition = NULL;
    
    if ( symbol )
    {
        std::set<Transition>::const_iterator i = transitions_.begin();
        while ( i != transitions_.end() && !i->is_symbol(symbol) )
        {
            ++i;
        }
        transition = i != transitions_.end() ? &(*i) : NULL;
    }
    
    return transition;
}

/**
// Generate indices for the transitions in this state.
*/
void State::generate_indices_for_transitions()
{
    int index = 0;
    for ( std::set<Transition>::iterator transition = transitions_.begin(); transition != transitions_.end(); ++transition )
    {
        transition->set_index( index );
        ++index;
    }
}

/**
// Get the transitions from this state.
//
// @return
//  The transitions.
*/
const std::set<Transition>& State::get_transitions() const
{
    return transitions_;
}

/**
// Set this state as having been processed.
//
// @param processed
//  True to mark this state as processed or false to mark it as not processed.
*/
void State::set_processed( bool processed )
{
    processed_ = processed;
}

/**
// Has this state been processed?
//
// @return
//  True if this state has been processed otherwise false.
*/
bool State::is_processed() const
{
    return processed_;
}

/**
// Set the index of this state.
//
// @param index
//  The value to set the index of this state to.
*/
void State::set_index( int index )
{
    index_ = index;
}

/**
// Get the index of this state.
//
// @return
//  The index of this state.
*/
int State::get_index() const
{
    return index_;
}

/**
// Less than operator.
//
// @param state
//  The state to compare this state with.
//
// @return
//  True if the items in this state are less than the items in \e state.
*/
bool State::operator<( const State& state ) const
{
    return std::lexicographical_compare( items_.begin(), items_.end(), state.items_.begin(), state.items_.end() );
}
