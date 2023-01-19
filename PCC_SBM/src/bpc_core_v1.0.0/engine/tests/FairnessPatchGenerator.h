#ifndef __FairnessPatchGenerator_h__
#define __FairnessPatchGenerator_h__

#include "File.h"
#include "Stringf.h"
#include "ThreadStateGraph.h"

class FairnessPatchGenerator
{
public:
    void generate( const StringTag &outputFile, const StringTag &patchName, ThreadStateGraph &graph, const StringTag &targetState )
    {
        storeInput( outputFile, patchName, graph, targetState );
        createHeader();
        createConstructor();
        createFooter();
    }

    ~FairnessPatchGenerator()
    {
        if ( _output )
        {
            delete _output;
            _output = NULL;
        }
    }

private:
    ThreadStateGraph *_graph;
    File *_output;
    StringTag _patchName;
    StringTag _targetState;

    void storeInput( const StringTag &outputFile, const StringTag &patchName, ThreadStateGraph &graph, const StringTag &targetState )
    {
        _output = new File( outputFile );
        _output->open( IFile::MODE_WRITE );

        _graph = &graph;
        _targetState = targetState;
        _patchName = patchName;
    }

    void createHeader()
    {
        _output->write( Stringf( "#ifndef __%s_h__\n", _patchName.ascii() ) );
        _output->write( Stringf( "#define __%s_h__\n", _patchName.ascii() ) );
        _output->write( "\n" );
        _output->write( "#include \"FairnessPatchBase.h\"" );
        _output->write( "\n" );
        _output->write( "\n" );
        _output->write( Stringf( "class %s : public FairnessPatchBase\n", _patchName.ascii() ) );
        _output->write( "{\n" );
        _output->write( "public:\n" );
        _output->write( "\n" );
    }

    void createConstructor()
    {
        _output->write( Stringf( "\t%s()\n", _patchName.ascii() ) );
        _output->write( "\t{\n" );
        _output->write( Stringf( "\t\t_currentState = \"%s\";\n", _graph->_initialState.ascii() ) );
        _output->write( "\n" );

        dumpStateGraph();
        dumpTargetState();

        _output->write( "\t}\n" );
    }

    void dumpStateGraph()
    {
        _output->write( "\t\tTransitions emptyTransitions;\n" );

        Map<StringTag, ThreadState>::iterator it;

        for ( it = _graph->_graph.begin(); it != _graph->_graph.end(); ++it )
        {
            _output->write( Stringf( "\t\t_stateToTransitions[\"%s\"] = emptyTransitions;\n",
                                     it->second._label.ascii() ) );
        }

        for ( it = _graph->_graph.begin(); it != _graph->_graph.end(); ++it )
        {
            _output->write( "\n" );
            for ( unsigned i = 0; i < it->second._successors.size(); ++i )
            {
                _output->write( Stringf( "\t\t_stateToTransitions[\"%s\"].append( Transition( %u, \"%s\" ) );\n",
                                         it->second._label.ascii(),
                                         it->second._successors[i]._event.type(),
                                         it->second._successors[i]._state.ascii() ) );
            }
        }
    }

    void dumpTargetState()
    {
        _output->write( "\n" );
        _output->write( Stringf( "\t\t_targetState = \"%s\";\n", _targetState.ascii() ) );
    }

    void createFooter()
    {
        _output->write( "};\n" );
        _output->write( "\n" );
        _output->write( Stringf( "#endif // __%s_h__", _patchName.ascii() ) );
    }
};

#endif // __FairnessPatchGenerator_h__

//
// Local Variables:
// compile-command: "make -C ../.. "
// tags-file-name: "../../TAGS"
// c-basic-offset: 4
// End:
//
