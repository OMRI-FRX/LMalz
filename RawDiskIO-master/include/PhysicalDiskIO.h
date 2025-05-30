/*
 * Copyright (c) 2017, p.podwapinski
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* 
 * File:   PhysicalDiskIO.h
 * Author: p.podwapinski
 *
 * Created on March 7, 2017, 10:23 AM
 */

#ifndef PHYSICALDISKIO_H
#define PHYSICALDISKIO_H

#include <string>
#include "PhysicalDiskInfo.h"

namespace rawio
{
    class PhysicalDiskIO 
    {
    public:
        explicit PhysicalDiskIO( const PhysicalDiskInfo& info );
        ~PhysicalDiskIO()
        {            
        }
        
        /**
         * Dump the whole binary disk content into a file
         * @param outFile
         * @return 
         */
        bool dump( const ::std::wstring& outFile ) const;
        
    private:
        const PhysicalDiskInfo& m_info;
    };    
};

#endif /* PHYSICALDISKIO_H */

