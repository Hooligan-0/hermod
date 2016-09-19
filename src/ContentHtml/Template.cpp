/*
 * Hermod - Modular application framework
 *
 * Hermod is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License 
 * version 3 as published by the Free Software Foundation. You
 * should have received a copy of the GNU Lesser General Public
 * License along with this program, see LICENSE file for more details.
 * This program is distributed WITHOUT ANY WARRANTY see README file.
 *
 * The Template layer is based on NLTemplate, an HTML templatnig library
 * for C++. See https://github.com/catnapgames/NLTemplate for the original
 * project.
 *
 * Authors: Tomas Andrle (Original library)
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Template.hpp"

using namespace std;

namespace hermod {
	namespace contentHtml {

enum {
    TOKEN_END,
    TOKEN_TEXT,
    TOKEN_BLOCK,
    TOKEN_ENDBLOCK,
    TOKEN_INCLUDE,
    TOKEN_VAR
};


static inline bool alphanum( const char c ) {
    return
    ( c >= 'a' && c <= 'z' ) ||
    ( c >= 'A' && c <= 'Z' ) ||
    ( c >= '0' && c <= '9' ) ||
    ( c == '_' ) ||
    ( c == '.' );
}


static inline long match_var( const char *text, string & result ) {
    if (text[ 0 ] != '{' ||
        text[ 1 ] != '{' ||
        text[ 2 ] != ' ' )
    {
        return -1;
    }
    
    const char *var = text + 3;
    const char *cursor = var;
    
    while ( *cursor ) {
        if (cursor[ 0 ] == ' ' &&
            cursor[ 1 ] == '}' &&
            cursor[ 2 ] == '}' )
        {
            result = string( var, cursor - var );
            return cursor + 3 - text;
        }
        
        if ( !alphanum( *cursor ) ) {
            return -1;
        }
        
        cursor++;
    }
    
    return -1;
}


static inline long match_tag_with_param( const char *tag, const char *text, string & result ) {
    if (text[ 0 ] != '{' ||
        text[ 1 ] != '%' ||
        text[ 2 ] != ' ')
    {
        return -1;
    }

    long taglen = strlen( tag );
    if ( strncmp( text + 3, tag, taglen ) ) {
        return -1;
    }

    const char *param = text + 3 + taglen;
    
    if ( *param != ' ' ) {
        return -1;
    }
    
    param++;

    const char *cursor = param;

    while ( *cursor ) {
        if (cursor[ 0 ] == ' ' &&
            cursor[ 1 ] == '%' &&
            cursor[ 2 ] == '}' )
        {
            result = string( param, cursor - param );
            return cursor + 3 - text;
        }

        if ( !alphanum( *cursor ) ) {
            return -1;
        }
        
        cursor++;
    }
    
    return -1;
}

// ----------------------------------------------------------------------------
// -- Template --
// ----------------------------------------------------------------------------

/**
 * @brief Default constructor
 *
 */
Template::Template( ) : Block( "main" )
{
	// Nothing more to do
}

/**
 * @brief Clean (remove) all known items for the current template
 *
 */
void Template::clear()
{
    for ( size_t i=0; i < fragments.size(); i++ ) {
        delete fragments[ i ];
    }
    for ( size_t i=0; i < nodes.size(); i++ ) {
        delete nodes[ i ];
    }
    nodes.clear();
    fragments.clear();
    properties.clear();
}

/**
 * @brief Internal method to create the item tree from the raw template text
 *
 * @param loader Pointer to the Loader object used to read source data
 * @param files  Reference to a Tokenizer vector used during load
 * @param nodes  Reference to a Node vector used during load
 */
