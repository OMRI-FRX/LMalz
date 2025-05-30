/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DeviceFile.cpp
 * Author: p.podwapinski
 * 
 * Created on March 6, 2017, 10:26 AM
 */

#include "log.h"
#include "DeviceFile.h"

namespace rawio 
{
    
const File::OpenParameters DeviceFile::DEV_READ = { 
    GENERIC_READ, 
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    OPEN_EXISTING
    };

const File::OpenParameters DeviceFile::DEV_WRITE = { 
    GENERIC_READ | GENERIC_WRITE, 
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    OPEN_EXISTING
    };

DeviceFile::DeviceFile() 
    : File()
{
}


bool DeviceFile::open(const ::std::wstring& path, const bool writable )
{
    const OpenParameters& openParams = writable ? DEV_WRITE : DEV_READ;
    return File::open( path, openParams );
}

bool DeviceFile::ioCtl( const DWORD ctlCode, void* output, size_t size ) 
{
    bool result = false;

    if ( getNativeHandle() != INVALID_HANDLE_VALUE )
    {
        DWORD readB = 0U;
        if ( 0 != ::DeviceIoControl( getNativeHandle()
                            , ctlCode
                            , NULL
                            , 0
                            , output
                            , size
                            , &readB,
                            NULL
                            ) )
        {
            result = true;
        }
        else
        {
            LOG_E( L"IoControl failed: " << ::GetLastError() << L"(" << ctlCode << L"," << size << L"," << readB << L")" );
        }
    }    

    return result;     
}

    
};

