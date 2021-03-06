#ifdef GE_WIN32
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> /* gethostbyname */
#define closesocket(s) close(s)
#define ioctlsocket ioctl
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;
#endif
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <cstdlib>

#include "Instance.h"
#include "Socket.h"
#include "Debug.h"

using namespace GE;

Socket::Socket( const std::string& server, unsigned short port, PortType type )
	: mSocket( -1 )
	, mSin( nullptr )
{
	if ( server != "" and port != 0 and ( type == TCP or type == UDP ) ) {
		Connect( server, port, type );
	}
}


Socket::~Socket()
{
}


int Socket::Connect( const std::string& server, short unsigned int port, PortType type )
{
	struct hostent* hp = 0;
	struct in_addr addr;
	mSin = Instance::baseInstance()->Malloc( sizeof( struct sockaddr_in ) );
	struct sockaddr_in* sin = ( struct sockaddr_in* )mSin;

	int ptype = SOCK_STREAM;
	if ( type == UDP ) {
		ptype = SOCK_DGRAM;
	}

	addr.s_addr = inet_addr( server.c_str() );
	hp = gethostbyname( server.c_str() );

	if ( hp and hp->h_addr_list and hp->h_addr_list[0] ) {
		sin->sin_addr = *(IN_ADDR *) hp->h_addr;
	} else {
		sin->sin_addr = addr;
	}
	sin->sin_family = AF_INET;
	sin->sin_port = htons( port );
	mSocket = socket( hp ? hp->h_addrtype : AF_INET, ptype, 0 );

	int ret = ( mSocket < 0 ) ? mSocket : connect( mSocket, (SOCKADDR*)sin, sizeof( *sin ) );

	if ( ret < 0 ) {
		gDebug() << "Socket error : " << strerror( errno ) << "\n";
		if ( mSocket >= 0 ) {
			closesocket( mSocket );
		}
		mSocket = -1;
	}
	return ret;
}


int Socket::Send( const void* data, size_t size )
{
	return send( mSocket, (const char*)data, size, 0 );
}


int Socket::Receive( void* _data, size_t size, bool fixed )
{
	uint8_t* data = (uint8_t*)_data;
	int ret = 0;

	for ( std::list< Buffer >::iterator it = mRecvQueue.begin(); it != mRecvQueue.end(); ) {
		if ( size < (*it).s ) {
			memcpy( data + ret, (*it).p, size );
			memmove( (*it).p, (*it).p + size, (*it).s - size );
			(*it).s -= size;
			ret += size;
			break;
		} else {
			memcpy( data + ret, (*it).p, (*it).s );
			ret += (*it).s;
			Instance::baseInstance()->Free( (*it).p );
			it = mRecvQueue.erase( it );
		}
	}

	while ( ( fixed and ret < (int)size ) or ( not fixed and ret == 0 ) ) {
		int needed = size - ret;
#ifdef GE_WIN32
		unsigned long int check_size = 0;
#else
		int check_size = 0;
#endif
		while ( check_size == 0 ) {
			ioctlsocket( mSocket, FIONREAD, &check_size );
			usleep( 0 );
		}
#ifndef GE_WIN32
		if ( check_size < 0 ) {
			return -1;
		}
#endif
		uint8_t* os_buf = (uint8_t*)Instance::baseInstance()->Malloc( check_size );
		int os_size = recv( mSocket, (char*)os_buf, check_size, 0 );
#ifdef GE_WIN32
		if ( os_size < 0 ) {
			return -1;
		}
#endif

		memcpy( data + ret, os_buf, std::min( needed, os_size ) );

		if ( os_size < needed ) {
			ret += os_size;
		} else {
			ret += needed;
			Buffer buf;
			buf.s = os_size - needed;
			buf.p = (uint8_t*)Instance::baseInstance()->Malloc( buf.s );
			memcpy( buf.p, os_buf + needed, buf.s );
			mRecvQueue.push_back( buf );
		}
		Instance::baseInstance()->Free( os_buf );
	}

	return ret;
}


int Socket::Send( const std::string& s )
{
	return Send( s.c_str(), s.length() + 1 );
}


int Socket::HTTPGet( const std::string& url, const std::map< std::string, std::string >& args )
{
	std::string str = url;
	bool first = true;

	for ( std::map< std::string, std::string >::const_iterator it = args.begin(); it != args.end(); it++ ) {
		if ( first ) {
			str += "?";
		} else {
			str += "&";
		}
		str += (*it).first + "=" + (*it).second;
		first = false;
	}

	return Send( "GET " + str + " HTTP/1.1\r\n\r\n" );
}


std::string Socket::Receive()
{
	std::string str;
	uint8_t buf[65];

	while ( true ) {
		memset( buf, 0, sizeof(buf) );
		int ret = Receive( buf, sizeof(buf) - 1 );
		if ( ret <= 0 ) {
			break;
		}
		str += std::string( (char*)buf );
		if ( memchr( buf, 0x00, ret ) ) {
			if ( memchr( buf, 0x00, ret ) < buf + ret - 1 ) {
				Buffer mem;
				mem.s = ret - ( (uint8_t*)memchr( buf, 0x00, ret ) - buf ) - 1;
				mem.p = (uint8_t*)Instance::baseInstance()->Malloc( mem.s );
				memcpy( mem.p, (uint8_t*)memchr( buf, 0x00, ret ) + 1, mem.s );
				mRecvQueue.push_back( mem );
			}
			break;
		}
	}

	return str;
}


std::string Socket::HTTPResponse( std::string* pHeader )
{
	std::string header;
	std::string str;
	uint8_t buf[65];
	int content_length = 0;
	uint8_t* header_end = nullptr;
	uint8_t header_end_len = 2;

	while ( true ) {
		memset( buf, 0, sizeof(buf) );
		int ret = Receive( buf, sizeof(buf) - 1 );
		if ( ret <= 0 ) {
			break;
		}
		header += std::string( (char*)buf );

		uint8_t* s = (uint8_t*)header.c_str();
		if ( strstr( (char*)s, "Content-Length" ) and strrchr( (char*)s, '\n' ) > strstr( (char*)s, "Content-Length" ) ) {
			content_length = std::atoi( strstr( strstr( strstr( (char*)s, "Content-Length" ), ":" ), " " ) + 1 );
		}

		header_end = (uint8_t*)strstr( (char*)s, "\n\n" );
		header_end_len = 2;
		if ( !header_end ) {
			header_end = (uint8_t*)strstr( (char*)s, "\r\n\r\n" );
			header_end_len = 4;
		}
		if ( header_end ) {
			str = header.substr( ( header_end - s ) + header_end_len );
			header = header.substr( 0, header_end - s );
			break;
		}
	}

	if ( pHeader ) {
		*pHeader = header;
	}

	if ( (int)str.length() > content_length ) {
		int trailing = str.length() - content_length;
		Buffer mem;
		mem.s = trailing;
		mem.p = (uint8_t*)Instance::baseInstance()->Malloc( trailing );
		memcpy( mem.p, str.c_str() + content_length, mem.s );
		mRecvQueue.push_back( mem );
		str = str.substr( 0, content_length );
	}

	uint8_t* b = (uint8_t*)Instance::baseInstance()->Malloc( content_length + 1 );
	memset( b, 0, content_length + 1 );
	Receive( b, content_length );
	str += std::string( (char*)b );
	Instance::baseInstance()->Free( b );
	return str;
}