void Template::load(Loader *loader, std::vector<Tokenizer*> & files, std::vector<Node*> & nodes )
{

	try {
		const char *buffer = loader->get();
		Tokenizer *tokenizer = new Tokenizer( buffer );
		files.push_back( tokenizer );
	} catch (...) {
		// ToDo: log error ?
		return;
	}
    
    bool done = false;
    while( !done ) {
        Token token = files.back()->next();
        switch ( token.type ) {
            case TOKEN_END:
                done = true;
                break;
            case TOKEN_BLOCK: {
                Block *block = new Block( token.value );
                nodes.back()->fragments.push_back( block );
                nodes.push_back( block );
            }
                break;
            case TOKEN_ENDBLOCK:
                nodes.pop_back();
                break;
            case TOKEN_VAR:
                nodes.back()->fragments.push_back( new Property( token.value ) );
                break;
            case TOKEN_TEXT:
                nodes.back()->fragments.push_back( new Text( token.value ) );
                break;
            case TOKEN_INCLUDE:
                Loader *incLoader = 0;
                try {
                    incLoader = loader->getInclude();
                    incLoader->load(token.value);
                    load(incLoader, files, nodes );
                    delete incLoader;
                } catch (...) {
                    if (incLoader)
                        delete incLoader;
                }
                break;
        }
    }
    
    delete files.back();
    files.pop_back();
}

/**
 * @brief Read a template file and load it
 *
 * @param filename String that contains the file name (and path)
 */
void Template::loadFile(const std::string &filename)
{
    clear();
    vector<Node*> stack;
    stack.push_back( this );
    
    vector<Tokenizer*> file_stack;
    
    LoaderFile ldr;
    ldr.load( filename.c_str() );

    load(&ldr, file_stack, stack );
}

/**
 * @brief Render recursively the item tree, and write data to an Output object
 *
 * @param output Reference to the Output object
 */
void Template::render( Output & output ) const
{
	Node::render( output, *this );
}

/**
 * @brief Render recursively the item tree, and write data to a byte vector
 *
 * @param vec Pointer to the byte vector
 */
void Template::render( std::vector<unsigned char> *vec ) const
{
	OutputVector output(vec);
	Node::render( output, *this );
}

// ----------------------------------------------------------------------------


Tokenizer::Tokenizer( const char *text ) :
text( text ),
len( strlen( text ) ),
pos( 0 ),
peeking( false )
{
}


Tokenizer::~Tokenizer() {
    free( (void*) text );
}


Token Tokenizer::next() {
    static const char * s_endblock = "{% endblock %}";
    static const char * s_block = "block";
    static const char * s_include = "include";
    static const long s_endblock_len = strlen( s_endblock );
    
    if ( peeking ) {
        peeking = false;
        return peek;
    }
    
    Token token;
    token.value.clear();
    peek.value.clear();
    token.type = TOKEN_END;
    peek.type = TOKEN_END;
    
    long textpos = pos;
    long textlen = 0;
    
a:
    if ( pos < len ) {
        long m = match_tag_with_param( s_block, text + pos, peek.value );
        if ( m > 0 ) {
            peek.type = TOKEN_BLOCK;
            pos += m;
        } else if ( !strncmp( s_endblock, text + pos, s_endblock_len ) ) {
            peek.type = TOKEN_ENDBLOCK;
            pos += s_endblock_len;
        } else if ( ( m = match_tag_with_param( s_include, text + pos, peek.value ) ) > 0 ) {
            peek.type = TOKEN_INCLUDE;
            pos += m;
        } else if ( ( m = match_var( text + pos, peek.value ) ) > 0 ) {
            peek.type = TOKEN_VAR;
            pos += m;
        } else {
            textlen ++;
            pos ++;
            peeking = true;
            goto a;
        }
    }

    if ( peeking ) {
        token.type = TOKEN_TEXT;
        token.value = string( text + textpos, textlen );
        return token;
    }

    return peek;
}


const string Dictionary::find( const string & name ) const {
    for ( size_t i=0; i < properties.size(); i++ ) {
        if ( properties[ i ].first == name ) {
            return properties[ i ].second;
        }
    }
    return "";
}


void Dictionary::set( const string & name, const string & value ) {
    for ( size_t i=0; i < properties.size(); i++ ) {
        if ( properties[ i ].first == name ) {
            properties[ i ].second = value;
            return;
        }
    }
    properties.push_back( pair<string, string>( name, value ) );
}


Fragment::~Fragment() {
}


bool Fragment::isBlockNamed( const string & ) const {
    return false;
}



Text::Text( const string & text ) : text( text ) {
}


void Text::render( Output & output, const Dictionary & ) const {
    output.print( text );
}


Fragment *Text::copy() const {
    return new Text( text );
}


Property::Property( const string & name ) : name( name ) {
}


