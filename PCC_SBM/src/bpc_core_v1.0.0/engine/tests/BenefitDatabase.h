#ifndef __BenefitDatabase_h__
#define __BenefitDatabase_h__

#include "Stringf.h"
#include "Map.h"

struct Link
{
    Link( const StringTag &state, Link *prev, Link *next ) : _state( state ), _prev( prev ), _next( next )
    {
    }

    StringTag _state;
    Link *_prev;
    Link *_next;
};

class BenefitDatabase
{
public:
    BenefitDatabase() : _highest( 0 )
    {
    }

    Map<unsigned, Link *> _scoreToStates;
    Map<StringTag, unsigned> _stateToScore;
    Map<StringTag, Link *> _stateToLink;
    unsigned _highest;

    StringTag getHighest()
    {
        return _scoreToStates[_highest]->_state;
    }

    void addState( const StringTag &state, unsigned score )
    {
        if ( !_scoreToStates.exists( score ) )
        {
            _scoreToStates[score] = NULL;
        }

        Link *link = new Link( state, NULL, _scoreToStates[score] );

        if ( link->_next != NULL )
        {
            link->_next->_prev = link;
        }

        _scoreToStates[score] = link;
        _stateToLink[state] = link;
        _stateToScore[state] = score;

        if ( score > _highest )
        {
            _highest = score;
        }
    }

    void removeState( const StringTag &state )
    {
        if ( !_stateToLink.exists( state ) )
        {
            TS_TRACE( "Error in remove!" );
        }
        if ( !_stateToScore.exists( state ) )
        {
            TS_TRACE( "Error in remove!" );
        }

        Link *link = _stateToLink[state];

        if ( link->_prev != NULL )
        {
            link->_prev->_next = link->_next;
        }
        else
        {
            _scoreToStates[_stateToScore[state]] = link->_next;
        }

        if ( link->_next != NULL )
        {
            link->_next->_prev = link->_prev;
        }

        if ( ( link->_prev == NULL ) && ( link->_next == NULL ) && ( _stateToScore[state] == _highest ) )
        {
            adjustHighest();
        }

        _stateToLink.erase( state );
        _stateToScore.erase( state );

        delete link;
    }

    void adjustHighest()
    {
        while ( _highest > 0 )
        {
            if ( _scoreToStates.exists( _highest ) && ( _scoreToStates[_highest] != NULL ) )
            {
                return;
            }

            --_highest;
        }
    }

    void decrementBenefit( const StringTag &state )
    {
        if ( !_stateToScore.exists( state ) )
        {
            TS_TRACE( "Error in decrement" );
        }

        unsigned newScore = _stateToScore[state] - 1;

        removeState( state );
        addState( state, newScore );
    }
};

#endif // __BenefitDatabase_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//
