#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

#define MAX_BUFFERSIZE 512

void read_data_from_com( HANDLE serial, char *buffer, int buffersize )
{
    DWORD dwBytesRead = 0;
    if( !ReadFile( serial, buffer, buffersize - 1, &dwBytesRead, NULL ) ) printf( "no data read" );
}

void write_data_to_com( HANDLE serial, char *buffer, int buffersize )
{
    DWORD dwBytesWritten = 0;
    if ( !WriteFile( serial, buffer, buffersize - 1, &dwBytesWritten, NULL ) ) printf( "no data send" );
}

int main( int argc, char *argv[] )
{
    int assigned_btn = 0;
    int sample_count = 0;
    char filter[5] = {0};
    int filter_argc = 0;
    char effect[5] = {0};
    int effect_argc = {0};

    if ( argc > 1 )
    {
        for ( int i = 1; i < argc; ++i )
        {
            switch ( i )
            {
                case 1:
                    sscanf( argv[i], "%i", &assigned_btn );
                    break;
                case 2:
                    sscanf( argv[i], "%i", &sample_count );
                    break;
                case 3:
                    strcpy( filter, argv[i] );
                    break;
                case 4:
                    sscanf( argv[i], "%i", &filter_argc );
                    break;
                case 5:
                    strcpy( effect, argv[i] );
                    break;
                case 6:
                    sscanf( argv[i], "%i", &effect_argc );
                    break;
                default:
                    if ( argc < 6 + filter_argc + effect_argc + 3 )
                    {
                        printf( "invalid argument count\n" );
                        return -2;
                    }
                    break;
            }
        }
    }
    else
    {
        printf( "invalid argument count\n" );
        return -1;
    }

    printf( "assign to btn %i with %i samples \n", assigned_btn, sample_count );
    printf( "filter to use is %s and effect %s\n", filter, effect );
    
    HANDLE serial = CreateFile( "COM5", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
    if ( serial == INVALID_HANDLE_VALUE )
    {
        if ( GetLastError() == ERROR_FILE_NOT_FOUND )
        {
            printf( "invalid port\n" );
        }
        else
        {
            printf( "other error\n" );
        }
    }
    
    printf( "continue\n" );

    //fill buffer to write
    char buffer[MAX_BUFFERSIZE] = {0};
    sprintf( buffer, "%i,%i,%s,%i,%s,%i", assigned_btn, sample_count, filter, filter_argc, effect, effect_argc );

    for ( int i = 6; i < argc; ++i )
    {
        sprintf( buffer, "%s,%s", buffer, argv[i] );
    }

    write_data_to_com( serial, buffer, MAX_BUFFERSIZE );
    
    CloseHandle( serial );
    return 0;
}