void Property::render( Output & output, const Dictionary & dictionary ) const {
    output.print( dictionary.find( name ) );
}


Fragment *Property::copy() const {
    return new Property( name );
}


Node::~Node() {
    for ( size_t i=0; i < fragments.size(); i++ ) {
        delete fragments[ i ];
    }
}


Fragment *Node::copy() const {
    Node *node = new Node();
    node->properties = properties;
    for ( size_t i=0; i < fragments.size(); i++ ) {
        node->fragments.push_back( fragments[ i ]->copy() );
    }
    return node;
}


void Node::render( Output & output, const Dictionary & ) const {
    for ( size_t i=0; i < fragments.size(); i++ ) {
        fragments[ i ]->render( output, *this );
    }
}



Block & Node::block( const string & name ) const {
    for ( size_t i=0; i < fragments.size(); i++ ) {
        if ( fragments[ i ]->isBlockNamed( name ) ) {
            return *dynamic_cast<Block*>( fragments[ i ] );
        }
    }
    throw 0;
}


Block::Block( const string & name ) : name( name ), enabled( true ), resized( false ) {
}


Fragment *Block::copy() const {
    Block *block = new Block( name );
    block->properties = properties;
    for ( size_t i=0; i < fragments.size(); i++ ) {
        block->fragments.push_back( fragments[ i ]->copy() );
    }
    return block;
}


Block::~Block() {
    for ( size_t i=0; i < nodes.size(); i++ ) {
        delete nodes[ i ];
    }
}


bool Block::isBlockNamed( const string & name ) const {
    return this->name == name;
}


void Block::enable() {
    enabled = true;
}


void Block::disable() {
    enabled = false;
}

void Block::repeat( size_t n ) {
    resized = true;
    for ( size_t i=0; i < nodes.size(); i++ ) {
        delete nodes[ i ];
    }
    nodes.clear();
    for ( size_t i=0; i < n; i++ ) {
        nodes.push_back( static_cast<Node*>( copy() ) );
    }
}


Node & Block::operator[]( size_t index ) {
    return *nodes.at( index );
}


void Block::render( Output & output, const Dictionary & ) const {
    if ( enabled ) {
        if ( resized ) {
            for ( size_t i=0; i < nodes.size(); i++ ) {
                nodes[ i ]->render( output, *nodes[ i ] );
            }
        } else {
            Node::render( output, *this );
        }
    }
}


Output::~Output() {
}


void OutputString::print( const string & text ) {
    buf << text;
}


void OutputStdout::print( const std::string &text ) {
    cout << text;
}

OutputVector::OutputVector(std::vector<unsigned char> *vec)
{
    mVector = vec;
}

void OutputVector::print( const std::string &text )
{
	const char *str = text.c_str();
	std::copy(str,
	          str + text.length(),
	          std::back_inserter(*mVector));
	return;
}

// ----------------------------------------------------------------------------
// --                              Data Loaders                              --
// ----------------------------------------------------------------------------

Loader::Loader()
{
    mDataBuffer = 0;
}

Loader::~Loader()
{
}

const char *Loader::get(void)
{
	if (mDataBuffer == 0)
		throw -1;

	return mDataBuffer;
}

LoaderFile::LoaderFile()
{
}

Loader *LoaderFile::getInclude(void)
{
    LoaderFile *inc = new LoaderFile;
    inc->mPath = mPath;
    return inc;
}

bool LoaderFile::load(const std::string &filename)
{
	std::string fullname(mPath);
	fullname += filename;
	FILE *f = fopen(fullname.c_str(), "rb" );
	if (f == 0)
		return false;
	std::size_t sep = filename.rfind('/');
	if (sep == std::string::npos)
		mFilename = filename;
	else
	{
	        mPath += filename.substr(0, sep + 1);
	        mFilename = filename.substr(sep+1, std::string::npos);
	}
	
	fseek( f, 0, SEEK_END );
	long len = ftell( f );
	fseek( f, 0, SEEK_SET );

	mDataBuffer = (char*) malloc( len + 1 );
	fread( (void*)mDataBuffer, len, 1, f );
	fclose( f );
	mDataBuffer[ len ] = 0;
	return true;
}

	} // namespace contentHtml
} // namespace hermod
/* EOF */
